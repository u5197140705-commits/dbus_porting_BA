#include "dbus_driver_config.h"
#include "dbus_app_layer.h" // Include for BYTE_SIZE and other common definitions
#include "dbus_driver_public.h" // Include for public function prototypes
#include <zephyr/logging/log.h>
#include <zephyr/drivers/spi.h> // Include for Zephyr SPI API
#include <zephyr/drivers/can.h> // Include for Zephyr CAN API
#include <string.h> // For memcpy

LOG_MODULE_REGISTER(dbus_driver, LOG_LEVEL_DBG);

// Define the SPI device from device tree
// Define the SPI device from device tree
#define SPI_DEV_NODE DT_INST(0, nxp_lpc_spi)
static const struct device *dbus_spi_bus = DEVICE_DT_GET(SPI_DEV_NODE); // Pointer to the SPI bus

static struct spi_config dbus_spi_cfg = {
    .frequency = 1000000, // Placeholder frequency
    .operation = SPI_OP_MODE_MASTER | SPI_WORD_SET(8) | SPI_HOLD_ON_CS,
    .slave = 0, // Assuming slave select 0
};

// Placeholder for MCAL functions
struct MDIO_Channel MDIO_INPUT_PULL_UP; // Placeholder
struct MDIO_Channel MDIO_OUTPUT; // Placeholder
struct MDIO_Channel MDIO_ALTERNATE_FUNCTION; // Placeholder

enum MCAL_Error MDIO_init(const struct MDIO_Channel *channel, const void *config) {
    LOG_DBG("MDIO_init placeholder called.");
    return MCAL_OK;
}

const struct MDIO_Channel *MEXTI_getPin(uint32_t channel) {
    LOG_DBG("MEXTI_getPin placeholder called.");
    return &MDIO_INPUT_PULL_UP; // Dummy return
}

bool MDIO_read(const struct MDIO_Channel *channel) {
    LOG_DBG("MDIO_read placeholder called.");
    return false; // Dummy return
}

void MDIO_toggle(const struct MDIO_Channel *channel) {
    LOG_DBG("MDIO_toggle placeholder called.");
}

void MDIO_write(const struct MDIO_Channel *channel, bool value) {
    LOG_DBG("MDIO_write placeholder called.");
}

enum MCAL_Error MEXTI_init(struct MEXTI_Handle *handle, uint32_t channel, const struct MEXTI_Config *config) {
    LOG_DBG("MEXTI_init placeholder called.");
    return MCAL_OK;
}

void MCAL_initCallback(MCAL_Callback_t *cb, MCAL_CallbackFunction_t func, void *obj) {
    LOG_DBG("MCAL_initCallback placeholder called.");
}

enum MCAL_Error MEXTI_enableEvent(struct MEXTI_Handle *handle, MCAL_Callback_t *cb) {
    LOG_DBG("MEXTI_enableEvent placeholder called.");
    return MCAL_OK;
}

void MEXTI_disableEvent(struct MEXTI_Handle *handle, MCAL_Callback_t *cb) {
    LOG_DBG("MEXTI_disableEvent placeholder called.");
}

enum MCAL_Error MSPI_init(struct MSPI_Handle *handle, const void *channel, const void *config) {
    LOG_DBG("MSPI_init called.");

    if (!device_is_ready(dbus_spi_bus)) {
        LOG_ERR("SPI device not ready!");
        return MCAL_ERROR;
    }
    return MCAL_OK;
}

uint32_t MSPI_getStatus(struct MSPI_Handle *handle) {
    LOG_DBG("MSPI_getStatus placeholder called.");
    return MCAL_STATUS_READY; // Dummy return
}

enum MCAL_Error MSPI_transferDma(struct MSPI_Handle *handle, const uint8_t *writeBuf, uint16_t writeLen, uint8_t *readBuf, uint16_t readLen) {
    LOG_DBG("MSPI_transferDma called. WriteLen: %u, ReadLen: %u (Note: DMA not fully implemented, using blocking transfer)", writeLen, readLen);
    return MSPI_transferBlocking(handle, writeBuf, writeLen, readBuf, readLen);
}

enum MCAL_Error MSPI_transfer(struct MSPI_Handle *handle, const uint8_t *writeBuf, uint16_t writeLen, uint8_t *readBuf, uint16_t readLen) {
    LOG_DBG("MSPI_transfer called. WriteLen: %u, ReadLen: %u", writeLen, readLen);
    return MSPI_transferBlocking(handle, writeBuf, writeLen, readBuf, readLen);
}

void MSPI_endCommunication(struct MSPI_Handle *handle) {
    LOG_DBG("MSPI_endCommunication placeholder called.");
}

enum MCAL_Error MDMA_disableChannelEvent(void *handle, MCAL_Callback_t *cb) {
    LOG_DBG("MDMA_disableChannelEvent placeholder called.");
    return MCAL_OK;
}

void MDMA_disableChannel(void *handle) {
    LOG_DBG("MDMA_disableChannel placeholder called.");
}

void MSPI_disableDMA(void *mspi) {
    LOG_DBG("MSPI_disableDMA placeholder called.");
}

enum MCAL_Error MSPI_transferBlocking(struct MSPI_Handle *handle, const uint8_t *writeBuf, uint16_t writeLen, uint8_t *readBuf, uint16_t readLen) {
    LOG_DBG("MSPI_transferBlocking called. WriteLen: %u, ReadLen: %u", writeLen, readLen);

    struct spi_buf tx_buf = {
        .buf = (void *)writeBuf,
        .len = writeLen
    };
    struct spi_buf_set tx_bufs = {
        .buffers = &tx_buf,
        .count = 1
    };

    struct spi_buf rx_buf = {
        .buf = (void *)readBuf,
        .len = readLen
    };
    struct spi_buf_set rx_bufs = {
        .buffers = &rx_buf,
        .count = 1
    };

    int ret = spi_transceive(dbus_spi_bus, &dbus_spi_cfg, &tx_bufs, &rx_bufs);
    if (ret) {
        LOG_ERR("SPI transceive failed: %d", ret);
        return MCAL_ERROR;
    }

    return MCAL_OK;
}

enum MCAL_Error MDMA_init(struct MDMA_Handle *handle, const void *periph, const void *channel) {
    LOG_DBG("MDMA_init placeholder called.");
    return MCAL_OK;
}

void MSPI_initDma(struct MSPI_Handle *handle, struct MDMA_Handle *txHandle, struct MDMA_Handle *rxHandle) {
    LOG_DBG("MSPI_initDma placeholder called.");
}

enum MCAL_Error MSPI_enableEvent(struct MSPI_Handle *handle, MCAL_Callback_t *cb, uint32_t event) {
    LOG_DBG("MSPI_enableEvent placeholder called.");
    return MCAL_OK;
}

void MSUP_setTimeOutExt(struct MSUP_TimeoutHandle *handle, uint32_t timeout_us) {
    LOG_DBG("MSUP_setTimeOutExt placeholder called.");
}

bool MSUP_isTimeOutExt(struct MSUP_TimeoutHandle *handle) {
    LOG_DBG("MSUP_isTimeOutExt placeholder called.");
    return true; // Always timeout for now
}

void MSUP_delay(uint32_t delay_us) {
    LOG_DBG("MSUP_delay placeholder called.");
}

uint32_t MTDIV_div_mod(uint32_t numerator, uint32_t denominator) {
    LOG_DBG("MTDIV_div_mod placeholder called.");
    return numerator / denominator; // Simple division for now
}

uint32_t MTDIV_div_32_32(uint32_t numerator, uint32_t denominator) {
    LOG_DBG("MTDIV_div_32_32 placeholder called.");
    return numerator / denominator; // Simple division for now
}

// CAN device binding
// #define CAN_DEV_NODE DT_NODELABEL(can0)
// static const struct device *dbus_can_dev = DEVICE_DT_GET(CAN_DEV_NODE);

// Private data definitions
static bool DBCDRV_isPwrOnReset = false; // Indicates that power on reset of the device has occurred
static union DBC_SpiBuf DBCDRV_spiBuff;              ///< Buffer for writing data to the DBusCAN chip over SPI
static uint8_t DBCDRV_spiHdrSize = DBC_SPI_HDR_SIZE; ///< Size of CRC in SPI frame
static bool DBCDRV_spiCrcReadError = false;          ///< Flag indicating that CRC error occurred during SPI read operation
static uint16_t DBCDRV_spiBuffCrcPtr;                ///< Pointer to the CRC field in the SPI buffer
static bool DBCDRV_eepromWriteEnable = false; // Placeholder for global variable
static uint8_t DBCDRV_readRegBuf[DBC_SPI_BUFFER_SIZE];   ///< Buffer for reading DBusCAN registers via non-blocking SPI communication
static uint8_t DBCDRV_readHdrBuf[DBC_SPI_BUFFER_SIZE];   ///< Buffer for the SPI read command header data

// Placeholder for DBCDRV_getConfig
DBC_Cfg_t DBCDRV_getConfig(void)
{
    DBC_Cfg_t cfg = {{0}};
    // Dummy configuration for testing
    cfg.GP_MEM = 0x01;
    cfg.NODE_ID = 0x01;
    cfg.SUBNODE_ID = 0x00;
    cfg.ADV_PWR_MGMT = 0;
    cfg.BVD_WAIT_EN = 0;
    cfg.BVD_THLD = 0;
    cfg.BVD_TO_NWKRQ = 0;
    cfg.DBR = DBC_DBUS_BAUD_125K;
    cfg.CLKIN = DBC_DBUS_CLKIN_40M;
    cfg.MCAN_EN = 0;
    cfg.DBUS_EN = 1;
    cfg.DBUS2CAN = 0;
    cfg.CAN_BIAS = 0;
    return cfg;
}

// DBCDRV_setSpiFrameHdr function
void DBCDRV_setSpiFrameHdr(enum DBC_RegAddr addr, uint16_t len, enum DBC_command cmd, uint8_t *writeBuf)
{
#ifdef DBUSCAN_SPI_CRC_USED
    DBCDRV_spiBuffCrcPtr = (len / sizeof(uint32_t)) + DBC_SPI_HDR_SIZE; // CRC pointer is after header and data words
#endif
    writeBuf[DBC_SPI_HDR_BYTE_CMD]       = (uint8_t)cmd;
    writeBuf[DBC_SPI_HDR_BYTE_ADDR_HIGH] = (uint8_t)((uint16_t)addr >> BYTE_SIZE);
    writeBuf[DBC_SPI_HDR_BYTE_ADDR_LOW]  = (uint8_t)addr;
    writeBuf[DBC_SPI_HDR_BYTE_DATA_LEN]  = (uint8_t)WORD_SIZEOF(len); // data length must be given in number of words (1 word = 4 bytes)
}

// DBCDRV_readReg32 function
enum DBC_Error DBCDRV_readReg32(enum DBC_RegAddr addr, uint32_t *data)
{
    LOG_DBG("Reading register 0x%x", addr);

    uint8_t tx_buffer[DBC_SPI_HDR_SIZE + sizeof(uint32_t)] = {0};
    uint8_t rx_buffer[DBC_SPI_HDR_SIZE + sizeof(uint32_t)] = {0};

    // Prepare the SPI header for a read command
    DBCDRV_setSpiFrameHdr(addr, sizeof(uint32_t), DBC_CMD_READ, tx_buffer);

    struct spi_buf tx_spi_buf = {
        .buf = tx_buffer,
        .len = sizeof(tx_buffer)
    };
    struct spi_buf_set tx_bufs = {
        .buffers = &tx_spi_buf,
        .count = 1
    };

    struct spi_buf rx_spi_buf = {
        .buf = rx_buffer,
        .len = sizeof(rx_buffer)
    };
    struct spi_buf_set rx_bufs = {
        .buffers = &rx_spi_buf,
        .count = 1
    };

    int ret = spi_transceive(dbus_spi_bus, &dbus_spi_cfg, &tx_bufs, &rx_bufs);
    if (ret) {
        LOG_ERR("SPI read transceive failed: %d", ret);
        return DBC_ERROR;
    }

    // Extract the 32-bit data from the received buffer
    // Assuming the 32-bit data starts after the header
    *data = (uint32_t)rx_buffer[DBC_SPI_HDR_SIZE] << 24 |
            (uint32_t)rx_buffer[DBC_SPI_HDR_SIZE + 1] << 16 |
            (uint32_t)rx_buffer[DBC_SPI_HDR_SIZE + 2] << 8 |
            (uint32_t)rx_buffer[DBC_SPI_HDR_SIZE + 3];

    return DBC_OK;
}

// DBCDRV_writeReg32 function
enum DBC_Error DBCDRV_writeReg32(enum DBC_RegAddr addr, uint32_t data)
{
    LOG_DBG("Writing 0x%x to register 0x%x", data, addr);

    uint8_t tx_buffer[DBC_SPI_HDR_SIZE + sizeof(uint32_t)];
    uint8_t rx_buffer[DBC_SPI_HDR_SIZE + sizeof(uint32_t)]; // Required for spi_transceive, even if data is not used

    // Prepare the SPI header for a write command
    DBCDRV_setSpiFrameHdr(addr, sizeof(uint32_t), DBC_CMD_WRITE, tx_buffer);

    // Copy the 32-bit data into the transmit buffer after the header
    tx_buffer[DBC_SPI_HDR_SIZE]     = (uint8_t)(data >> 24);
    tx_buffer[DBC_SPI_HDR_SIZE + 1] = (uint8_t)(data >> 16);
    tx_buffer[DBC_SPI_HDR_SIZE + 2] = (uint8_t)(data >> 8);
    tx_buffer[DBC_SPI_HDR_SIZE + 3] = (uint8_t)(data);

    struct spi_buf tx_spi_buf = {
        .buf = tx_buffer,
        .len = sizeof(tx_buffer)
    };
    struct spi_buf_set tx_bufs = {
        .buffers = &tx_spi_buf,
        .count = 1
    };

    struct spi_buf rx_spi_buf = {
        .buf = rx_buffer,
        .len = sizeof(rx_buffer)
    };
    struct spi_buf_set rx_bufs = {
        .buffers = &rx_spi_buf,
        .count = 1
    };

    int ret = spi_transceive(dbus_spi_bus, &dbus_spi_cfg, &tx_bufs, &rx_bufs);
    if (ret) {
        LOG_ERR("SPI write transceive failed: %d", ret);
        return DBC_ERROR;
    }

    return DBC_OK;
}

// DBCDRV_writeRegIpec function
enum DBC_Error DBCDRV_writeRegIpec(uint32_t bitVal, uint32_t bitPos, uint32_t bitMask)
{
    uint32_t regVal;
    uint32_t written_val;
    uint32_t read_val;

    LOG_DBG("DBCDRV_writeRegIpec: bitVal=0x%x, bitPos=%u, bitMask=0x%x", bitVal, bitPos, bitMask);

    DBC_RETURN_ON_ERROR(DBCDRV_readReg32(DBC_IPEC_ADDR, &regVal));
    LOG_DBG("DBCDRV_writeRegIpec: Initial read of DBC_IPEC_ADDR (0x%x): 0x%x", DBC_IPEC_ADDR, regVal);

    if (((regVal & bitMask) >> bitPos) == bitVal)
    {
        LOG_DBG("DBCDRV_writeRegIpec: Desired bitfield already set. Returning OK.");
        return DBC_OK;
    }

    // enable write access to IPEC reg
    written_val = regVal | DBC_IPEC_CCE_MASK;
    LOG_DBG("DBCDRV_writeRegIpec: Enabling write access to IPEC. Writing 0x%x to DBC_IPEC_ADDR (0x%x)", written_val, DBC_IPEC_ADDR);
    DBC_RETURN_ON_ERROR(DBCDRV_writeReg32(DBC_IPEC_ADDR, written_val));
    DBC_RETURN_ON_ERROR(DBCDRV_readReg32(DBC_IPEC_ADDR, &read_val));
    LOG_DBG("DBCDRV_writeRegIpec: After enabling write access. Read back: 0x%x. Match: %d", read_val, (written_val == read_val));
    regVal = read_val; // Update regVal with the actual read-back value

    // configure desired bit/bitfield value
    regVal &= ~bitMask;
    regVal |= (bitVal << bitPos);

    // disable write access to IPEC reg
    written_val = regVal & ~DBC_IPEC_CCE_MASK;
    LOG_DBG("DBCDRV_writeRegIpec: Configuring bitfield and disabling write access. Writing 0x%x to DBC_IPEC_ADDR (0x%x)", written_val, DBC_IPEC_ADDR);

    // apply new IPEC reg settings
    DBC_RETURN_ON_ERROR(DBCDRV_writeReg32(DBC_IPEC_ADDR, written_val));
    DBC_RETURN_ON_ERROR(DBCDRV_readReg32(DBC_IPEC_ADDR, &read_val));
    LOG_DBG("DBCDRV_writeRegIpec: After configuring bitfield. Read back: 0x%x. Match: %d", read_val, (written_val == read_val));
    regVal = read_val; // Update regVal with the actual read-back value

    DBC_RETURN_ON_ERROR(DBCDRV_readReg32(DBC_IPEC_ADDR, &regVal));
    uint32_t final_reg_val_masked = regVal & bitMask;
    final_reg_val_masked &= ~DBC_IPEC_EP_CC_MASK; // do not include EP_CC bitfield as it always reads zeros
    uint32_t expected_val = (bitVal << bitPos);
    LOG_DBG("DBCDRV_writeRegIpec: Final verification. Read 0x%x, Masked 0x%x, Expected 0x%x. Result: %d", regVal, final_reg_val_masked, expected_val, (final_reg_val_masked == expected_val));

    return (final_reg_val_masked != expected_val) ? DBC_ERROR : DBC_OK;
}

// DBCDRV_writeEeprom function
enum DBC_Error DBCDRV_writeEeprom(void)
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
            DBC_RETURN_ON_ERROR(DBCDRV_readReg32(DBC_EEPP_ADDR, &eepPending));
            uint32_t eepCurrent;
            DBC_RETURN_ON_ERROR(DBCDRV_readReg32(DBC_EEPC_ADDR, &eepCurrent));
            if (eepCurrent == eepPending)
            {   // nothing to do, all EEPROM bits are set as expected
                return DBC_OK;
            }
            if (true != DBCDRV_isSupplyForEepromWrite())
            {   // insufficient supply- not possible to flash EEPROM
                DBCDRV_eepromWriteEnable = false;
                return DBC_ERROR;
            }
            DBC_RETURN_ON_ERROR(DBCDRV_readReg32(DBC_IPEC_ADDR, &regVal));
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
            DBC_RETURN_ON_ERROR(DBCDRV_readReg32(DBC_IPEC_ADDR, &regVal));
            regVal &= DBC_IPEC_EP_WR_MASK;
            writeState = (DBC_IPEC_EP_WR_MASK == regVal) ? DBCDRV_EEP_WRITE_ONGOING : DBCDRV_EEP_WRITE_END; // wait until EEPROM write process is finished
            break;
        }
        case DBCDRV_EEP_WRITE_END:
        {
            DBC_RETURN_ON_ERROR(DBCDRV_readReg32(DBC_EEPP_ADDR, &eepPending));
            uint32_t eepCurrent;
            DBC_RETURN_ON_ERROR(DBCDRV_readReg32(DBC_EEPC_ADDR, &eepCurrent));
            if (eepCurrent != eepPending)
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

// DBCDRV_isSupplyForEepromWrite function
bool DBCDRV_isSupplyForEepromWrite(void)
{
    LOG_DBG("DBCDRV_isSupplyForEepromWrite placeholder called.");
    return true; // Placeholder, assume supply is always sufficient
}

// DBCDRV_setTableDbusBaudrate function
enum DBC_Error DBCDRV_setTableDbusBaudrate(uint32_t baudrate, uint32_t clockInput)
{
    LOG_DBG("DBCDRV_setTableDbusBaudrate placeholder called. Baudrate: %u, Clock Input: %u", baudrate, clockInput);
    return DBC_OK;
}

// DBCDRV_setAnyDbusBaudrate function
enum DBC_Error DBCDRV_setAnyDbusBaudrate(uint32_t baudrate, uint32_t clockInput)
{
    LOG_DBG("DBCDRV_setAnyDbusBaudrate placeholder called. Baudrate: %u, Clock Input: %u", baudrate, clockInput);
    return DBC_OK;
}

// DBCDRV_getClockInputInHz function
uint32_t DBCDRV_getClockInputInHz(uint32_t clockInput)
{
    LOG_DBG("DBCDRV_getClockInputInHz placeholder called. Clock Input: %u", clockInput);
    switch (clockInput) {
        case DBC_DBUS_CLKIN_8M: return 8000000;
        case DBC_DBUS_CLKIN_16M: return 16000000;
        case DBC_DBUS_CLKIN_20M: return 20000000;
        case DBC_DBUS_CLKIN_40M: return 40000000;
        default: return 0; // Invalid clock input
    }
}

enum DBC_Error DBCDRV_sendSpiFrame(enum DBC_command command, uint32_t address, uint8_t *txBuff, uint8_t *rxBuff, uint32_t len)
{
    LOG_DBG("DBCDRV_sendSpiFrame placeholder called. Command: %u, Address: 0x%x, Length: %u", command, address, len);
    // Placeholder for actual SPI transfer logic
    return DBC_OK;
}

// DBCDRV_sendSpiFrameNbl function
enum DBC_Error DBCDRV_sendSpiFrameNbl(enum DBC_command command, uint32_t address, uint8_t *txBuff, uint8_t *rxBuff, uint32_t len)
{
    LOG_DBG("DBCDRV_sendSpiFrameNbl placeholder called. Command: %u, Address: 0x%x, Length: %u (Note: DMA not fully implemented, using blocking transfer)", command, address, len);

    if (len > DBC_SPI_MAX_DATA_LEN) {
        LOG_ERR("SPI data length exceeds max allowed: %u", len);
        return DBC_ERROR;
    }

    // Prepare transmit buffer
    DBCDRV_setSpiFrameHdr(address, len, command, DBCDRV_spiBuff.byte);
    if (txBuff != NULL && len > 0) {
        memcpy(&DBCDRV_spiBuff.byte[DBC_SPI_HDR_SIZE], txBuff, len);
    }

#ifdef DBUSCAN_SPI_CRC_USED
    uint16_t crc = DBCDRV_calculateCrc(DBCDRV_spiBuff.byte, DBC_SPI_HDR_SIZE + len);
    DBCDRV_spiBuff.byte[DBC_SPI_HDR_SIZE + len] = (uint8_t)(crc >> BYTE_SIZE);
    DBCDRV_spiBuff.byte[DBC_SPI_HDR_SIZE + len + 1] = (uint8_t)crc;
    uint32_t total_len = DBC_SPI_HDR_SIZE + len + DBC_SPI_CRC_SIZE;
#else
    uint32_t total_len = DBC_SPI_HDR_SIZE + len;
#endif

    // Perform non-blocking SPI transfer (currently mapped to blocking)
    enum MCAL_Error mcal_err = MSPI_transferDma(NULL, DBCDRV_spiBuff.byte, total_len, DBCDRV_readHdrBuf, total_len);
    if (mcal_err != MCAL_OK) {
        LOG_ERR("SPI non-blocking transfer failed in DBCDRV_sendSpiFrameNbl: %d", mcal_err);
        return DBC_ERROR;
    }

    // For non-blocking, the rxBuff would typically be filled in a callback.
    // Since MSPI_transferDma currently uses blocking, we can copy here.
    if (rxBuff != NULL && len > 0) {
        memcpy(rxBuff, &DBCDRV_readHdrBuf[DBC_SPI_HDR_SIZE], len);
    }

#ifdef DBUSCAN_SPI_CRC_USED
    // CRC verification for non-blocking would typically happen in the callback.
    // For now, with blocking behavior, we can check here.
    uint16_t received_crc = (uint16_t)(DBCDRV_readHdrBuf[DBC_SPI_HDR_SIZE + len] << BYTE_SIZE) | DBCDRV_readHdrBuf[DBC_SPI_HDR_SIZE + len + 1];
    uint16_t calculated_crc = DBCDRV_calculateCrc(DBCDRV_readHdrBuf, DBC_SPI_HDR_SIZE + len);

    if (received_crc != calculated_crc) {
        LOG_ERR("SPI CRC mismatch in NBL! Received: 0x%x, Calculated: 0x%x", received_crc, calculated_crc);
        DBCDRV_spiCrcReadError = true;
        return DBC_ERROR;
    } else {
        DBCDRV_spiCrcReadError = false;
    }
#endif

    return DBC_OK;
}

// DBCDRV_setPowerModeStandby function
enum DBC_Error DBCDRV_setPowerModeStandby(bool enable)
{
    LOG_DBG("DBCDRV_setPowerModeStandby placeholder called. Enable: %d", enable);
    return DBC_OK;
}

// DBCDRV_doReset function
enum DBC_Error DBCDRV_doReset(enum DBC_Reset resetType)
{
    LOG_DBG("DBCDRV_doReset placeholder called. Reset Type: %u", resetType);
    return DBC_OK;
}

// DBCDRV_enableAndClearIrqFlags function
enum DBC_Error DBCDRV_enableAndClearIrqFlags(uint32_t flags)
{
    LOG_DBG("DBCDRV_enableAndClearIrqFlags placeholder called. Flags: 0x%x", flags);
    return DBC_OK;
}

// DBCDRV_enableCfgDbus function
enum DBC_Error DBCDRV_enableCfgDbus(void)
{
    uint32_t regVal;
    DBC_RETURN_ON_ERROR(DBCDRV_readReg32(DBC_DBUS_CCCR_ADDR, &regVal));

    if ((regVal & DBC_DBUS_CCCR_INIT_MASK) != DBC_DBUS_CCCR_INIT_MASK)
    {
        regVal |= DBC_DBUS_CCCR_INIT_MASK;
        DBC_RETURN_ON_ERROR(DBCDRV_writeReg32(DBC_DBUS_CCCR_ADDR, regVal));
    }
    regVal |= DBC_DBUS_CCCR_CCE_MASK;
    DBC_RETURN_ON_ERROR(DBCDRV_writeReg32(DBC_DBUS_CCCR_ADDR, regVal));

    return DBC_OK;
}

// DBCDRV_disableCfgDbus function
enum DBC_Error DBCDRV_disableCfgDbus(void)
{
    uint32_t regVal;
    DBC_RETURN_ON_ERROR(DBCDRV_readReg32(DBC_DBUS_CCCR_ADDR, &regVal));
    regVal &= ~(DBC_DBUS_CCCR_CCE_MASK | DBC_DBUS_CCCR_INIT_MASK);
    return DBCDRV_writeReg32(DBC_DBUS_CCCR_ADDR, regVal);
}

// DBCDRV_configureRestForDbus function
enum DBC_Error DBCDRV_configureRestForDbus(DBC_Cfg_t *config)
{
    LOG_DBG("DBCDRV_configureRestForDbus placeholder called.");
    // Placeholder for configuring RAM buffers, communication mode, and node filters.
    return DBC_OK;
}

// DBCDRV_calculateCrc function
uint16_t DBCDRV_calculateCrc(uint8_t *data, uint32_t len)
{
    LOG_DBG("DBCDRV_calculateCrc placeholder called. Length: %u", len);
    return 0; // Dummy CRC
}

// DBCDRV_enableSpiCrc function
enum DBC_Error DBCDRV_enableSpiCrc(void)
{
    LOG_DBG("DBCDRV_enableSpiCrc placeholder called.");
    return DBC_OK;
}

// DBCDRV_disableSpiCrc function
enum DBC_Error DBCDRV_disableSpiCrc(void)
{
    LOG_DBG("DBCDRV_disableSpiCrc placeholder called.");
    return DBC_OK;
}

// DBCDRV_dmaInit function
enum MCAL_Error DBCDRV_dmaInit(void)
{
    LOG_DBG("DBCDRV_dmaInit placeholder called.");
    return MCAL_OK;
}

// DBCDRV_dmaCbFunction function
void DBCDRV_dmaCbFunction(void *obj, uint32_t flags, const void *eventResponse)
{
    LOG_DBG("DBCDRV_dmaCbFunction placeholder called.");
}

// DBCDRV_initComChannels function
enum DBC_Error DBCDRV_initComChannels(void)
{
    LOG_DBG("DBCDRV_initComChannels placeholder called.");
    return DBC_OK;
}

enum DBC_Error DBCDRV_configure(DBC_Cfg_t cfg)
{
    uint32_t bitMask = 0u;
    uint32_t regVal;
    DBC_Cfg_t currCfg;

    DBC_RETURN_ON_ERROR(DBCDRV_readReg32(DBC_EEPP_ADDR, &currCfg.word)); // get current configuration of the chip

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
                DBC_RETURN_ON_ERROR(DBCDRV_readReg32(DBC_DBUS_DPC_ADDR, &regVal));
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
                DBC_RETURN_ON_ERROR(DBCDRV_readReg32(DBC_DBUS_SIDFC_ADDR, &regVal));
                UTI_RES(regVal, DBC_DBUS_SIDFC_SID_MASK);
                UTI_SET(regVal, ((uint32_t)cfg.SUBNODE_ID));
                UTI_RES(regVal, DBC_DBUS_SIDFC_PID_MASK);
                UTI_SET(regVal, ((uint32_t)cfg.NODE_ID << DBC_DBUS_SIDFC_PID_POS));
                DBC_RETURN_ON_ERROR(DBCDRV_writeReg32(DBC_DBUS_SIDFC_ADDR, regVal));
            }
            // configuration of DBUS_DBR register bits
            if ((currCfg.word & DBC_EEP_BITS_REG_DBUS_DBR_MASK) != (cfg.word & DBC_EEP_BITS_REG_DBUS_DBR_MASK))
            {
                DBC_RETURN_ON_ERROR(DBCDRV_readReg32(DBC_DBUS_DBR_ADDR, &regVal));
                UTI_RES(regVal, DBC_DBUS_DBR_DBR_MASK);
                UTI_SET(regVal, ((uint32_t)cfg.DBR));
                UTI_RES(regVal, DBC_DBUS_DBR_CLKIN_MASK);
                UTI_SET(regVal, ((uint32_t)cfg.CLKIN << DBC_DBUS_DBR_CLKIN_POS));
                DBC_RETURN_ON_ERROR(DBCDRV_writeReg32(DBC_DBUS_DBR_ADDR, regVal));
            }
            DBC_RETURN_ON_ERROR(DBCDRV_disableCfgDbus()); // lock DBus regs for write access
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

        DBC_RETURN_ON_ERROR(DBCDRV_readReg32(DBC_EEPP_ADDR, &currCfg.word));
        if (currCfg.word != cfg.word)
        {
            return DBC_ERROR;
        }
    }
    return DBC_OK;
}

enum DBC_Error DBCDRV_init(void)
{
    static bool DBCDRV_isInternalEepromError = false; // Indicates if an Internal EEPROM CRC error was detected during initialization; do not initialize the chip if true
    DBC_Cfg_t cfg = DBCDRV_getConfig();
    if(DBCDRV_isInternalEepromError)
    {
        return DBC_ERROR;
    }
    DBC_RETURN_ON_ERROR(DBCDRV_initComChannels());
    DBC_RETURN_ON_ERROR(DBCDRV_setPowerModeStandby(true)); // Set to standby mode
#ifdef DBUSCAN_SPI_CRC_USED
    DBC_RETURN_ON_ERROR(DBCDRV_enableSpiCrc());
#else
    DBC_RETURN_ON_ERROR(DBCDRV_disableSpiCrc());
#endif
    DBC_RETURN_ON_ERROR(DBCDRV_doReset(DBC_RST_FULL)); // Assuming full reset for now, will refine based on original logic
    DBC_RETURN_ON_ERROR(DBCDRV_configure(cfg));
    DBC_RETURN_ON_ERROR(DBCDRV_writeRegIpec(0uL, DBC_IPEC_NWKRQ_DELAY_POS, DBC_IPEC_NWKRQ_DELAY_MASK)); // clear NWKRQ_DELAY bit to disable nWKRQ pin delayed de-assertion in sleep mode
    // DBC_RETURN_ON_ERROR(DBCDRV_enableCfgDbus()); // This is handled within DBCDRV_configure
    DBC_RETURN_ON_ERROR(DBCDRV_enableAndClearIrqFlags(0)); // Placeholder for flags
    DBC_RETURN_ON_ERROR(DBCDRV_configureRestForDbus(&cfg));
    DBC_RETURN_ON_ERROR(DBCDRV_setPowerModeStandby(false)); // Set to normal mode (false for standby means normal)

    DBCDRV_isPwrOnReset = false; // Assuming APP_VARIANT behavior

    return DBC_OK;
}