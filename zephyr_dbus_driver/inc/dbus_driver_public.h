#ifndef DBUS_DRIVER_PUBLIC_H__
#define DBUS_DRIVER_PUBLIC_H__

#include "dbus_driver_config.h" // For MCAL_Error, DBC_Error, DBC_Cfg_t, etc.
#include <stdint.h>
#include <stdbool.h>
#include <zephyr/drivers/gpio.h> // Required for struct gpio_callback

enum MEXTI_Trigger {
    MEXTI_TRIGGER_FALLING = 0, // Placeholder
    MEXTI_TRIGGER_RISING, // Added dummy member
};

struct MEXTI_Config {
    enum MEXTI_Trigger trigger; // Placeholder
};

// Forward declaration for gpio_callback
struct gpio_callback;

// MEXTI_Handle definition
struct MEXTI_Handle {
    struct gpio_callback gpio_cb;
    MCAL_Callback_t *mcal_cb;
    uint32_t channel; // Store the channel for context in the callback
};

struct MSPI_Channel {
    void *mspi;
    const struct MDIO_Channel *sclk;
    const struct MDIO_Channel *miso;
    const struct MDIO_Channel *mosi;
    const struct MDIO_Channel *cs;
};

enum DBCDRV_SpiTarget {
    DBCDRV_SPI_TARGET_PRIMARY_PICO = 0,
    DBCDRV_SPI_TARGET_SECONDARY_PICO,
    DBCDRV_SPI_TARGET_COUNT,
};

// MCAL function prototypes
enum MCAL_Error MDIO_init(const struct MDIO_Channel *channel, const void *config);
const struct MDIO_Channel *MEXTI_getPin(uint32_t channel);
bool MDIO_read(const struct MDIO_Channel *channel);
void MDIO_toggle(const struct MDIO_Channel *channel);
void MDIO_write(const struct MDIO_Channel *channel, bool value);
enum MCAL_Error MEXTI_init(struct MEXTI_Handle *handle, uint32_t channel, const struct MEXTI_Config *config);
void MCAL_initCallback(MCAL_Callback_t *cb, MCAL_CallbackFunction_t func, void *obj);
enum MCAL_Error MEXTI_enableEvent(struct MEXTI_Handle *handle, MCAL_Callback_t *cb);
void MEXTI_disableEvent(struct MEXTI_Handle *handle, MCAL_Callback_t *cb);
enum MCAL_Error MSPI_init(struct MSPI_Handle *handle, const void *channel, const void *config);
uint32_t MSPI_getStatus(struct MSPI_Handle *handle);
enum MCAL_Error MSPI_transferDma(struct MSPI_Handle *handle, const uint8_t *writeBuf, uint16_t writeLen, uint8_t *readBuf, uint16_t readLen);
enum MCAL_Error MSPI_transfer(struct MSPI_Handle *handle, const uint8_t *writeBuf, uint16_t writeLen, uint8_t *readBuf, uint16_t readLen);
void MSPI_endCommunication(struct MSPI_Handle *handle);
enum MCAL_Error MDMA_disableChannelEvent(void *handle, MCAL_Callback_t *cb);
void MDMA_disableChannel(void *handle);
void MSPI_disableDMA(void *mspi);
enum MCAL_Error MSPI_transferBlocking(struct MSPI_Handle *handle, const uint8_t *writeBuf, uint16_t writeLen, uint8_t *readBuf, uint16_t readLen);
enum MCAL_Error MDMA_init(struct MDMA_Handle *handle, const void *periph, const void *channel);
void MSPI_initDma(struct MSPI_Handle *handle, struct MDMA_Handle *txHandle, struct MDMA_Handle *rxHandle);
enum MCAL_Error MSPI_enableEvent(struct MSPI_Handle *handle, MCAL_Callback_t *cb, uint32_t event);
void MSUP_setTimeOutExt(struct MSUP_TimeoutHandle *handle, uint32_t timeout_us);
bool MSUP_isTimeOutExt(struct MSUP_TimeoutHandle *handle);
void MSUP_delay(uint32_t delay_us);
uint32_t MTDIV_div_mod(uint32_t numerator, uint32_t denominator);
uint32_t MTDIV_div_32_32(uint32_t numerator, uint32_t denominator);

// DBCDRV function prototypes
DBC_Cfg_t DBCDRV_getConfig(void);
void DBCDRV_setSpiFrameHdr(enum DBC_RegAddr addr, uint16_t len, enum DBC_command cmd, uint8_t *writeBuf);
enum DBC_Error DBCDRV_readReg32(enum DBC_RegAddr addr, uint32_t *data);
enum DBC_Error DBCDRV_writeReg32(enum DBC_RegAddr addr, uint32_t data);
void DBCDRV_setSpiMode(bool cpol, bool cpha);
uint16_t DBCDRV_getSpiMode(void);
enum DBC_Error DBCDRV_setSpiTarget(enum DBCDRV_SpiTarget target);
enum DBCDRV_SpiTarget DBCDRV_getSpiTarget(void);
void DBCDRV_logSpiRouting(const char *tag);
enum DBC_Error DBCDRV_pulseCs(enum DBCDRV_SpiTarget target,
                              uint32_t pulse_count,
                              uint32_t low_time_us,
                              uint32_t high_time_us);
enum DBC_Error DBCDRV_writeRegIpec(uint32_t bitVal, uint32_t bitPos, uint32_t bitMask);
enum DBC_Error DBCDRV_writeEeprom(void);
bool DBCDRV_isSupplyForEepromWrite(void);
enum DBC_Error DBCDRV_setTableDbusBaudrate(uint32_t baudrate, uint32_t clockInput);
enum DBC_Error DBCDRV_setAnyDbusBaudrate(uint32_t baudrate, uint32_t clockInput);
uint32_t DBCDRV_getClockInputInHz(uint32_t clockInput);
enum DBC_Error DBCDRV_sendSpiFrame(enum DBC_command command, uint32_t address, uint8_t *txBuff, uint8_t *rxBuff, uint32_t len);
enum DBC_Error DBCDRV_sendSpiFrameNbl(enum DBC_command command, uint32_t address, uint8_t *txBuff, uint8_t *rxBuff, uint32_t len);
enum DBC_Error DBCDRV_setPowerModeStandby(void);
enum DBC_Error DBCDRV_doReset(bool *internalEepromError);
bool DBCDRV_isAllFeatureRevision(void);
enum DBC_Error DBCDRV_enableAndClearIrqFlags(uint32_t flags);
enum DBC_Error DBCDRV_configureRestForDbus(DBC_Cfg_t *config);
uint16_t DBCDRV_calculateCrc(uint8_t *data, uint32_t len);
enum DBC_Error DBCDRV_enableSpiCrc(void);
enum DBC_Error DBCDRV_disableSpiCrc(void);
enum MCAL_Error DBCDRV_dmaInit(void);
void DBCDRV_dmaCbFunction(void *obj, uint32_t flags, const void *eventResponse);
enum DBC_Error DBCDRV_initComChannels(MCAL_CallbackFunction_t irqHandleCbFunc);
enum DBC_Error DBCDRV_configure(DBC_Cfg_t cfg);
enum DBC_Error DBCDRV_init(void);

#endif // DBUS_DRIVER_PUBLIC_H__