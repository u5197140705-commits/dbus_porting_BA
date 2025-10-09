/*******************************************************************************
 *  Copyright (c) 2022 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          Generic SW
 *  COMP_ABBREV      DBCDRV
 ******************************************************************************/


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *  \ingroup dbuscan
 *
 *  \brief   Implementation of generic drivers for the DBusCAN chip
 *
 */


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include <stdint.h>
#include "dbuscan_drv.h"
#include "dbuscan_dbus.h"
#include "math/mathDiv.h"
#include "utility.h"
#ifdef DBUSCAN_WITH_BBL_SPI
#include "dbuscan_bbl.h"
#endif
#ifdef DBUSCAN_SPI_CRC_USED
    #include "stdcrc/StdCrc.h"
#endif


/******************************************************************************/
/* PRIVATE DEFINITIONS                                                        */
/******************************************************************************/
#define DBCDRV_RESET_TIME_US                   (700u) ///< The time (in microseconds) after a reset event before the device is ready
#define DBCDRV_REG_SIZE                          (4u) ///< The number of bytes of DBusCAN chip registers
#define DBCDRV_DBUS_TXS_FIFO_SIZE               (64u) ///< The number of bytes to be allocated in DBusCAN's RAM for the DBus Tx Status FIFO buffer. 64 bytes is maximum, must be a multiple of 8 bytes.
#define DBCDRV_DBUS_TX_FIFO_SIZE               (512u) ///< The number of bytes to be allocated in DBusCAN's RAM for the DBus Tx FIFO buffer. Must be a multiple of 4 bytes.
#define DBCDRV_DBUS_BASE_RAM_ADDR                (0u) ///< Start (offset) address of DBusCAN's RAM space allocated for DBus buffers
#define DBCDRV_SPI_CRC_STATUS_OK                 (1u) ///< The value of the CRC status byte received from the DBusCAN chip in SPI transfer when CRC is OK
#define DBCDRV_MOPC_STANDBY_MODE_MASK            ((uint32_t)DBC_POWER_MODE_STANDBY << DBC_MOPC_MODE_SEL_POS) ///< Mask for setting the standby mode in the MOPC register

#ifndef DBUSCAN_SPI_CRC_USED
// these first two defines are used to avoid compilation errors when CRC is not enabled
#define DBCDRV_spiHdrSize          (DBC_SPI_HDR_SIZE) ///< SPI header size + size of CRC in SPI frame if CRC is not enabled
#define DBCDRV_setSpiHdrSize(size)                    ///< Macro to avoid compilation errors when CRC is not enabled
#endif // DBUSCAN_SPI_CRC_USED


/******************************************************************************/
/* PRIVATE TYPE DEFINITIONS                                                   */
/******************************************************************************/

/** \brief   Possible states of the write process to DBusCAN chip's EEPROM.
 */
enum DBCDRV_EepWriteState
{
    DBCDRV_EEP_WRITE_START,
    DBCDRV_EEP_WRITE_ONGOING,
    DBCDRV_EEP_WRITE_END
};


/******************************************************************************/
/* PRIVATE FUNCTION DECLARATIONS AND PRIVATE MACRO FUNCTION DEFINITIONS       */
/******************************************************************************/
static void DBCDRV_setSpiFrameHdr(enum DBC_RegAddr addr, uint16_t len, enum DBC_command cmd, uint8_t *writeBuf);
static enum DBC_Error DBCDRV_configure(DBC_Cfg_t cfg);
static enum DBC_Error DBCDRV_writeRegIpec(uint32_t bitVal, uint32_t bitPos, uint32_t bitMask);
static enum DBC_Error DBCDRV_writeEeprom(void);
static bool DBCDRV_isSupplyForEepromWrite(void);
static enum DBC_Error DBCDRV_setTableDbusBaudrate(uint16_t baudrate);
#ifdef DBUSCAN_ANY_DBUS_BAUDRATE_ENABLED
static enum DBC_Error DBCDRV_setAnyDbusBaudrate(uint16_t baudrate);
static uint32_t DBCDRV_getClockInputInHz(uint32_t clkinBitfield);
#endif //DBUSCAN_ANY_DBUS_BAUDRATE_ENABLED
static enum DBC_Error DBCDRV_sendSpiFrame(const uint8_t *writeBuf, uint16_t writeLen, uint8_t *readBuf, uint16_t readLen);
static enum DBC_Error DBCDRV_sendSpiFrameNbl(const uint8_t *writeBuf, uint16_t writeLen, uint8_t *readBuf, uint16_t readLen);
static enum DBC_Error DBCDRV_setPowerModeStandby(void);
static enum DBC_Error DBCDRV_doReset(bool *internalEepromError);
static enum DBC_Error DBCDRV_enableAndClearIrqFlags(void);
static enum DBC_Error DBCDRV_configureRestForDbus(DBC_Cfg_t cfg);
#ifdef DBUSCAN_SPI_CRC_USED
static uint32_t DBCDRV_calculateCrc(const uint8_t *data);
static enum DBC_Error DBCDRV_enableSpiCrc(void);
#else
static enum DBC_Error DBCDRV_disableSpiCrc(void);
#endif // DBUSCAN_SPI_CRC_USED
#ifdef DBUSCAN_DMA_USED
static enum DBC_Error DBCDRV_dmaInit(void);
static void DBCDRV_dmaCbFunction(void *obj, uint32_t flags, const struct MCAL_EventResponse *eventResponse);
#endif


/******************************************************************************/
/* PRIVATE DATA DEFINITIONS                                                   */
/******************************************************************************/
static bool DBCDRV_isPwrOnReset = false; // Indicates that power on reset of the device has occurred
#ifdef APP_VARIANT
static struct MEXTI_Handle DBCDRV_mextiHandle;
static MCAL_Callback_t DBCDRV_cbIrqHandle;
#endif
__ALIGNED(4) static uint8_t DBCDRV_readRegBuf[DBC_SPI_HDR_SIZE + DBCDRV_REG_SIZE + DBC_SPI_CRC_SIZE];   ///< Buffer for reading DBusCAN registers via non-blocking SPI communication
__ALIGNED(4) static uint8_t DBCDRV_readHdrBuf[DBC_SPI_HDR_SIZE];   ///< Buffer for the SPI read command header data
static union DBC_SpiBuf DBCDRV_spiBuff;              ///< Buffer for writing data to the DBusCAN chip over SPI
#ifdef DBUSCAN_SPI_CRC_USED
static uint8_t DBCDRV_spiHdrSize = DBC_SPI_HDR_SIZE; ///< Size of CRC in SPI frame
static bool DBCDRV_spiCrcReadError = false;          ///< Flag indicating that CRC error occurred during SPI read operation
static uint16_t DBCDRV_spiBuffCrcPtr;                ///< Pointer to the CRC field in the SPI buffer
#endif // DBUSCAN_SPI_CRC_USED
#ifdef DBUSCAN_DMA_USED
static MCAL_Callback_t DBCDRV_dmaCb;
static struct MDMA_Handle DBCDRV_dmaTxHandle;
static struct MDMA_Handle DBCDRV_dmaRxHandle;
static volatile bool DBCDRV_isDmaReady = true;
#endif // DBUSCAN_DMA_USED


/******************************************************************************/
/* PUBLIC DATA DEFINITIONS                                                    */
/******************************************************************************/
struct MSPI_Handle DBCDRV_mspiHandle;


/******************************************************************************/
#ifdef DBUSCAN_SPI_CRC_USED
/** \brief   Sets the number of bytes added to the SPI frame.
 *
 *  \note    This function sets the number of bytes added to the SPI frame. If CRC check is enabled,
 *           the SPI frame requires 4 additional bytes.
 *
 *  \param   size: The number of bytes to add to the SPI frame. 0 - no additional bytes, 4 - 4 additional bytes for CRC
 */
static inline void DBCDRV_setSpiHdrSize(uint8_t size)
{
    DBCDRV_spiHdrSize = size + DBC_SPI_HDR_SIZE;
}
#endif // DBUSCAN_SPI_CRC_USED


/******************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                */
/******************************************************************************/
enum DBC_Error DBCDRV_initComChannels(MCAL_CallbackFunction_t irqHandleCbFunc)
{
    /* Configuration of IO pin used for IRQ sensing from DBusCAN */
    const struct MDIO_Channel *intPin = MEXTI_getPin(DBCDRV_getMextiChannel());

    if(MCAL_OK != MDIO_init(intPin, &MDIO_INPUT_PULL_UP))
    {
        return DBC_ERROR;
    }

#ifdef APP_VARIANT
    const struct MEXTI_Config mextiCfg = {.trigger = MEXTI_TRIGGER_FALLING};
    if(MCAL_OK == MEXTI_init(&DBCDRV_mextiHandle, DBCDRV_getMextiChannel(), &mextiCfg))
    {
        MCAL_initCallback(&DBCDRV_cbIrqHandle, irqHandleCbFunc, (void*)&DBCDRV_mextiHandle);
    }
    else
    {
        return DBC_ERROR;
    }
#else
    (void)irqHandleCbFunc; //lint !e920 Cast from pointer to void - callback function is not used in bootloader variants
    DBCDRV_mspiHandle.status = MCAL_STATUS_RESET; // set reset status explicitly (necessary for bootloader variants due to the uninitialized RAM)
#endif //APP_VARIANT

#ifdef DBUSCAN_WITH_BBL_SPI
    DBC_RETURN_ON_ERROR(DBCBBL_init());
#else
    if(MCAL_OK != MSPI_init(&DBCDRV_mspiHandle, &DBCDRV_mspiChannel, &DBCDRV_mspiCfg))
    {
        return DBC_ERROR;
    }
  #ifdef DBUSCAN_DMA_USED
    DBC_RETURN_ON_ERROR(DBCDRV_dmaInit());
  #endif
#endif // !DBUSCAN_WITH_BBL_SPI
    return DBC_OK;
}

enum DBC_Error DBCDRV_init(MCAL_CallbackFunction_t irqHandleCbFunc)
{
#ifdef BOOTLOADER_VARIANT
    bool DBCDRV_isInternalEepromError = false;        // force to false, the EEPROM error is detected in the application
#else
    static bool DBCDRV_isInternalEepromError = false; // Indicates if an Internal EEPROM CRC error was detected during initialization; do not initialize the chip if true
#endif
    DBC_Cfg_t cfg = DBCDRV_getConfig();
    if(DBCDRV_isInternalEepromError)
    {
        return DBC_ERROR;
    }
    DBC_RETURN_ON_ERROR(DBCDRV_initComChannels(irqHandleCbFunc));
    DBC_RETURN_ON_ERROR(DBCDRV_setPowerModeStandby());
#ifdef DBUSCAN_SPI_CRC_USED
    DBC_RETURN_ON_ERROR(DBCDRV_enableSpiCrc());
#else
    DBC_RETURN_ON_ERROR(DBCDRV_disableSpiCrc());
#endif
    DBC_RETURN_ON_ERROR(DBCDRV_doReset(&DBCDRV_isInternalEepromError));
    DBC_RETURN_ON_ERROR(DBCDRV_configure(cfg));
    DBC_RETURN_ON_ERROR(DBCDRV_writeRegIpec(0uL, DBC_IPEC_NWKRQ_DELAY_POS, DBC_IPEC_NWKRQ_DELAY_MASK)); // clear NWKRQ_DELAY bit to disable nWKRQ pin delayed de-assertion in sleep mode
    DBC_RETURN_ON_ERROR(DBCDRV_enableCfgDbus());
    DBC_RETURN_ON_ERROR(DBCDRV_enableAndClearIrqFlags());
    DBC_RETURN_ON_ERROR(DBCDRV_configureRestForDbus(cfg));
    DBC_RETURN_ON_ERROR(DBCDRV_setPowerMode(DBC_POWER_MODE_NORMAL));

#if defined(VARIANT_PROGRAMMER) || defined(APP_VARIANT)
    DBCDRV_isPwrOnReset = false;
#endif
#ifdef APP_VARIANT
    MEXTI_enableEvent(&DBCDRV_mextiHandle, &DBCDRV_cbIrqHandle);
#endif

    return DBC_OK;
}

#ifdef APP_VARIANT
void DBCDRV_disableIrq(void)
{
    MEXTI_disableEvent(&DBCDRV_mextiHandle, &DBCDRV_cbIrqHandle);
}
#endif

uint8_t DBCDRV_HandleTask(void)
{
    if (true == DBCDRV_eepromWriteEnable)
    {
        if (DBC_ERROR != DBCDRV_writeEeprom())
        {
            DBCDRV_eepromWriteEnable = false;
        }
    }
    return TASK_INITIALISED;
}

bool DBCDRV_isPowerOnReset(void)
{
    return DBCDRV_isPwrOnReset;
}

void DBCDRV_notifyPowerOnReset(void)
{
    DBCDRV_isPwrOnReset = true;
}

enum DBC_Error DBCDRV_reset(enum DBC_Reset rstType)
{
    struct MSUP_TimeoutHandle resetTimeHandle;
    if (rstType == DBC_RST_FULL)
    {
        DBC_RETURN_ON_ERROR(DBCDRV_setPowerMode(DBC_POWER_MODE_STANDBY)); // Full reset is only achievable in STANDBY or NORMAL mode
#ifdef DBUSCAN_SPI_CRC_USED
        // Error is always returned here because chip does not send CRC OK as the last byte due to reset
        (void)DBCDRV_writeReg32(DBC_MOPC_ADDR, DBC_MOPC_DEVICE_RESET_MASK);
#else
        DBC_RETURN_ON_ERROR(DBCDRV_writeReg32(DBC_MOPC_ADDR, DBC_MOPC_DEVICE_RESET_MASK));
#endif
        DBCDRV_setSpiHdrSize(0u);
    }
    else if((rstType == DBC_RST_DBUS_SOFT) || (rstType == DBC_RST_DBUS_HARD))
    {
        DBC_RETURN_ON_ERROR(DBCDRV_writeReg32(DBC_DBUS_BCC_ADDR, (uint32_t)rstType));
    }
    else
    {
        // invalid reset mode
        return DBC_ERROR;
    }
    MSUP_setTimeOutExt(&resetTimeHandle, DBCDRV_RESET_TIME_US);
    while(!MSUP_isTimeOutExt(&resetTimeHandle)) {};
    return DBC_OK;
}

enum DBC_Error DBCDRV_spiReset(void)
{
    struct MSUP_TimeoutHandle resetTimeHandle;
    DBCDRV_setSpiHdrSize(0u);
    (void)MDIO_init(DBCDRV_mspiChannel.mosi, &MDIO_OUTPUT);
#ifdef DBUSCAN_WITH_BBL_SPI
    DBC_RETURN_ON_ERROR(DBCBBL_spiBusAcquire());
#else
    if (MCAL_STATUS_READY != MSPI_getStatus(&DBCDRV_mspiHandle))
    {
        return DBC_ERROR;
    }
    else
    {
        DBCDRV_mspiHandle.status = MCAL_STATUS_BUSY;
    }
    MDIO_write(DBCDRV_mspiChannel.cs, (bool)false);
#endif //!DBUSCAN_WITH_BBL_SPI

    // SPI reset sequence- toggle MOSI pin (at least) 3 times with 10us time delay while holding CS pin low
    MSUP_delay(10u);
    MDIO_toggle(DBCDRV_mspiChannel.mosi);
    MSUP_delay(10u);
    MDIO_toggle(DBCDRV_mspiChannel.mosi);
    MSUP_delay(10u);
    MDIO_toggle(DBCDRV_mspiChannel.mosi);
    MSUP_delay(10u);

#ifdef DBUSCAN_WITH_BBL_SPI
    DBC_RETURN_ON_ERROR(DBCBBL_spiBusRelease());
#else
    MDIO_write(DBCDRV_mspiChannel.cs, (bool)true);
    DBCDRV_mspiHandle.status = MCAL_STATUS_READY;
#endif

    MSUP_setTimeOutExt(&resetTimeHandle, DBCDRV_RESET_TIME_US);
    while(!MSUP_isTimeOutExt(&resetTimeHandle)) {};

    (void)MDIO_init(DBCDRV_mspiChannel.mosi, &MDIO_ALTERNATE_FUNCTION);
    return DBC_OK;
}

enum DBC_Error DBCDRV_setPowerMode(enum DBC_PowerMode mode)
{
    uint32_t regVal = DBCDRV_readReg32(DBC_MOPC_ADDR);
    uint32_t chipMode = (regVal & DBC_MOPC_MODE_SEL_MASK) >> DBC_MOPC_MODE_SEL_POS;
    const uint32_t INVALID_REG_VAL = ~0uL;
    if(regVal == INVALID_REG_VAL)
    {
        return DBC_ERROR;
    }
    if (chipMode != (uint32_t)mode)
    {
        if ((DBC_POWER_MODE_NORMAL == mode) && (chipMode != (uint32_t)DBC_POWER_MODE_STANDBY))
        {   // changing to NORMAL mode is only possible from STANDBY mode
            regVal &= ~DBC_MOPC_MODE_SEL_MASK;
            regVal |= DBCDRV_MOPC_STANDBY_MODE_MASK;
            (void)DBCDRV_writeReg32(DBC_MOPC_ADDR, regVal);
        }
        regVal &= ~DBC_MOPC_MODE_SEL_MASK;
        regVal |= (uint32_t)mode << DBC_MOPC_MODE_SEL_POS;
        (void)DBCDRV_writeReg32(DBC_MOPC_ADDR, regVal);
        regVal &= DBC_MOPC_MODE_SEL_MASK;
        return ((DBCDRV_readReg32(DBC_MOPC_ADDR) & DBC_MOPC_MODE_SEL_MASK) != regVal) ? DBC_ERROR : DBC_OK;
    }
    return DBC_OK;
}

uint8_t DBCDRV_getStatus(void)
{
    return DBCDRV_spiBuff.array[0];
}

enum DBC_Error DBCDRV_write(enum DBC_RegAddr addr, DBC_WriteBuf_t *writeBuf, uint16_t len, enum DBC_command cmd)
{
    DBCDRV_setSpiFrameHdr(addr, len, cmd, writeBuf->spiHdr);
#ifdef DBUSCAN_SPI_CRC_USED
    const uint16_t comLen = len + DBCDRV_spiHdrSize;
    *((uint32_t *)(void *)&writeBuf->bytes[len]) = DBCDRV_calculateCrc((uint8_t *)(void *)writeBuf);
    return DBCDRV_sendSpiFrame((uint8_t*)(void*)writeBuf, comLen, (uint8_t*)(void*)writeBuf, comLen);
#else
    return DBCDRV_sendSpiFrame((uint8_t*)(void*)writeBuf, (len + DBCDRV_spiHdrSize), DBCDRV_spiBuff.array, DBC_SPI_HDR_SIZE);
#endif // DBUSCAN_SPI_CRC_USED
}

enum DBC_Error DBCDRV_writeNbl(enum DBC_RegAddr addr, DBC_WriteBuf_t *writeBuf, uint16_t len, enum DBC_command cmd)
{
    DBCDRV_setSpiFrameHdr(addr, len, cmd, writeBuf->spiHdr);
#ifdef DBUSCAN_SPI_CRC_USED
    *((uint32_t *)(void *)&writeBuf->bytes[len]) = DBCDRV_calculateCrc((uint8_t *)(void *)writeBuf);
    const uint16_t comLen = len + DBCDRV_spiHdrSize;
    return DBCDRV_sendSpiFrameNbl((uint8_t*)(void*)writeBuf, comLen, DBCDRV_spiBuff.array, comLen);
#else
    return DBCDRV_sendSpiFrameNbl((uint8_t*)(void*)writeBuf, (len + DBCDRV_spiHdrSize), DBCDRV_spiBuff.array, DBC_SPI_HDR_SIZE);
#endif // DBUSCAN_SPI_CRC_USED
}

enum DBC_Error DBCDRV_read(enum DBC_RegAddr addr, uint8_t *readBuf, uint16_t len, enum DBC_command cmd)
{
    enum DBC_Error retVal;
    const uint16_t comLen = (len + DBCDRV_spiHdrSize);
    DBCDRV_setSpiFrameHdr(addr, len, cmd, DBCDRV_readHdrBuf);
    retVal = DBCDRV_sendSpiFrame(DBCDRV_readHdrBuf, comLen, readBuf, comLen);
#ifdef DBUSCAN_SPI_CRC_USED
    if(DBC_OK == retVal)
    {
        DBCDRV_checkSpiCrcInReadOp(readBuf);
        return DBCDRV_isSpiCrcReadError() ? DBC_ERROR : DBC_OK;
    }
#endif // DBUSCAN_SPI_CRC_USED
    return retVal;
}

enum DBC_Error DBCDRV_readNbl(enum DBC_RegAddr addr, uint8_t *readBuf, uint16_t len, enum DBC_command cmd)
{
    const uint16_t comLen = len + DBCDRV_spiHdrSize;
    DBCDRV_setSpiFrameHdr(addr, len, cmd, DBCDRV_readHdrBuf);
    return DBCDRV_sendSpiFrameNbl(DBCDRV_readHdrBuf, comLen, readBuf, comLen);
}

enum DBC_Error DBCDRV_writeReg32(enum DBC_RegAddr addr, uint32_t data)
{
    DBCDRV_setSpiFrameHdr(addr, DBCDRV_REG_SIZE, DBC_WRITE_L, DBCDRV_spiBuff.array);
    DBCDRV_spiBuff.d0.data0 = data;
#ifdef DBUSCAN_SPI_CRC_USED
    DBCDRV_spiBuff.d0.crc = DBCDRV_calculateCrc(DBCDRV_spiBuff.array);
    DBC_RETURN_ON_ERROR(DBCDRV_sendSpiFrame(DBCDRV_spiBuff.array, ((uint16_t)DBCDRV_spiHdrSize + DBCDRV_REG_SIZE), NULL, 0u));
    return (DBCDRV_isSpiCrcWriteError()) ? DBC_ERROR : DBC_OK;
#else
    return DBCDRV_sendSpiFrame(DBCDRV_spiBuff.array, ((uint16_t)DBCDRV_spiHdrSize + DBCDRV_REG_SIZE), NULL, 0u);
#endif // DBUSCAN_SPI_CRC_USED
}

enum DBC_Error DBCDRV_writeReg32Nbl(enum DBC_RegAddr addr, uint32_t data)
{
    DBCDRV_setSpiFrameHdr(addr, DBCDRV_REG_SIZE, DBC_WRITE_L, DBCDRV_spiBuff.array);
    DBCDRV_spiBuff.d0.data0 = data;
#ifdef DBUSCAN_SPI_CRC_USED
    DBCDRV_spiBuff.d0.crc = DBCDRV_calculateCrc(DBCDRV_spiBuff.array);
#endif
    return DBCDRV_sendSpiFrameNbl(DBCDRV_spiBuff.array, ((uint16_t)DBCDRV_spiHdrSize + DBCDRV_REG_SIZE), NULL, 0u);
}

uint32_t DBCDRV_readReg32(enum DBC_RegAddr addr)
{
    (void)DBCDRV_read(addr, DBCDRV_spiBuff.array, DBCDRV_REG_SIZE, DBC_READ_L);
    return DBCDRV_spiBuff.d0.data0;
}

enum DBC_Error DBCDRV_readReg32Nbl(enum DBC_RegAddr addr)
{
    DBCDRV_setSpiFrameHdr(addr, DBCDRV_REG_SIZE, DBC_READ_L, DBCDRV_readHdrBuf);
    return DBCDRV_sendSpiFrameNbl(DBCDRV_readHdrBuf, DBC_SPI_HDR_SIZE, DBCDRV_readRegBuf, ((uint16_t)DBCDRV_spiHdrSize + DBCDRV_REG_SIZE));
}

uint32_t DBCDRV_getReadReg32Nbl(void)
{
#ifdef DBUSCAN_SPI_CRC_USED
    DBCDRV_checkSpiCrcInReadOp(DBCDRV_readRegBuf);
#endif
    return *(uint32_t*)(void*)&DBCDRV_readRegBuf[DBC_SPI_HDR_SIZE];
}

bool DBCDRV_isIrqEvent(void)
{
    const struct MDIO_Channel *intPin = MEXTI_getPin(DBCDRV_getMextiChannel());
    return !MDIO_read(intPin);
}

enum DBC_Error DBCDRV_readIrq(union DBC_IrqBuf *irqBuf)
{
    uint16_t irqBufSize = (uint16_t)sizeof(*irqBuf);
    DBCDRV_setSpiFrameHdr(DBC_IF_ADDR, (uint16_t)(irqBufSize - DBCDRV_spiHdrSize), DBC_READ_L, DBCDRV_readHdrBuf);
#if defined(VARIANT_PROGRAMMER) || defined(APP_VARIANT)
    return DBCDRV_sendSpiFrameNbl(DBCDRV_readHdrBuf, DBC_SPI_HDR_SIZE, irqBuf->irqArray, irqBufSize);
#else
    // Bootloader variants do not use interrupts
    return DBCDRV_sendSpiFrame(DBCDRV_readHdrBuf, DBC_SPI_HDR_SIZE, irqBuf->irqArray, irqBufSize);
#endif // VARIANT_PROGRAMMER || APP_VARIANT
}

enum DBC_Error DBCDRV_enableCfgDbus(void)
{
    uint32_t regVal = DBCDRV_readReg32(DBC_DBUS_CCCR_ADDR);

    if ((regVal & DBC_DBUS_CCCR_INIT_MASK) != DBC_DBUS_CCCR_INIT_MASK)
    {
        regVal |= DBC_DBUS_CCCR_INIT_MASK;
        DBC_RETURN_ON_ERROR(DBCDRV_writeReg32(DBC_DBUS_CCCR_ADDR, regVal));
    }
    regVal |= DBC_DBUS_CCCR_CCE_MASK;
    DBC_RETURN_ON_ERROR(DBCDRV_writeReg32(DBC_DBUS_CCCR_ADDR, regVal));

    return DBC_OK;
}

enum DBC_Error DBCDRV_disableCfgDbus(void)
{
    uint32_t regVal = DBCDRV_readReg32(DBC_DBUS_CCCR_ADDR);
    regVal &= ~(DBC_DBUS_CCCR_CCE_MASK | DBC_DBUS_CCCR_INIT_MASK);
    return DBCDRV_writeReg32(DBC_DBUS_CCCR_ADDR, regVal);
}

uint16_t DBCDRV_convertDbusBaudValue(uint16_t baudrate)
{
    switch (baudrate)
    {
        case 96u:    baudrate = (uint16_t)DBC_DBUS_BAUD_9600;  break;
        case 192u:   baudrate = (uint16_t)DBC_DBUS_BAUD_19200; break;
        case 384u:   baudrate = (uint16_t)DBC_DBUS_BAUD_38400; break;
        case 576u:   baudrate = (uint16_t)DBC_DBUS_BAUD_57600; break;
        case 1250u:  baudrate = (uint16_t)DBC_DBUS_BAUD_125K;  break;
        case 2500u:  baudrate = (uint16_t)DBC_DBUS_BAUD_250K;  break;
        case 5000u:  baudrate = (uint16_t)DBC_DBUS_BAUD_500K;  break;
        case 10000u: baudrate = (uint16_t)DBC_DBUS_BAUD_1M;    break;
        default:     baudrate = (uint16_t)DBC_DBUS_BAUD_9600;  break;
    }
    return baudrate;
}

enum DBC_Error DBCDRV_setDbusBaudrate(uint16_t baudrate)
{
#ifdef DBUSCAN_ANY_DBUS_BAUDRATE_ENABLED
    if (DBCDRV_isAllFeatureRevision())
    {
        return DBCDRV_setAnyDbusBaudrate(baudrate);
    }
    else
    {
        return DBCDRV_setTableDbusBaudrate(baudrate);
    }
#else
    return DBCDRV_setTableDbusBaudrate(baudrate);
#endif //DBUSCAN_ANY_DBUS_BAUDRATE_ENABLED
}

extern bool DBCDRV_isAllFeatureRevision(void)
{
    return (DBCDRV_readReg32(DBC_REVISION_ADDR) >= DBC_REVISION_WITH_ALL_FEATURES) ? true : false;
}

#ifdef DBUSCAN_SPI_CRC_USED
bool DBCDRV_isSpiCrcReadError(void)
{
    const bool retVal = DBCDRV_spiCrcReadError;
    DBCDRV_spiCrcReadError = false;
    return retVal;
}

void DBCDRV_checkSpiCrcInReadOp(uint8_t *readData)
{
    uint8_t statusByte = readData[0u];
    *(uint32_t *)(void *)readData = *(uint32_t *)(void *)DBCDRV_readHdrBuf; // copy SPI header into read data for correct CRC calculation
    const uint32_t CRC_POS = DBC_SPI_HDR_SIZE + ((uint32_t)readData[DBC_SPI_HDR_BYTE_DATA_LEN] * WORD_SIZE);
    const uint32_t CRC_CAL = DBCDRV_calculateCrc(readData);
    readData[0u] = statusByte;
    DBCDRV_spiCrcReadError = (CRC_CAL != *(uint32_t *)(void *)&readData[CRC_POS]) ? true : false;
}

bool DBCDRV_isSpiCrcWriteError(void)
{
    return DBCDRV_spiBuff.array[DBCDRV_spiBuffCrcPtr] != DBCDRV_SPI_CRC_STATUS_OK;
}
#endif // DBUSCAN_SPI_CRC_USED

/******************************************************************************/
/* PRIVATE FUNCTION DEFINITIONS                                               */
/******************************************************************************/

static void DBCDRV_setSpiFrameHdr(enum DBC_RegAddr addr, uint16_t len, enum DBC_command cmd, uint8_t *writeBuf)
{
#ifdef DBUSCAN_SPI_CRC_USED
    DBCDRV_spiBuffCrcPtr = (len | 0x03u) + DBC_SPI_HDR_SIZE;
#endif
    writeBuf[DBC_SPI_HDR_BYTE_CMD]       = (uint8_t)cmd;
    writeBuf[DBC_SPI_HDR_BYTE_ADDR_HIGH] = (uint8_t)((uint16_t)addr >> BYTE_SIZE);
    writeBuf[DBC_SPI_HDR_BYTE_ADDR_LOW]  = (uint8_t)addr;
    writeBuf[DBC_SPI_HDR_BYTE_DATA_LEN]  = (uint8_t)WORD_SIZEOF(len); // data length must be given in number of words (1 word = 4 bytes)
}

/** \brief   Configures the DBusCAN chip according to the configuration structure settings.
 *
 *  \param   cfg: configuration structure
 *
 *  \note    DBusCAN chip must be in STANDBY power mode when using this function.
 *
 *  \return  enum DBC_Error
 */
static enum DBC_Error DBCDRV_configure(DBC_Cfg_t cfg)
{
    uint32_t bitMask = 0u;
    uint32_t regVal;
    DBC_Cfg_t currCfg;

    currCfg.word = DBCDRV_readReg32(DBC_EEPP_ADDR); // get current configuration of the chip

    if (currCfg.word != cfg.word)
    {
        // configuration of general purpose memory bits
        if (currCfg.GP_MEM != cfg.GP_MEM)
        {
            regVal = currCfg.word;
            UTI_RES(regVal, DBC_EEP_GP_MEM_MASK);
            UTI_SET(regVal, (cfg.word & DBC_EEP_GP_MEM_MASK));
            DBC_RETURN_ON_ERROR(DBCDRV_writeReg32(DBC_EEPP_ADDR, regVal));
        }

        // configuration of DBus register bits
        if ((currCfg.word & DBC_EEP_BITS_REG_DBUS_MASK) != (cfg.word & DBC_EEP_BITS_REG_DBUS_MASK))
        {
            DBC_RETURN_ON_ERROR(DBCDRV_writeRegIpec(1uL, DBC_IPEC_DBUS_EN_POS, DBC_IPEC_DBUS_EN_MASK)); // Enable DBus part
            DBC_RETURN_ON_ERROR(DBCDRV_enableCfgDbus()); // unlock DBus regs for write access
            // configuration of DBUS_DPC register bits
            if ((currCfg.word & DBC_EEP_BITS_REG_DBUS_DPC_MASK) != (cfg.word & DBC_EEP_BITS_REG_DBUS_DPC_MASK))
            {
                regVal = DBCDRV_readReg32(DBC_DBUS_DPC_ADDR);
                UTI_RES(regVal, DBC_DBUS_DPC_ADV_PWR_MGMT_MASK);
                UTI_SET(regVal, ((uint32_t)cfg.ADV_PWR_MGMT << DBC_DBUS_DPC_ADV_PWR_MGMT_POS));
                UTI_RES(regVal, DBC_DBUS_DPC_BVD_WAIT_EN_MASK);
                UTI_SET(regVal, ((uint32_t)cfg.BVD_WAIT_EN << DBC_DBUS_DPC_BVD_WAIT_EN_POS));
                UTI_RES(regVal, DBC_DBUS_DPC_BVD_THLD_MASK);
                UTI_SET(regVal, ((uint32_t)cfg.BVD_THLD << DBC_DBUS_DPC_BVD_THLD_POS));
                UTI_RES(regVal, DBC_DBUS_DPC_BVD_TO_NWKRQ_MASK);
                UTI_SET(regVal, ((uint32_t)cfg.BVD_TO_NWKRQ << DBC_DBUS_DPC_BVD_TO_NWKRQ_POS));
                DBC_RETURN_ON_ERROR(DBCDRV_writeReg32(DBC_DBUS_DPC_ADDR, regVal));
            }
            // configuration of DBUS_SIDFC register bits
            if ((currCfg.word & DBC_EEP_BITS_REG_DBUS_SIDFC_MASK) != (cfg.word & DBC_EEP_BITS_REG_DBUS_SIDFC_MASK))
            {
                regVal = DBCDRV_readReg32(DBC_DBUS_SIDFC_ADDR);
                UTI_RES(regVal, DBC_DBUS_SIDFC_SID_MASK);
                UTI_SET(regVal, ((uint32_t)cfg.SUBNODE_ID));
                UTI_RES(regVal, DBC_DBUS_SIDFC_PID_MASK);
                UTI_SET(regVal, ((uint32_t)cfg.NODE_ID << DBC_DBUS_SIDFC_PID_POS));
                DBC_RETURN_ON_ERROR(DBCDRV_writeReg32(DBC_DBUS_SIDFC_ADDR, regVal));
            }
            // configuration of DBUS_DBR register bits
            if ((currCfg.word & DBC_EEP_BITS_REG_DBUS_DBR_MASK) != (cfg.word & DBC_EEP_BITS_REG_DBUS_DBR_MASK))
            {
                regVal = DBCDRV_readReg32(DBC_DBUS_DBR_ADDR);
                UTI_RES(regVal, DBC_DBUS_DBR_DBR_MASK);
                UTI_SET(regVal, ((uint32_t)cfg.DBR));
                UTI_RES(regVal, DBC_DBUS_DBR_CLKIN_MASK);
                UTI_SET(regVal, ((uint32_t)cfg.CLKIN << DBC_DBUS_DBR_CLKIN_POS));
                DBC_RETURN_ON_ERROR(DBCDRV_writeReg32(DBC_DBUS_DBR_ADDR, regVal));
            }
            DBC_RETURN_ON_ERROR(DBCDRV_disableCfgDbus()); // lock DBus regs for write access
        }

        // configuration of CAN register bits
        if ((currCfg.word & DBC_EEP_BITS_REG_CAN_MASK) != (cfg.word & DBC_EEP_BITS_REG_CAN_MASK))
        {
            //(void)DBCDRV_writeRegIpec(1uL, DBC_IPEC_MCAN_EN_POS, DBC_IPEC_MCAN_EN_MASK); // Enable CAN part
            // todo
        }

        // configuration of IPEC register bits
        if ((currCfg.word & DBC_EEP_BITS_REG_IPEC_MASK) != (cfg.word & DBC_EEP_BITS_REG_IPEC_MASK))
        {
            regVal = 0u;
            bitMask |= DBC_IPEC_MCAN_EN_MASK;
            regVal  |= (uint32_t)cfg.MCAN_EN << DBC_IPEC_MCAN_EN_POS;
            bitMask |= DBC_IPEC_DBUS_EN_MASK;
            regVal  |= (uint32_t)cfg.DBUS_EN << DBC_IPEC_DBUS_EN_POS;
            bitMask |= DBC_IPEC_DBUS2CAN_MASK;
            regVal  |= (uint32_t)cfg.DBUS2CAN << DBC_IPEC_DBUS2CAN_POS;
            bitMask |= DBC_IPEC_CAN_BIAS_MASK;
            regVal  |= (uint32_t)cfg.CAN_BIAS << DBC_IPEC_CAN_BIAS_POS;
            DBC_RETURN_ON_ERROR(DBCDRV_writeRegIpec(regVal, 0uL, bitMask));
        }

        if (DBCDRV_readReg32(DBC_EEPP_ADDR) != cfg.word)
        {
            return DBC_ERROR;
        }
    }
    return DBC_OK;
}


/** \brief   Writes the protected IPEC register (0x0814) of the DBusCAN chip.
 *
 *  \note    To successfully write the IPEC register the DBusCAN chip must be in STANDBY power mode.
 *
 *  \param   bitVal:  value to be set
 *  \param   bitPos:  position of the bit/bitfield to be set
 *  \param   bitMask: mask of the bit/bitfield to be set
 *
 *  \return  enum DBC_Error
 */
static enum DBC_Error DBCDRV_writeRegIpec(uint32_t bitVal, uint32_t bitPos, uint32_t bitMask)
{
    uint32_t regVal = DBCDRV_readReg32(DBC_IPEC_ADDR);
    if (((regVal & bitMask) >> bitPos) == bitVal)
    {
        return DBC_OK;
    }

    // enable write access to IPEC reg
    regVal |= DBC_IPEC_CCE_MASK;
    DBC_RETURN_ON_ERROR(DBCDRV_writeReg32(DBC_IPEC_ADDR, regVal));

    // configure desired bit/bitfield value
    regVal &= ~bitMask;
    regVal |= (bitVal << bitPos);

    // disable write access to IPEC reg
    regVal &= ~DBC_IPEC_CCE_MASK;

    // apply new IPEC reg settings
    DBC_RETURN_ON_ERROR(DBCDRV_writeReg32(DBC_IPEC_ADDR, regVal));

    regVal &= bitMask;
    regVal &= ~DBC_IPEC_EP_CC_MASK; // do not include EP_CC bitfield as it always reads zeros
    return ((DBCDRV_readReg32(DBC_IPEC_ADDR) & bitMask) != regVal) ? DBC_ERROR : DBC_OK;
}


/** \brief   Performs non-blocking write to the EEPROM memory of the DBusCAN chip.
 *
 *  \return  enum DBC_Error
 */
static enum DBC_Error DBCDRV_writeEeprom(void)
{
    uint32_t regVal;
    uint32_t eepPending;
    const uint32_t eepWriteKey = DBC_IPEC_EP_WR_MASK | ((uint32_t)DBC_IPEC_EP_CC_VAL << DBC_IPEC_EP_CC_POS);
    static enum DBCDRV_EepWriteState writeState = DBCDRV_EEP_WRITE_START;

    switch (writeState)
    {
        case DBCDRV_EEP_WRITE_START:
        {
            if (DBCDRV_isPwrOnReset)
            {   // need to wait until device is (re)initialized
                return DBC_ERROR;
            }
            eepPending = DBCDRV_readReg32(DBC_EEPP_ADDR);
            if (DBCDRV_readReg32(DBC_EEPC_ADDR) == eepPending)
            {   // nothing to do, all EEPROM bits are set as expected
                return DBC_OK;
            }
            if (true != DBCDRV_isSupplyForEepromWrite())
            {   // insufficient supply- not possible to flash EEPROM
                DBCDRV_eepromWriteEnable = false;
                return DBC_ERROR;
            }
            regVal = DBCDRV_readReg32(DBC_IPEC_ADDR);
            if (DBC_IPEC_EP_WR_MASK == (regVal & DBC_IPEC_EP_WR_MASK)) // check if there isn't ongoing write process to EEPROM
            {
                return DBC_ERROR;
            }
            // start EEPROM write process
            DBC_RETURN_ON_ERROR(DBCDRV_writeRegIpec(eepWriteKey, 0uL, DBC_IPEC_EP_CC_MASK));
            writeState = DBCDRV_EEP_WRITE_ONGOING;
            break;
        }
        case DBCDRV_EEP_WRITE_ONGOING:
        {
            regVal  = DBCDRV_readReg32(DBC_IPEC_ADDR);
            regVal &= DBC_IPEC_EP_WR_MASK;
            writeState = (DBC_IPEC_EP_WR_MASK == regVal) ? DBCDRV_EEP_WRITE_ONGOING : DBCDRV_EEP_WRITE_END; // wait until EEPROM write process is finished
            break;
        }
        case DBCDRV_EEP_WRITE_END:
        {
            eepPending = DBCDRV_readReg32(DBC_EEPP_ADDR);
            if (DBCDRV_readReg32(DBC_EEPC_ADDR) != eepPending)
            {   // try to write EEPROM again
                writeState = DBCDRV_EEP_WRITE_START;
            }
            else
            {   // write to EEPROM has been successful
                return DBC_OK;
            }
            break;
        }
        default:
        {
            break;
        }
    }
    return DBC_ERROR;
}


/** \brief   Checks if there is enough voltage level (on VCC pin) to flash EEPROM of the DBusCAN chip.
 *
 *  \return  bool
 */
static bool DBCDRV_isSupplyForEepromWrite(void)
{
    bool isVcc;
    uint32_t regVal = DBCDRV_readReg32(DBC_IPEC_ADDR);
    bool isCanEnabled = (DBC_IPEC_MCAN_EN_MASK == (regVal & DBC_IPEC_MCAN_EN_MASK)) ? true : false;
    bool isCanBiasSet = (DBC_IPEC_CAN_BIAS_MASK == (regVal & DBC_IPEC_CAN_BIAS_MASK)) ? true : false;

    if (!isCanEnabled || !isCanBiasSet)
    {
        (void)DBCDRV_setPowerMode(DBC_POWER_MODE_STANDBY);
        if (!isCanEnabled)
        {   // enable CAN to see if UVCC flag is set
            (void)DBCDRV_writeRegIpec(1uL, DBC_IPEC_MCAN_EN_POS, DBC_IPEC_MCAN_EN_MASK);
        }
        if (!isCanBiasSet)
        {   // set CAN_BIAS (which lowers VCC undervoltage level) to see if UVCC flag is still set
            (void)DBCDRV_writeRegIpec(1uL, DBC_IPEC_CAN_BIAS_POS, DBC_IPEC_CAN_BIAS_MASK);
        }
        (void)DBCDRV_setPowerMode(DBC_POWER_MODE_NORMAL);
    }

    (void)DBCDRV_writeReg32(DBC_IF_ADDR, DBC_IF_UVCC_MASK);
    regVal = DBCDRV_readReg32(DBC_IF_ADDR) & DBC_IF_UVCC_MASK;
    isVcc = (DBC_IF_UVCC_MASK == regVal) ? false : true;

    // set back CAN_BIAS and MCAN_EN bits
    if (!isCanEnabled || !isCanBiasSet)
    {
        (void)DBCDRV_setPowerMode(DBC_POWER_MODE_STANDBY);
        if (!isCanEnabled)
        {   // disable CAN
            (void)DBCDRV_writeRegIpec(0uL, DBC_IPEC_MCAN_EN_POS, DBC_IPEC_MCAN_EN_MASK);
        }
        if (!isCanBiasSet)
        {   // reset CAN_BIAS bit
            (void)DBCDRV_writeRegIpec(0uL, DBC_IPEC_CAN_BIAS_POS, DBC_IPEC_CAN_BIAS_MASK);
        }
        (void)DBCDRV_setPowerMode(DBC_POWER_MODE_NORMAL);
    }

    return isVcc;
}

/** \brief   Sets DBus baudrate value from the enum DBC_DbusBaudrate in the DBusCAN chip.
 *
 *  \param   baudrate: DBus baudrate value in hectobauds
 *
 *  \return  enum DBC_Error
 */
static enum DBC_Error DBCDRV_setTableDbusBaudrate(uint16_t baudrate)
{
    uint32_t regVal;

    baudrate = DBCDRV_convertDbusBaudValue(baudrate);

    if (baudrate <= (uint16_t)DBC_DBUS_BAUD_1M)
    {
        regVal = DBCDRV_readReg32(DBC_DBUS_DBR_ADDR);
        if ((regVal & DBC_DBUS_DBR_DBR_MASK) != (uint32_t)baudrate)
        {
            DBC_RETURN_ON_ERROR(DBCDRV_enableCfgDbus());
            // change baudrate value
            regVal &= ~DBC_DBUS_DBR_DBR_MASK;
            regVal |= (uint32_t)baudrate;
            DBC_RETURN_ON_ERROR(DBCDRV_writeReg32(DBC_DBUS_DBR_ADDR, regVal));
            DBC_RETURN_ON_ERROR(DBCDRV_disableCfgDbus());
        }
        return DBC_OK;
    }
    else
    {
        return DBC_ERROR;
    }
}


#ifdef DBUSCAN_ANY_DBUS_BAUDRATE_ENABLED
/** \brief   Sets any (possible) DBus baudrate value in the DBusCAN chip.
 *
 *  \param   baudrate: DBus baudrate value in hectobauds
 *
 *  \return  enum DBC_Error
 */
static enum DBC_Error DBCDRV_setAnyDbusBaudrate(uint16_t baudrate)
{
    const uint8_t OVERSAMPLING = 16u;
    const uint32_t INT_DIV_MAX = (DBC_DBUS_DBR_CB_INT_MASK >> DBC_DBUS_DBR_CB_INT_POS);
    const uint32_t FRAC_DIV_RESOLUTION = (DBC_DBUS_DBR_CB_FRAC_MASK >> DBC_DBUS_DBR_CB_FRAC_POS) + 1u;
    uint32_t regVal = DBCDRV_readReg32(DBC_DBUS_DBR_ADDR);
    uint32_t clkin  = DBCDRV_getClockInputInHz((regVal & DBC_DBUS_DBR_CLKIN_MASK) >> DBC_DBUS_DBR_CLKIN_POS);
    struct MTDIV_Result divResult = MTDIV_div_mod(clkin, (INT_DIV_MAX * OVERSAMPLING));
    uint32_t minBaudrate = (0u != divResult.remainder) ? (divResult.quotient + 1u) : divResult.quotient;
    uint32_t maxBaudrate = MTDIV_div_32_32(clkin, (uint32_t)OVERSAMPLING);
    uint32_t neededBaudrate = 100uL * baudrate; // convert baudrate from hectobauds to Hz

    if ((neededBaudrate >= minBaudrate) && (neededBaudrate <= maxBaudrate))
    {
        divResult = MTDIV_div_mod(clkin, (neededBaudrate * OVERSAMPLING));
        uint32_t intDiv = divResult.quotient;
        uint32_t fracDiv = 0u;
        if (0u != divResult.remainder)
        {
            // formula: fracDiv = (FRAC_DIV_RESOLUTION * clkin / OVERSAMPLING / neededBaudrate) - (intDiv * FRAC_DIV_RESOLUTION)
            fracDiv = MTDIV_div_32_32(clkin, neededBaudrate) * FRAC_DIV_RESOLUTION; // partial division before multiplication to avoid overflowing uint32_t
            fracDiv = MTDIV_div_32_32(fracDiv, (uint32_t)OVERSAMPLING) - (intDiv * FRAC_DIV_RESOLUTION);
        }
        DBC_RETURN_ON_ERROR(DBCDRV_enableCfgDbus());
        // set custom baudrate value
        regVal &= ~(DBC_DBUS_DBR_CB_INT_MASK | DBC_DBUS_DBR_CB_FRAC_MASK);
        regVal |= intDiv << DBC_DBUS_DBR_CB_INT_POS;
        regVal |= fracDiv << DBC_DBUS_DBR_CB_FRAC_POS;
        regVal |= DBC_DBUS_DBR_CB_EN_MASK;
        DBC_RETURN_ON_ERROR(DBCDRV_writeReg32(DBC_DBUS_DBR_ADDR, regVal));
        DBC_RETURN_ON_ERROR(DBCDRV_disableCfgDbus());
        return DBC_OK;
    }
    else
    {
        return DBC_ERROR;
    }
}


static uint32_t DBCDRV_getClockInputInHz(uint32_t clkinBitfield)
{
    uint32_t clkinHz = 0u;
    switch (clkinBitfield)
    {
        case (uint32_t)DBC_CLKIN_20M: clkinHz = 20000000u;  break;
        case (uint32_t)DBC_CLKIN_40M: clkinHz = 40000000u;  break;
        case (uint32_t)DBC_CLKIN_80M: clkinHz = 80000000u;  break;
        default:                                            break;
    }
    return clkinHz;
}
#endif //DBUSCAN_ANY_DBUS_BAUDRATE_ENABLED


/** \brief   Sends SPI frame to the DBusCAN chip.
 *
 *  \param   writeBuf:   pointer to buffer with data to be written
 *  \param   writeLen:   number of bytes to be written
 *  \param   readBuf:    pointer to buffer for storing read data
 *  \param   readLen:    number of bytes to be read
 *
 *  \return  enum DBC_Error
 */
static enum DBC_Error DBCDRV_sendSpiFrame(const uint8_t *writeBuf, uint16_t writeLen, uint8_t *readBuf, uint16_t readLen)
{
    if((readBuf == NULL) && (writeLen <= sizeof(union DBC_SpiBuf)))
    {
        readLen = writeLen;
        readBuf = DBCDRV_spiBuff.array;
    }
#if defined(VARIANT_PROGRAMMER) || defined(APP_VARIANT)
  #ifdef DBUSCAN_WITH_BBL_SPI
    return DBCBBL_transferBlocking(writeBuf, writeLen, readBuf, readLen);
  #else
    struct MSUP_TimeoutHandle transferTimeout;
    #ifdef DBUSCAN_DMA_USED
    if(DBCDRV_isDmaReady && (MCAL_OK == MSPI_transferDma(&DBCDRV_mspiHandle, writeBuf, writeLen, readBuf, readLen)))
    {
        DBCDRV_isDmaReady = false;
        MSUP_setTimeOutExt(&transferTimeout, MSPI_TIMEOUT_VALUE);
        while(!DBCDRV_isDmaReady)
        {
            if(MSUP_isTimeOutExt(&transferTimeout))
            {
                (void)MDMA_disableChannelEvent(DBCDRV_mspiHandle.mdmaRxHandle, &DBCDRV_mspiHandle.mdmaRxCallback);
                (void)MDMA_disableChannelEvent(DBCDRV_mspiHandle.mdmaTxHandle, &DBCDRV_mspiHandle.mdmaTxCallback);
                MDMA_disableChannel(DBCDRV_mspiHandle.mdmaRxHandle);
                MDMA_disableChannel(DBCDRV_mspiHandle.mdmaTxHandle);
                MSPI_disableDMA(DBCDRV_mspiHandle.channel->mspi);
                MSPI_endCommunication(&DBCDRV_mspiHandle);
                DBCDRV_isDmaReady = true;
                return DBC_ERROR;
            }
        }
        return DBC_OK;
    }
    #else // !DBUSCAN_DMA_USED
    if(MCAL_OK == MSPI_transfer(&DBCDRV_mspiHandle, writeBuf, writeLen, readBuf, readLen))
    {
        bool isTimeout;
        MSUP_setTimeOutExt(&transferTimeout, MSPI_TIMEOUT_VALUE);
        do
        {
            isTimeout = MSUP_isTimeOutExt(&transferTimeout);
        } while((DBCDRV_mspiHandle.status != MCAL_STATUS_READY) && !isTimeout);
        if(!isTimeout)
        {
            return DBC_OK;
        }
        MSPI_endCommunication(&DBCDRV_mspiHandle);
    }
    #endif // !DBUSCAN_DMA_USED
    return DBC_ERROR;
  #endif //!DBUSCAN_WITH_BBL_SPI
#else
    // bootloader variants do not use interrupts
    return (MCAL_OK != MSPI_transferBlocking(&DBCDRV_mspiHandle, writeBuf, writeLen, readBuf, readLen)) ? DBC_ERROR : DBC_OK;
#endif // VARIANT_PROGRAMMER || APP_VARIANT
}


/** \brief   Sends SPI frame to the DBusCAN chip with non-blocking SPI communication.
 *
 *  \param   writeBuf:    pointer to buffer with data to be written
 *  \param   writeLen:    number of bytes to be written
 *  \param   readBuf:     pointer to buffer for storing read data
 *  \param   readLen:     number of bytes to be read
 *
 *  \return  enum DBC_Error
 */
static enum DBC_Error DBCDRV_sendSpiFrameNbl(const uint8_t *writeBuf, uint16_t writeLen, uint8_t *readBuf, uint16_t readLen)
{
    if((readBuf == NULL) && (writeLen <= sizeof(union DBC_SpiBuf)))
    {
        readLen = writeLen;
        readBuf = DBCDRV_spiBuff.array;
    }
#ifdef DBUSCAN_WITH_BBL_SPI
    return DBCBBL_transfer(writeBuf, writeLen, readBuf, readLen);
#else
  #ifdef DBUSCAN_DMA_USED
    if(DBCDRV_isDmaReady && (MCAL_OK == MSPI_transferDma(&DBCDRV_mspiHandle, writeBuf, writeLen, readBuf, readLen)))
    {
        DBCDRV_isDmaReady = false;
        return DBC_OK;
    }
    return DBC_ERROR;
  #else
    return (MCAL_OK != MSPI_transfer(&DBCDRV_mspiHandle, writeBuf, writeLen, readBuf, readLen)) ? DBC_ERROR : DBC_OK;
  #endif // !DBUSCAN_DMA_USED
#endif // !DBUSCAN_WITH_BBL_SPI
}


/** \brief   Sets the DBusCAN chip to the Standby power mode while considering that SPI CRC might be enabled.
 *
 *  \return  enum DBC_Error
 */
static enum DBC_Error DBCDRV_setPowerModeStandby(void)
{
    const uint32_t SPI_CRC_FOR_SET_STANDBY_CMD = 0xE6EBu; // Precalculated CRC value for the SPI frame which sets device to the STANDBY power mode
    DBCDRV_setSpiHdrSize(0u);
    DBCDRV_setSpiFrameHdr(DBC_MOPC_ADDR, DBCDRV_REG_SIZE, DBC_WRITE_L, DBCDRV_spiBuff.array);
    DBCDRV_spiBuff.d1.data0 = DBCDRV_MOPC_STANDBY_MODE_MASK;
    DBCDRV_spiBuff.d1.data1 = SPI_CRC_FOR_SET_STANDBY_CMD;
    DBC_RETURN_ON_ERROR(DBCDRV_sendSpiFrame(DBCDRV_spiBuff.array, (DBCDRV_REG_SIZE * 2u), DBCDRV_spiBuff.array, 0u)); // sending SPI command (set standby power mode) without SPI CRC
    DBC_RETURN_ON_ERROR(DBCDRV_sendSpiFrame(DBCDRV_spiBuff.array, (DBCDRV_REG_SIZE * 3u), DBCDRV_spiBuff.array, 0u)); // sending SPI command (set standby power mode) with SPI CRC

    DBCDRV_spiBuff.d1.data0 = ~DBCDRV_spiBuff.d1.data0; // change value in the buffer before adding read data to it (to see if data in it really changed)
    (void)DBCDRV_read(DBC_MOPC_ADDR, DBCDRV_spiBuff.array, DBCDRV_REG_SIZE, DBC_READ_L); // data read by this function are checked on the next line as it might return error status if the SPI CRC is enabled in the chip
    if ((DBCDRV_spiBuff.d1.data0 & DBC_MOPC_MODE_SEL_MASK) != DBCDRV_MOPC_STANDBY_MODE_MASK)
    {
        return DBC_ERROR; // the chip is not in STANDBY mode
    }
    return DBC_OK;
}


/** \brief   Performs necessary reset of the DBusCAN chip.
 *
 *  \details This function performs either a full reset or a hard DBus reset depending on the revision,
 *           whether the chip is power-on reset, and whether advanced power management is enabled.
 *           If an "Internal EEPROM CRC error" is detected, the initialization will not be performed,
 *           and the chip must be replaced.
 *
 *  \param   internalEepromError: Pointer to a boolean flag that is set to `true` if an Internal EEPROM
 *           CRC error is detected, preventing further initialization of the chip.
 *
 *  \return  enum DBC_Error
 */
static enum DBC_Error DBCDRV_doReset(bool *internalEepromError)
{
    uint32_t regVal = DBCDRV_readReg32(DBC_IF_ADDR);
    if(0u != (regVal & DBC_IF_ECCERR_INT_MASK)) // check if an Internal EEPROM CRC error is detected
    {
        DBC_RETURN_ON_ERROR(DBCDRV_reset(DBC_RST_DBUS_HARD));
        *internalEepromError = true;
        return DBC_ERROR;
    }
    if (DBCDRV_isAllFeatureRevision())
    {
        DBC_RETURN_ON_ERROR(DBCDRV_reset(DBC_RST_DBUS_HARD));
        return DBC_OK;
    }
    bool isPwrOnReset = (0u != (regVal & DBC_IF_PWRON_MASK));
    bool isAdvPwrMgmt = (0u != (DBCDRV_readReg32(DBC_DBUS_DPC_ADDR) & DBC_DBUS_DPC_ADV_PWR_MGMT_MASK));
    if (isPwrOnReset && !isAdvPwrMgmt)
    {   // full reset as a workaround for the issue of the 1p0 DBusCAN chip (not signalling interrupts when booting in simple power management)
        DBC_RETURN_ON_ERROR(DBCDRV_reset(DBC_RST_FULL));
#ifdef DBUSCAN_SPI_CRC_USED
        DBC_RETURN_ON_ERROR(DBCDRV_enableSpiCrc());
#endif
    }
    else
    {
        DBC_RETURN_ON_ERROR(DBCDRV_reset(DBC_RST_DBUS_HARD));
    }
    return DBC_OK;
}


static enum DBC_Error DBCDRV_enableAndClearIrqFlags(void)
{
    uint32_t regVal;
    regVal = DBCDRV_readReg32(DBC_IE_ADDR);
    regVal &= ~DBC_IF_UVCC_MASK; // disable interrupt on UVCC event (as this flag cannot be cleared by SW)
    DBC_RETURN_ON_ERROR(DBCDRV_writeReg32(DBC_IE_ADDR, regVal));
    DBC_RETURN_ON_ERROR(DBCDRV_writeReg32(DBC_DBUS_IE_ADDR, (DBC_DBUS_IE_ALL_BIT_MASK & ~DBC_DBUS_IE_DBUSSLNT_EN_MASK))); //enable all DBus interrupts except DBus silent flag
    // Clear global interrupt and SPI status flags
    DBC_RETURN_ON_ERROR(DBCDRV_writeReg32(DBC_IF_ADDR, DBCDRV_readReg32(DBC_IF_ADDR)));
    DBC_RETURN_ON_ERROR(DBCDRV_writeReg32(DBC_STATUS_ADDR, DBCDRV_readReg32(DBC_STATUS_ADDR)));
    return DBC_OK;
}


/** \brief   Configures remaining DBus settings in the DBusCAN chip.
 *
 *  \details This function sets up buffers for DBus, communication mode and DBus address filters depending on the application type (bootLoader / programmer / app).
 *
 *  \param   cfg: configuration settings for the DBusCAN chip.
 *
 *  \return  enum DBC_Error
 */
static enum DBC_Error DBCDRV_configureRestForDbus(DBC_Cfg_t cfg)
{
    uint32_t regVal;
    // Configuration of DBus buffers in chip's RAM
    DBCDRV_setSpiFrameHdr(DBC_DBUS_BSA_ADDR, (DBCDRV_REG_SIZE * 3u), DBC_WRITE_L, DBCDRV_spiBuff.array);
    DBCDRV_spiBuff.d2.data0 = DBCDRV_DBUS_BASE_RAM_ADDR; // Dbus Base Address reg (0x4060) => set start address of DBusCAN's RAM allocated for DBus buffers
    DBCDRV_spiBuff.d2.data1 = ((uint32_t)DBCDRV_DBUS_RX_FIFO_SIZE << DBC_DBUS_BSC0_RX_BUF_SIZE_POS) + DBCDRV_DBUS_TX_FIFO_SIZE; // DBus Buffer Size Control 0 reg (0x4064) => set DBus Rx & Tx FIFO size
    DBCDRV_spiBuff.d2.data2 = DBCDRV_DBUS_TXS_FIFO_SIZE; // Dbus buff Size Control 1 reg (0x4068) => set DBus Tx Status FIFO size
#ifdef DBUSCAN_SPI_CRC_USED
    DBCDRV_spiBuff.d2.crc = DBCDRV_calculateCrc(DBCDRV_spiBuff.array);
#endif // DBUSCAN_SPI_CRC_USED
    DBC_RETURN_ON_ERROR(DBCDRV_sendSpiFrame(DBCDRV_spiBuff.array, ((uint16_t)DBCDRV_spiHdrSize + (DBCDRV_REG_SIZE * 3u)), DBCDRV_spiBuff.array, 0u)); // write to registers BSA, BSC0 and BSC1

#ifdef BOOTLOADER_VARIANT
    (void)cfg;
    // Set RAW communication mode over DBus
    regVal = DBCDRV_readReg32(DBC_DBUS_RXC_ADDR);
    regVal |= DBC_DBUS_RXC_RX_MODE_MASK;
    return DBCDRV_writeReg32(DBC_DBUS_RXC_ADDR, regVal);
#else
    // Allow maximum number of DBus Node ID filters
    regVal = DBCDRV_readReg32(DBC_DBUS_SIDFC_ADDR) | DBC_DBUS_SIDFC_LSS_MASK;
    DBC_RETURN_ON_ERROR(DBCDRV_writeReg32(DBC_DBUS_SIDFC_ADDR, regVal));
    // Enable message reception for all subsystems by using node filter
    #ifdef VARIANT_PROGRAMMER
    return DBCDRV_writeReg32(DBC_DBUS_NF0_ADDR, DBC_DBUS_NF_ALL_SUBSYS_EN_MASK | ((uint32_t)cfg.NODE_ID << DBC_DBUS_NF_PID_POS));
    #else // APP_VARIANT
    (void)cfg;
    return DBCDBUS_setMultipleAddresses();
    #endif
#endif // BOOTLOADER_VARIANT
}


#ifdef DBUSCAN_SPI_CRC_USED
/** \brief   Calculates the CRC (CRC-16/CCITT-FALSE) for given SPI frame data.
 *
 *  \note    The data length over which the CRC is calculated is specified in the given SPI frame.
 *
 *  \param   data: Pointer to the SPI frame data array for which the CRC is calculated.
 *
 *  \return  uint32_t The calculated CRC value.
 */
static uint32_t DBCDRV_calculateCrc(const uint8_t *data)
{
    uint16_t totalBytes;
    uint16_t crc = CRC_Ccitt_Init();
    uint16_t dataLen = data[DBC_SPI_HDR_BYTE_DATA_LEN];
    const uint16_t SPI_CMD_DATAWORD_MAX = 256u;
    dataLen = ((dataLen == 0u) ? SPI_CMD_DATAWORD_MAX : dataLen) + 1u;  // 0 in the data length byte of the SPI command represents max. dataword limit (256). Also, by +1 the word of SPI command data is taken into account as well
    totalBytes = (uint16_t)(dataLen * WORD_SIZE);
    for(; totalBytes != 0u; totalBytes--)
    {
        crc = CRC_Ccitt_UpdateByte(crc, *data);
        data++;
    }
    return (uint32_t)(uint16_t)(crc << BYTE_SIZE) | (crc >> BYTE_SIZE);
}


/** \brief   Enables SPI CRC in the DBusCAN chip.
 *
 *  \return  enum DBC_Error
 */
static enum DBC_Error DBCDRV_enableSpiCrc(void)
{
    bool isSpiCrcEnabled, isSpiError;
    const uint32_t SPI_CRC_CFG_MASK = ((uint32_t)DBC_SPI_CRC_POLY_CCITT | (uint32_t)DBC_SPI_CRC_SEED_0XFFFF | DBC_SPI_CRC_CFG_EN_MASK); // Mask for SPI CRC configuration - CRC-16/CCITT-FALSE
    (void)DBCDRV_read(DBC_SPI_CRC_CFG_ADDR, DBCDRV_spiBuff.array, (DBCDRV_REG_SIZE * 2u), DBC_READ_L); // read CRC configuration
    isSpiCrcEnabled = (0u != (DBCDRV_spiBuff.d1.data0 & DBC_SPI_CRC_CFG_EN_MASK)) ? true : false;
    isSpiError = (0u != (DBCDRV_getStatus() & DBC_IF_SPIERR_MASK)) ? true : false;
    DBCDRV_setSpiHdrSize((isSpiCrcEnabled) ? DBC_SPI_CRC_SIZE : 0u);
    if(isSpiError)
    {
        if(isSpiCrcEnabled)
        {
            if((DBCDRV_spiBuff.d1.data0 != SPI_CRC_CFG_MASK) || (0u != DBCDRV_spiBuff.d1.data1))
            {
                (void)DBCDRV_spiReset();  // non expected CRC configuration detected, reset the chip
                return DBC_ERROR;
            }
        }
        (void)DBCDRV_writeReg32(DBC_STATUS_ADDR, DBCDRV_readReg32(DBC_STATUS_ADDR)); // clear SPI error flag - still returns DBC_ERROR
    }
    if(!isSpiCrcEnabled)
    {
        (void)DBCDRV_writeReg32(DBC_SPI_CRC_SEED_ADDR, 0u);              // set CRC seed value to 0
        (void)DBCDRV_writeReg32(DBC_SPI_CRC_CFG_ADDR, SPI_CRC_CFG_MASK); // enable CRC
        DBCDRV_setSpiHdrSize(DBC_SPI_CRC_SIZE);
        DBC_RETURN_ON_ERROR(DBCDRV_read(DBC_SPI_CRC_CFG_ADDR, DBCDRV_spiBuff.array, (DBCDRV_REG_SIZE * 2u), DBC_READ_L)); // read CRC configuration
        DBCDRV_spiCrcReadError = false;
        DBCDRV_checkSpiCrcInReadOp(DBCDRV_spiBuff.array);
        if((DBCDRV_spiBuff.d1.data0 != SPI_CRC_CFG_MASK) || (0u != DBCDRV_spiBuff.d1.data1) || DBCDRV_spiCrcReadError)
        {
            return DBC_ERROR;   // CRC is not correctly enabled
        }
    }
    return DBC_OK;
}

#else // !DBUSCAN_SPI_CRC_USED
/** \brief   Disables SPI CRC in the DBusCAN chip.
 *
 *  \return  enum DBC_Error
 */
static enum DBC_Error DBCDRV_disableSpiCrc(void)
{
    const uint32_t SPI_CRC_FOR_SPI_CRC_DISABLE_CMD = 0xF20Au; // Precalculated CRC value for the SPI frame which disables SPI CRC
    DBCDRV_setSpiHdrSize(0u);
    DBCDRV_setSpiFrameHdr(DBC_SPI_CRC_CFG_ADDR, DBCDRV_REG_SIZE, DBC_WRITE_L, DBCDRV_spiBuff.array);
    DBCDRV_spiBuff.d1.data0 = 0u;
    DBCDRV_spiBuff.d1.data1 = SPI_CRC_FOR_SPI_CRC_DISABLE_CMD;
    DBC_RETURN_ON_ERROR(DBCDRV_sendSpiFrame(DBCDRV_spiBuff.array, (DBCDRV_REG_SIZE * 2u), DBCDRV_spiBuff.array, 0u)); // Disable CRC, sending command twice (both without and with SPI CRC) to ensure correct reception of the command
    DBC_RETURN_ON_ERROR(DBCDRV_sendSpiFrame(DBCDRV_spiBuff.array, (DBCDRV_REG_SIZE * 3u), NULL, 0u));
    DBC_RETURN_ON_ERROR(DBCDRV_read(DBC_SPI_CRC_CFG_ADDR, DBCDRV_spiBuff.array, DBCDRV_REG_SIZE, DBC_READ_L)); // read CRC configuration
    return (0u == (DBCDRV_spiBuff.d1.data0 & DBC_SPI_CRC_CFG_EN_MASK)) ? DBC_OK : DBC_ERROR;                   // check if CRC was disabled
}
#endif // DBUSCAN_SPI_CRC_USED

#ifdef DBUSCAN_DMA_USED
/** \brief   Initializes DMA for communication with the DBusCAN chip over SPI.
 *
 *  \return  enum DBC_Error
 */
static enum DBC_Error DBCDRV_dmaInit(void)
{
    if((MCAL_OK != MDMA_init(&DBCDRV_dmaTxHandle, &DBCDRV_dmaPeriph, &DBCDRV_dmaTxChannel)) ||
       (MCAL_OK != MDMA_init(&DBCDRV_dmaRxHandle, &DBCDRV_dmaPeriph, &DBCDRV_dmaRxChannel)))
    {
        return DBC_ERROR;
    }
    MSPI_initDma(&DBCDRV_mspiHandle, &DBCDRV_dmaTxHandle, &DBCDRV_dmaRxHandle);
    MCAL_initCallback(&DBCDRV_dmaCb, DBCDRV_dmaCbFunction, (void*)&DBCDRV_mspiHandle);
    if(MCAL_OK != MSPI_enableEvent(&DBCDRV_mspiHandle, &DBCDRV_dmaCb, MCAL_EVENT_TRANSFER_COMPLETE))
    {
        return DBC_ERROR;
    }
    return DBC_OK;
}

/** \brief   Callback function invoked after the DMA transfer is completed.
 */
/*lint -esym(818,obj) Pointer parameter 'obj' could be declared as pointing to const [MISRA 2012 Rule 8.13, advisory] */
static void DBCDRV_dmaCbFunction(void *obj, uint32_t flags, const struct MCAL_EventResponse *eventResponse)
{
    if (obj == NULL) {}; (void)flags; if (eventResponse == NULL) {}
    DBCDRV_isDmaReady = true;
}
#endif // DBUSCAN_DMA_USED
