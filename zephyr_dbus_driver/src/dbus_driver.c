#include "dbus_driver_config.h"
#include "dbus_app_layer.h" // Include for BYTE_SIZE and other common definitions
#include "dbus_driver_public.h" // Include for public function prototypes
#include <zephyr/logging/log.h>
#include <zephyr/drivers/spi.h> // Include for Zephyr SPI API
#include <zephyr/drivers/can.h> // Include for Zephyr CAN API
#include <zephyr/drivers/gpio.h> // Include for Zephyr GPIO API
#include <string.h> // For memcpy
#include "StdCrc.h" // Include for CRC functions
#include "bsh_stdinc.h" // Include for standard definitions

LOG_MODULE_REGISTER(dbus_driver, LOG_LEVEL_DBG);

// Define the SPI device from device tree
// Define the SPI device from device tree
#define SPI_DEV_NODE DT_NODELABEL(flexcomm1)
static const struct device *dbus_spi_bus = DEVICE_DT_GET(SPI_DEV_NODE); // Pointer to the SPI bus

#define DBUS_CS_GPIO_NODE DT_NODELABEL(hsgpio0)
#define DBUS_CS_GPIO_PRIMARY_PIN 6
#define DBUS_CS_GPIO_SECONDARY_PIN 10
#define DBUS_CS_GPIO_FLAGS GPIO_OUTPUT
#define DBCDRV_SPI_RSP_MARKER 0xA0u
#define DBCDRV_SPI_CS_SETUP_US 8u
#define DBCDRV_SPI_CS_HOLD_US 2000u
#define DBCDRV_SPI_DUMMY_RETRIES 32u
#define DBCDRV_SPI_DUMMY_GAP_US 500u
#define DBCDRV_LOG_READ_RETRY_HEXDUMPS 0
#define DBCDRV_LOG_WRITE_HEXDUMPS 0
#define DBCDRV_LOG_PROVISIONAL_INTERLEAVED 0
#define DBCDRV_LOG_FAIL_HEXDUMPS 0
#define DBCDRV_LOG_FAIL_LANE_TAILS 0
#define DBCDRV_FAIL_DIAG_TAIL_BYTES 32u
#define DBCDRV_FAIL_DIAG_LANE_TAIL_FRAMES 8u
#define DBCDRV_FAIL_DIAG_MAX_BYTE_HITS 24u

static const struct device *dbus_cs_gpio_dev = DEVICE_DT_GET(DBUS_CS_GPIO_NODE);
static enum DBCDRV_SpiTarget dbus_spi_target = DBCDRV_SPI_TARGET_PRIMARY_PICO;
static bool dbus_secondary_cs_initialized = false;
static uint32_t dbus_drv_speed_write_seq = 0u;

static struct spi_config dbus_spi_cfg = {
    .frequency = 100000, // 100 kHz keeps the debug-era readback heuristics but avoids the very slow LCD/sonic refresh
    .operation = SPI_OP_MODE_MASTER | SPI_WORD_SET(8) | SPI_TRANSFER_MSB,
    .slave = 0,
    /* Flexcomm HW SSEL0 on GPIO6, manual CS only on GPIO10. */
    .cs = { 0 },
};

// Placeholder for MCAL functions
struct MDIO_Channel MDIO_INPUT_PULL_UP; // Placeholder
struct MDIO_Channel MDIO_OUTPUT; // Placeholder
struct MDIO_Channel MDIO_ALTERNATE_FUNCTION; // Placeholder

// Global variables for DBusCAN driver
struct MSPI_Handle DBCDRV_mspiHandle;
// Dummy MDIO_Channel instances for SPI pins
const struct MDIO_Channel MDIOB13_MSPI2_SCK = {0};
const struct MDIO_Channel MDIOB14_MSPI2_MISO = {0};
const struct MDIO_Channel MDIOB15_MSPI2_MOSI = {0};
const struct MDIO_Channel MDIOB12 = {0};
// Define MEXTID3 as a specific MDIO_Channel, assuming GPIO port 0 and pin 13
const struct MDIO_Channel MEXTID3 = { .dummy = 18 }; // Using dummy to store pin number, actual GPIO handled by Zephyr API
 
// Define the GPIO device for MEXTID3
#define MEXTID3_GPIO_PORT_NODE DT_NODELABEL(hsgpio0)
#define MEXTID3_GPIO_PIN 18

// Global variable to hold the GPIO device pointer
static const struct device *mextid3_gpio_dev = DEVICE_DT_GET(MEXTID3_GPIO_PORT_NODE);

const struct MSPI_Channel DBCDRV_mspiChannel = {
    .mspi = NULL, // Placeholder, as Zephyr's SPI API doesn't directly use this
    .sclk = &MDIOB13_MSPI2_SCK,
    .miso = &MDIOB14_MSPI2_MISO,
    .mosi = &MDIOB15_MSPI2_MOSI,
    .cs   = &MDIOB12
};
const struct spi_config DBCDRV_mspiCfg = { // Placeholder for MSPI_Config, using Zephyr's spi_config
    .frequency = 1000000,
    .operation = SPI_OP_MODE_MASTER | SPI_WORD_SET(8) | SPI_MODE_CPOL | SPI_MODE_CPHA,
    .slave = 0,
};

static uint32_t dbus_drv_get_cs_pin(enum DBCDRV_SpiTarget target)
{
    switch (target) {
    case DBCDRV_SPI_TARGET_PRIMARY_PICO:
        return DBUS_CS_GPIO_PRIMARY_PIN;
    case DBCDRV_SPI_TARGET_SECONDARY_PICO:
        return DBUS_CS_GPIO_SECONDARY_PIN;
    default:
        return DBUS_CS_GPIO_PRIMARY_PIN;
    }
}

static const char *dbus_drv_get_target_name(enum DBCDRV_SpiTarget target)
{
    switch (target) {
    case DBCDRV_SPI_TARGET_PRIMARY_PICO:
        return "primary";
    case DBCDRV_SPI_TARGET_SECONDARY_PICO:
        return "secondary";
    default:
        return "unknown";
    }
}

static int dbus_drv_configure_cs_pin(uint32_t pin)
{
    int ret = gpio_pin_configure(dbus_cs_gpio_dev, pin, DBUS_CS_GPIO_FLAGS);

    if (ret < 0) {
        return ret;
    }

    return gpio_pin_set(dbus_cs_gpio_dev, pin, 1);
}

static int dbus_drv_ensure_secondary_cs_ready(void)
{
    int ret;

    if (dbus_secondary_cs_initialized) {
        return 0;
    }

    ret = dbus_drv_configure_cs_pin(DBUS_CS_GPIO_SECONDARY_PIN);
    if (ret < 0) {
        return ret;
    }

    dbus_secondary_cs_initialized = true;
    return 0;
}

static int dbus_drv_set_cs_state(enum DBCDRV_SpiTarget target, bool asserted)
{
    /* Flexcomm SSEL0 (GPIO6) is handled natively by the SPI controller.
     * We manually drive only GPIO10 for selecting the secondary Pico. */
    if (target == DBCDRV_SPI_TARGET_SECONDARY_PICO) {
        int ret = dbus_drv_ensure_secondary_cs_ready();

        if (ret < 0) {
            return ret;
        }

        ret = gpio_pin_set(dbus_cs_gpio_dev,
                           DBUS_CS_GPIO_SECONDARY_PIN,
                           asserted ? 0u : 1u);

        if (ret < 0) {
            return ret;
        }

        return 0;
    }

    return 0;
}

// MEXTI related definitions
struct MEXTI_Handle DBCDRV_mextiHandle;
MCAL_Callback_t DBCDRV_cbIrqHandle;

// Static Zephyr GPIO callback function
static void zephyr_gpio_callback(const struct device *port, struct gpio_callback *cb, gpio_port_pins_t pins) {
    struct MEXTI_Handle *handle = CONTAINER_OF(cb, struct MEXTI_Handle, gpio_cb);
    if (handle && handle->mcal_cb && handle->mcal_cb->cbFunction) {
        // Pass the pins as flags and NULL for eventResponse for now
        handle->mcal_cb->cbFunction(handle->mcal_cb->obj, pins, NULL);
    }
}

// Define MEXTID3 as a specific MDIO_Channel

// DBCDRV_getMextiChannel function
uint32_t DBCDRV_getMextiChannel(void) {
    return 3;
}

// Dummy callback function for IRQ
void DBCDRV_irqCallback(void *obj, uint32_t flags, const struct MCAL_EventResponse *eventResponse) {
    LOG_DBG("DBCDRV_irqCallback called. Flags: 0x%x", flags);
    uint32_t if_reg_val = 0;
    uint32_t dbus_if_reg_val = 0;

    // Read and log the Device Interrupt Flags register
    if (DBCDRV_readReg32(DBC_IF_ADDR, &if_reg_val) == DBC_OK) {
        LOG_DBG("DBCDRV_irqCallback: DBC_IF_ADDR (0x%x) = 0x%x", DBC_IF_ADDR, if_reg_val);
        // Clear the flags after reading
        (void)DBCDRV_writeReg32(DBC_IF_ADDR, if_reg_val);
    } else {
        printk("DBCDRV_irqCallback: Failed to read DBC_IF_ADDR\n");
    }

    // Read and log the DBus Interrupt Flags register
    if (DBCDRV_readReg32(DBC_DBUS_IF_ADDR, &dbus_if_reg_val) == DBC_OK) {
        LOG_DBG("DBCDRV_irqCallback: DBC_DBUS_IF_ADDR (0x%x) = 0x%x", DBC_DBUS_IF_ADDR, dbus_if_reg_val);
        // Clear the flags after reading
        (void)DBCDRV_writeReg32(DBC_DBUS_IF_ADDR, dbus_if_reg_val);
    } else {
        printk("DBCDRV_irqCallback: Failed to read DBC_DBUS_IF_ADDR\n");
    }

    // Further processing of interrupt flags would go here
}

enum MCAL_Error MDIO_init(const struct MDIO_Channel *channel, const void *config) {
    LOG_DBG("MDIO_init called for pin %d.", channel->dummy);

    if (!device_is_ready(mextid3_gpio_dev)) {
        printk("MDIO_init: GPIO device not ready!\n");
        return MCAL_ERROR;
    }

    int ret = gpio_pin_configure(mextid3_gpio_dev, MEXTID3_GPIO_PIN, GPIO_INPUT | GPIO_PULL_UP);
    if (ret < 0) {
        printk("MDIO_init: Failed to configure MEXTID3 pin: %d\n", ret);
        return MCAL_ERROR;
    }

    ret = gpio_pin_interrupt_configure(mextid3_gpio_dev, MEXTID3_GPIO_PIN, GPIO_INT_EDGE_FALLING);
    if (ret < 0) {
        printk("MDIO_init: Failed to configure MEXTID3 interrupt: %d\n", ret);
        return MCAL_ERROR;
    }

    return MCAL_OK;
}

const struct MDIO_Channel *MEXTI_getPin(uint32_t channel) {
    LOG_DBG("MEXTI_getPin called for channel %u.", channel);
    if (channel == 3) {
        return &MEXTID3;
    }
    return NULL; // Return NULL for unsupported channels
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
    int ret;
    gpio_flags_t flags = GPIO_INT_EDGE_BOTH;

    LOG_DBG("MEXTI_init called for channel %u.", channel);

    if (!device_is_ready(mextid3_gpio_dev)) {
        printk("MEXTI_init: GPIO device not ready\n");
        return MCAL_ERROR;
    }

    ret = gpio_pin_interrupt_configure(mextid3_gpio_dev, MEXTID3_GPIO_PIN, flags);
    if (ret < 0) {
        printk("MEXTI_init: Failed to configure MEXTID3 interrupt: %d\n", ret);
        return MCAL_ERROR;
    }

    return MCAL_OK;
}

void MCAL_initCallback(MCAL_Callback_t *cb, MCAL_CallbackFunction_t func, void *obj) {
    if (cb != NULL) {
        cb->cbFunction = func;
        cb->obj = obj;
        LOG_DBG("MCAL_initCallback initialized. Function: %p, Object: %p", (void*)func, obj);
    } else {
        printk("MCAL_initCallback called with NULL callback handle.\n");
    }
}

enum MCAL_Error MEXTI_enableEvent(struct MEXTI_Handle *handle, MCAL_Callback_t *cb) {
    LOG_DBG("MEXTI_enableEvent called.");
    if (handle == NULL || cb == NULL) {
        printk("MEXTI_enableEvent called with NULL handle or callback.\n");
        return MCAL_ERROR;
    }
    handle->mcal_cb = cb; // Store the MCAL callback in the MEXTI handle
    // The interrupt is already configured and added in MEXTI_init.
    // This function primarily associates the MCAL callback with the MEXTI handle.
    return MCAL_OK;
}

void MEXTI_disableEvent(struct MEXTI_Handle *handle, MCAL_Callback_t *cb) {
    LOG_DBG("MEXTI_disableEvent called.");
    if (handle == NULL || cb == NULL) {
        printk("MEXTI_disableEvent called with NULL handle or callback.\n");
        return;
    }
    // Remove the GPIO callback
    int ret = gpio_remove_callback(mextid3_gpio_dev, &handle->gpio_cb);
    if (ret < 0) {
        printk("MEXTI_disableEvent: Failed to remove GPIO callback: %d\n", ret);
    }
    // Disable the interrupt
    ret = gpio_pin_interrupt_configure(mextid3_gpio_dev, MEXTID3_GPIO_PIN, GPIO_INT_DISABLE);
    if (ret < 0) {
        printk("MEXTI_disableEvent: Failed to disable MEXTID3 interrupt: %d\n", ret);
    }
    handle->mcal_cb = NULL; // Clear the MCAL callback
}

enum MCAL_Error MSPI_init(struct MSPI_Handle *handle, const void *channel, const void *config) {
    LOG_DBG("MSPI_init called.");

    if (!device_is_ready(dbus_spi_bus)) {
        printk("MSPI_init: SPI device not ready!\n");
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

    if (!device_is_ready(dbus_cs_gpio_dev)) {
        printk("MSPI_transferBlocking: CS GPIO device not ready!\n");
        return MCAL_ERROR;
    }

    // Assert CS (drive low)
    if (dbus_drv_set_cs_state(dbus_spi_target, true) < 0) {
        LOG_ERR("MSPI_transferBlocking: failed to assert CS for %s Pico",
                dbus_drv_get_target_name(dbus_spi_target));
        return MCAL_ERROR;
    }
    k_usleep(10); // Small delay after asserting CS

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
        printk("MSPI_transferBlocking: SPI transceive failed: %d\n", ret);
        return MCAL_ERROR;
    }
    LOG_DBG("MSPI_transferBlocking: SPI transceive successful. Read %u bytes.", readLen);
    LOG_HEXDUMP_DBG(readBuf, readLen, "MSPI_transferBlocking RX:");

    // Deassert CS (drive high)
    if (dbus_drv_set_cs_state(dbus_spi_target, false) < 0) {
        LOG_ERR("MSPI_transferBlocking: failed to deassert CS for %s Pico",
                dbus_drv_get_target_name(dbus_spi_target));
        return MCAL_ERROR;
    }
    k_usleep(5); // Small delay after deasserting CS

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
// Removed unused variables: DBCDRV_spiHdrSize, DBCDRV_spiCrcReadError, DBCDRV_spiBuffCrcPtr, DBCDRV_readRegBuf
static bool DBCDRV_eepromWriteEnable = false; // Placeholder for global variable
static uint8_t DBCDRV_readHdrBuf[DBC_SPI_BUFFER_SIZE];   ///< Buffer for the SPI read command header data

#define DBCDRV_RESET_TIME_US                   (700u) ///< The time (in microseconds) after a reset event before the device is ready
#define DBCDRV_REG_SIZE                          (4u) ///< The number of bytes of DBusCAN chip registers
#define DBCDRV_DBUS_TXS_FIFO_SIZE               (64u) ///< The number of bytes to be allocated in DBusCAN's RAM for the DBus Tx Status FIFO buffer. 64 bytes is maximum, must be a multiple of 8 bytes.
#define DBCDRV_DBUS_TX_FIFO_SIZE               (512u) ///< The number of bytes to be allocated in DBusCAN's RAM for the DBus Tx FIFO buffer. Must be a multiple of 4 bytes.
#define DBCDRV_DBUS_RX_FIFO_SIZE               (512u) ///< The number of bytes to be allocated in DBusCAN's RAM for the DBus Rx FIFO buffer. Must be a multiple of 4 bytes.
#define DBCDRV_DBUS_BASE_RAM_ADDR                (0u) ///< Start (offset) address of DBusCAN's RAM space allocated for DBus buffers
#define DBCDRV_MOPC_STANDBY_MODE_MASK            ((uint32_t)DBC_POWER_MODE_STANDBY << DBC_MOPC_MODE_SEL_POS) ///< Mask for setting the standby mode in the MOPC register

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

static bool dbus_drv_extract_read_response(const uint8_t *rx_data,
                                           size_t len,
                                           uint8_t expected_addr_high,
                                           uint8_t expected_addr_low,
                                           uint8_t matched_data[4])
{
    for (size_t offset = 0u; (offset + 8u) <= len; offset++) {
        bool addr_high_match = ((rx_data[offset + 1u] & 0x7Fu) == (expected_addr_high & 0x7Fu));

        if (rx_data[offset] == DBCDRV_SPI_RSP_MARKER &&
            addr_high_match &&
            rx_data[offset + 2u] == expected_addr_low &&
            rx_data[offset + 3u] == 0x01u) {
            matched_data[0] = rx_data[offset + 4u];
            matched_data[1] = rx_data[offset + 5u];
            matched_data[2] = rx_data[offset + 6u];
            matched_data[3] = rx_data[offset + 7u];
            return true;
        }

        if (addr_high_match &&
            rx_data[offset + 2u] == expected_addr_low &&
            rx_data[offset + 3u] == 0x01u) {
            matched_data[0] = rx_data[offset + 4u];
            matched_data[1] = rx_data[offset + 5u];
            matched_data[2] = rx_data[offset + 6u];
            matched_data[3] = rx_data[offset + 7u];
            return true;
        }
    }

    for (size_t offset = 0u; (offset + 9u) <= len; offset++) {
        if (((rx_data[offset + 1u] & 0x7Fu) == (DBCDRV_SPI_RSP_MARKER & 0x7Fu)) &&
            ((rx_data[offset + 2u] & 0x7Fu) == (expected_addr_high & 0x7Fu)) &&
            rx_data[offset + 3u] == expected_addr_low &&
            rx_data[offset + 4u] == 0x01u) {
            matched_data[0] = rx_data[offset + 5u];
            matched_data[1] = rx_data[offset + 6u];
            matched_data[2] = rx_data[offset + 7u];
            matched_data[3] = rx_data[offset + 8u];
            return true;
        }
    }

    return false;
}

static inline uint8_t dbus_drv_rol1(uint8_t value)
{
    return (uint8_t)((value << 1u) | (value >> 7u));
}

static inline uint8_t dbus_drv_ror1(uint8_t value)
{
    return (uint8_t)((value >> 1u) | (value << 7u));
}

static bool dbus_drv_is_expected_read_response(const uint8_t *frame,
                                               uint8_t expected_addr_high,
                                               uint8_t expected_addr_low)
{
    return frame[0] == DBCDRV_SPI_RSP_MARKER &&
           ((frame[1] & 0x7Fu) == (expected_addr_high & 0x7Fu)) &&
           frame[2] == expected_addr_low &&
           frame[3] == 0x01u;
}

static bool dbus_drv_try_decoded_response(const uint8_t *frame,
                                          uint8_t expected_addr_high,
                                          uint8_t expected_addr_low,
                                          uint8_t matched_data[4])
{
    if (!dbus_drv_is_expected_read_response(frame, expected_addr_high, expected_addr_low)) {
        return false;
    }

    matched_data[0] = frame[4];
    matched_data[1] = frame[5];
    matched_data[2] = frame[6];
    matched_data[3] = frame[7];
    return true;
}

static bool dbus_drv_extract_read_response_autodecode(const uint8_t *rx_data,
                                                      size_t len,
                                                      uint8_t expected_addr_high,
                                                      uint8_t expected_addr_low,
                                                      uint8_t matched_data[4])
{
    uint8_t rotated[DBC_SPI_HDR_SIZE + sizeof(uint32_t)] = {0};
    uint8_t decoded[DBC_SPI_HDR_SIZE + sizeof(uint32_t)] = {0};
    const size_t frame_len = DBC_SPI_HDR_SIZE + sizeof(uint32_t);

    if (len < frame_len) {
        return false;
    }

    for (size_t offset = 0u; (offset + frame_len) <= len; offset++) {
        const uint8_t *window = rx_data + offset;

        if (dbus_drv_try_decoded_response(window,
                                          expected_addr_high,
                                          expected_addr_low,
                                          matched_data)) {
            return true;
        }

        for (size_t rotation = 0u; rotation < frame_len; rotation++) {
            for (size_t i = 0u; i < frame_len; i++) {
                rotated[i] = window[(i + rotation) % frame_len];
            }

            for (size_t i = 0u; i < frame_len; i++) {
                decoded[i] = dbus_drv_rol1(rotated[i]);
            }
            if (dbus_drv_try_decoded_response(decoded,
                                              expected_addr_high,
                                              expected_addr_low,
                                              matched_data)) {
                return true;
            }

            for (size_t i = 0u; i < frame_len; i++) {
                decoded[i] = dbus_drv_ror1(rotated[i]);
            }
            if (dbus_drv_try_decoded_response(decoded,
                                              expected_addr_high,
                                              expected_addr_low,
                                              matched_data)) {
                return true;
            }

            decoded[0] = (uint8_t)(rotated[0] >> 1u);
            for (size_t i = 1u; i < frame_len; i++) {
                decoded[i] = (uint8_t)((rotated[i] >> 1u) |
                                       ((rotated[i - 1u] & 0x01u) << 7u));
            }
            if (dbus_drv_try_decoded_response(decoded,
                                              expected_addr_high,
                                              expected_addr_low,
                                              matched_data)) {
                return true;
            }

            for (size_t i = 0u; i < (frame_len - 1u); i++) {
                decoded[i] = (uint8_t)((rotated[i] << 1u) | (rotated[i + 1u] >> 7u));
            }
            decoded[frame_len - 1u] = (uint8_t)(rotated[frame_len - 1u] << 1u);
            if (dbus_drv_try_decoded_response(decoded,
                                              expected_addr_high,
                                              expected_addr_low,
                                              matched_data)) {
                return true;
            }
        }
    }

    return false;
}

static void dbus_drv_decode_serial_ror1_stream(const uint8_t *raw_data,
                                               uint8_t *decoded_data,
                                               size_t len)
{
    if (len == 0u) {
        return;
    }

    decoded_data[0] = (uint8_t)(raw_data[0] >> 1u);
    for (size_t i = 1u; i < len; i++) {
        decoded_data[i] = (uint8_t)((raw_data[i] >> 1u) |
                                    ((raw_data[i - 1u] & 0x01u) << 7u));
    }
}

static bool dbus_drv_extract_read_response_bitshifted(const uint8_t *rx_data,
                                                      size_t len,
                                                      uint8_t expected_addr_high,
                                                      uint8_t expected_addr_low,
                                                      uint8_t matched_data[4])
{
    uint8_t shifted[(DBCDRV_SPI_DUMMY_RETRIES + 1u) * (DBC_SPI_HDR_SIZE + sizeof(uint32_t))] = {0};
    const size_t frame_len = DBC_SPI_HDR_SIZE + sizeof(uint32_t);

    if (len < (frame_len + 1u)) {
        return false;
    }

    for (uint8_t shift = 1u; shift < 8u; shift++) {
        size_t shifted_len = len;

        shifted[0] = (uint8_t)(rx_data[0] >> shift);
        for (size_t i = 1u; i < len; i++) {
            shifted[i] = (uint8_t)((rx_data[i] >> shift) |
                                   (rx_data[i - 1u] << (8u - shift)));
        }

        if (dbus_drv_extract_read_response(shifted,
                                           shifted_len,
                                           expected_addr_high,
                                           expected_addr_low,
                                           matched_data)) {
            return true;
        }

        if (dbus_drv_extract_read_response_autodecode(shifted,
                                                      shifted_len,
                                                      expected_addr_high,
                                                      expected_addr_low,
                                                      matched_data)) {
            return true;
        }

        shifted_len = len - 1u;
        for (size_t i = 0u; i < shifted_len; i++) {
            shifted[i] = (uint8_t)((rx_data[i] << shift) |
                                   (rx_data[i + 1u] >> (8u - shift)));
        }

        if (dbus_drv_extract_read_response(shifted,
                                           shifted_len,
                                           expected_addr_high,
                                           expected_addr_low,
                                           matched_data)) {
            return true;
        }

        if (dbus_drv_extract_read_response_autodecode(shifted,
                                                      shifted_len,
                                                      expected_addr_high,
                                                      expected_addr_low,
                                                      matched_data)) {
            return true;
        }
    }

    return false;
}

static bool dbus_drv_extract_read_response_interleaved_byte_lanes(const uint8_t *rx_data,
                                                                  size_t len,
                                                                  size_t frame_len,
                                                                  uint8_t expected_addr_high,
                                                                  uint8_t expected_addr_low,
                                                                  uint8_t matched_data[4])
{
    uint8_t lane_stream[DBCDRV_SPI_DUMMY_RETRIES + 1u] = {0};
    size_t frame_count;

    if (frame_len == 0u || len < frame_len) {
        return false;
    }

    frame_count = len / frame_len;
    if (frame_count < (DBC_SPI_HDR_SIZE + sizeof(uint32_t))) {
        return false;
    }

    for (size_t lane = 0u; lane < frame_len; lane++) {
        for (size_t frame = 0u; frame < frame_count; frame++) {
            lane_stream[frame] = rx_data[(frame * frame_len) + lane];
        }

        if (dbus_drv_extract_read_response(lane_stream,
                                           frame_count,
                                           expected_addr_high,
                                           expected_addr_low,
                                           matched_data)) {
            return true;
        }

        if (dbus_drv_extract_read_response_autodecode(lane_stream,
                                                      frame_count,
                                                      expected_addr_high,
                                                      expected_addr_low,
                                                      matched_data)) {
            return true;
        }

        if (dbus_drv_extract_read_response_bitshifted(lane_stream,
                                                      frame_count,
                                                      expected_addr_high,
                                                      expected_addr_low,
                                                      matched_data)) {
            return true;
        }
    }

    return false;
}

static bool dbus_drv_extract_read_response_interleaved_diagonals(const uint8_t *rx_data,
                                                                 size_t len,
                                                                 size_t frame_len,
                                                                 uint8_t expected_addr_high,
                                                                 uint8_t expected_addr_low,
                                                                 uint8_t matched_data[4])
{
    uint8_t diag_stream[DBCDRV_SPI_DUMMY_RETRIES + 1u] = {0};
    size_t frame_count;

    if (frame_len == 0u || len < frame_len) {
        return false;
    }

    frame_count = len / frame_len;
    if (frame_count < (DBC_SPI_HDR_SIZE + sizeof(uint32_t))) {
        return false;
    }

    for (int lane_delta = -1; lane_delta <= 1; lane_delta += 2) {
        for (size_t start_frame = 0u; start_frame < frame_count; start_frame++) {
            for (size_t start_lane = 0u; start_lane < frame_len; start_lane++) {
                size_t diag_len = 0u;
                int lane = (int)start_lane;

                for (size_t frame = start_frame; frame < frame_count; frame++) {
                    if (lane < 0 || lane >= (int)frame_len) {
                        break;
                    }

                    diag_stream[diag_len++] = rx_data[(frame * frame_len) + (size_t)lane];
                    lane += lane_delta;
                }

                if (diag_len < (DBC_SPI_HDR_SIZE + sizeof(uint32_t))) {
                    continue;
                }

                if (dbus_drv_extract_read_response(diag_stream,
                                                   diag_len,
                                                   expected_addr_high,
                                                   expected_addr_low,
                                                   matched_data)) {
                    return true;
                }

                if (dbus_drv_extract_read_response_autodecode(diag_stream,
                                                              diag_len,
                                                              expected_addr_high,
                                                              expected_addr_low,
                                                              matched_data)) {
                    return true;
                }

                if (dbus_drv_extract_read_response_bitshifted(diag_stream,
                                                              diag_len,
                                                              expected_addr_high,
                                                              expected_addr_low,
                                                              matched_data)) {
                    return true;
                }
            }
        }
    }

    return false;
}

static void dbus_drv_log_interleaved_lane_tails(const uint8_t *rx_data,
                                                size_t len,
                                                size_t frame_len)
{
#if !DBCDRV_LOG_FAIL_LANE_TAILS
    (void)rx_data;
    (void)len;
    (void)frame_len;
    return;
#else
    uint8_t lane_stream[DBCDRV_SPI_DUMMY_RETRIES + 1u] = {0};
    uint8_t decoded_stream[DBCDRV_SPI_DUMMY_RETRIES + 1u] = {0};
    size_t frame_count;
    size_t tail_len;
    size_t tail_offset;

    if (frame_len == 0u || len < frame_len) {
        return;
    }

    frame_count = len / frame_len;
    if (frame_count == 0u) {
        return;
    }

    tail_len = frame_count;
    if (tail_len > DBCDRV_FAIL_DIAG_LANE_TAIL_FRAMES) {
        tail_len = DBCDRV_FAIL_DIAG_LANE_TAIL_FRAMES;
    }
    tail_offset = frame_count - tail_len;

    for (size_t lane = 0u; lane < frame_len; lane++) {
        bool interesting = false;

        for (size_t frame = 0u; frame < frame_count; frame++) {
            lane_stream[frame] = rx_data[(frame * frame_len) + lane];
        }
        dbus_drv_decode_serial_ror1_stream(lane_stream, decoded_stream, frame_count);

        for (size_t i = tail_offset; i < frame_count; i++) {
            if ((lane_stream[i] != 0x00u && lane_stream[i] != 0xFFu && lane_stream[i] != 0xA5u) ||
                (decoded_stream[i] != 0x00u && decoded_stream[i] != 0xFFu && decoded_stream[i] != 0x7Fu)) {
                interesting = true;
                break;
            }
        }

        if (!interesting) {
            continue;
        }

        LOG_INF("DBCDRV_readReg32 fail lane=%u frame_tail=%u", (unsigned)lane, (unsigned)tail_len);
        LOG_HEXDUMP_INF(&lane_stream[tail_offset], tail_len, "DBCDRV_readReg32 fail lane raw:");
        LOG_HEXDUMP_INF(&decoded_stream[tail_offset], tail_len, "DBCDRV_readReg32 fail lane decoded:");
    }
#endif
}

static void dbus_drv_log_expected_byte_hits(const uint8_t *rx_data,
                                            size_t len,
                                            size_t frame_len,
                                            uint8_t expected_addr_high,
                                            uint8_t expected_addr_low)
{
    size_t frame_count;
    uint32_t marker_hits = 0u;
    uint32_t addr_high_hits = 0u;
    uint32_t addr_low_hits = 0u;
    uint32_t len_word_hits = 0u;
    uint32_t aligned_header_frames = 0u;
    uint32_t window_header_hits = 0u;
    int first_window_frame = -1;
    int first_window_offset = -1;

    if (frame_len == 0u || len < frame_len) {
        return;
    }

    frame_count = len / frame_len;
    if (frame_count == 0u) {
        return;
    }

    for (size_t frame = 0u; frame < frame_count; frame++) {
        const uint8_t *frame_ptr = &rx_data[frame * frame_len];

        for (size_t byte = 0u; byte < frame_len; byte++) {
            uint8_t value = frame_ptr[byte];

            if (value == DBCDRV_SPI_RSP_MARKER) {
                marker_hits++;
            }
            if (value == expected_addr_high) {
                addr_high_hits++;
            }
            if (value == expected_addr_low) {
                addr_low_hits++;
            }
            if (value == 0x01u) {
                len_word_hits++;
            }
        }

        if ((frame_len >= 4u) &&
            (frame_ptr[0] == DBCDRV_SPI_RSP_MARKER) &&
            (frame_ptr[1] == expected_addr_high) &&
            (frame_ptr[2] == expected_addr_low) &&
            (frame_ptr[3] == 0x01u)) {
            aligned_header_frames++;
        }

        for (size_t offset = 0u; (offset + 3u) < frame_len; offset++) {
            if ((frame_ptr[offset] == DBCDRV_SPI_RSP_MARKER) &&
                (frame_ptr[offset + 1u] == expected_addr_high) &&
                (frame_ptr[offset + 2u] == expected_addr_low) &&
                (frame_ptr[offset + 3u] == 0x01u)) {
                window_header_hits++;
                if (first_window_frame < 0) {
                    first_window_frame = (int)frame;
                    first_window_offset = (int)offset;
                }
            }
        }
    }

    if ((marker_hits > DBCDRV_FAIL_DIAG_MAX_BYTE_HITS) ||
        (addr_high_hits > DBCDRV_FAIL_DIAG_MAX_BYTE_HITS) ||
        (addr_low_hits > DBCDRV_FAIL_DIAG_MAX_BYTE_HITS) ||
        (len_word_hits > DBCDRV_FAIL_DIAG_MAX_BYTE_HITS)) {
        LOG_INF("DBCDRV_readReg32 fail header-scan: addr=0x%02x%02x frames=%u a0=%u ah=%u al=%u len1=%u aligned=%u windows=%u first=%d:%d",
                expected_addr_high,
                expected_addr_low,
                (unsigned)frame_count,
                (unsigned)marker_hits,
                (unsigned)addr_high_hits,
                (unsigned)addr_low_hits,
                (unsigned)len_word_hits,
                (unsigned)aligned_header_frames,
                (unsigned)window_header_hits,
                first_window_frame,
                first_window_offset);
        return;
    }

    LOG_INF("DBCDRV_readReg32 fail header-scan: addr=0x%02x%02x frames=%u a0=%u ah=%u al=%u len1=%u aligned=%u windows=%u first=%d:%d",
            expected_addr_high,
            expected_addr_low,
            (unsigned)frame_count,
            (unsigned)marker_hits,
            (unsigned)addr_high_hits,
            (unsigned)addr_low_hits,
            (unsigned)len_word_hits,
            (unsigned)aligned_header_frames,
            (unsigned)window_header_hits,
            first_window_frame,
            first_window_offset);
}

static uint8_t dbus_drv_score_provisional_payload(const uint8_t matched_data[4])
{
    uint8_t score = 0u;
    for (size_t i = 0u; i < 4u; i++) {
        if (matched_data[i] != 0xFFu) {
            score++;
        }
    }
    return score;
}

static int dbus_drv_spi_transceive_bytewise(const uint8_t *tx_data,
                                            uint8_t *rx_data,
                                            size_t len)
{
    struct spi_buf tx_buf = {
        .buf = (void *)tx_data,
        .len = len,
    };
    struct spi_buf rx_buf = {
        .buf = rx_data,
        .len = len,
    };
    struct spi_buf_set tx_bufs = {
        .buffers = &tx_buf,
        .count = 1,
    };
    struct spi_buf_set rx_bufs = {
        .buffers = &rx_buf,
        .count = 1,
    };

    return spi_transceive(dbus_spi_bus, &dbus_spi_cfg, &tx_bufs, &rx_bufs);
}

void DBCDRV_setSpiMode(bool cpol, bool cpha)
{
    uint16_t operation = dbus_spi_cfg.operation;

    operation &= ~(SPI_MODE_CPOL | SPI_MODE_CPHA);
    if (cpol) {
        operation |= SPI_MODE_CPOL;
    }
    if (cpha) {
        operation |= SPI_MODE_CPHA;
    }

    dbus_spi_cfg.operation = operation;
    LOG_DBG("DBCDRV_setSpiMode: CPOL=%u CPHA=%u operation=0x%04x",
            cpol,
            cpha,
            dbus_spi_cfg.operation);
}

uint16_t DBCDRV_getSpiMode(void)
{
    return (uint16_t)(dbus_spi_cfg.operation & (SPI_MODE_CPOL | SPI_MODE_CPHA));
}

enum DBC_Error DBCDRV_setSpiTarget(enum DBCDRV_SpiTarget target)
{
    if (target >= DBCDRV_SPI_TARGET_COUNT) {
        LOG_ERR("DBCDRV_setSpiTarget: invalid target %d", (int)target);
        return DBC_ERROR;
    }

    dbus_spi_target = target;
    dbus_spi_cfg.slave = 0u;

    if (device_is_ready(dbus_cs_gpio_dev)) {
        int ret = dbus_drv_set_cs_state(DBCDRV_SPI_TARGET_SECONDARY_PICO, false);

        if (ret < 0) {
            LOG_ERR("DBCDRV_setSpiTarget: failed to release secondary CS: %d", ret);
            return DBC_ERROR;
        }
    }

    LOG_INF("DBCDRV SPI target set to %s Pico on GPIO%u",
            dbus_drv_get_target_name(dbus_spi_target),
            dbus_drv_get_cs_pin(dbus_spi_target));
    return DBC_OK;
}

enum DBCDRV_SpiTarget DBCDRV_getSpiTarget(void)
{
    return dbus_spi_target;
}

void DBCDRV_logSpiRouting(const char *tag)
{
    int cs_secondary = -1;

    if (!device_is_ready(dbus_cs_gpio_dev)) {
        printk("RW612 SPI ROUTE[%s]: CS GPIO not ready\n", (tag != NULL) ? tag : "-");
        return;
    }

    cs_secondary = gpio_pin_get(dbus_cs_gpio_dev, DBUS_CS_GPIO_SECONDARY_PIN);
    printk("RW612 SPI ROUTE[%s]: target=%s gpio%u=hw_ssel0 gpio%u=%d\n",
           (tag != NULL) ? tag : "-",
           dbus_drv_get_target_name(dbus_spi_target),
           (unsigned)DBUS_CS_GPIO_PRIMARY_PIN,
           (unsigned)DBUS_CS_GPIO_SECONDARY_PIN,
           cs_secondary);
}

enum DBC_Error DBCDRV_pulseCs(enum DBCDRV_SpiTarget target,
                              uint32_t pulse_count,
                              uint32_t low_time_us,
                              uint32_t high_time_us)
{
    if (!device_is_ready(dbus_cs_gpio_dev)) {
        LOG_ERR("DBCDRV_pulseCs: CS GPIO device not ready");
        return DBC_ERROR;
    }

    for (uint32_t pulse = 0u; pulse < pulse_count; pulse++) {
        if (dbus_drv_set_cs_state(target, true) < 0) {
            LOG_ERR("DBCDRV_pulseCs: failed to assert CS for %s Pico",
                    dbus_drv_get_target_name(target));
            return DBC_ERROR;
        }
        k_usleep(low_time_us);

        if (dbus_drv_set_cs_state(target, false) < 0) {
            LOG_ERR("DBCDRV_pulseCs: failed to deassert CS for %s Pico",
                    dbus_drv_get_target_name(target));
            return DBC_ERROR;
        }
        k_usleep(high_time_us);
    }

    return DBC_OK;
}

enum DBC_Error DBCDRV_writeReg32(enum DBC_RegAddr addr, uint32_t data)
{
    uint8_t tx_buffer[DBC_SPI_HDR_SIZE + sizeof(uint32_t)] = {0};
    uint8_t rx_buffer[DBC_SPI_HDR_SIZE + sizeof(uint32_t)] = {0};
    bool speed_write = (((uint16_t)addr == 0x5014u) || ((uint16_t)addr == 0x5034u));
    int ret;

    if (!device_is_ready(dbus_cs_gpio_dev)) {
        LOG_ERR("DBCDRV_writeReg32: CS GPIO device not ready");
        return DBC_ERROR;
    }

    DBCDRV_setSpiFrameHdr(addr, sizeof(uint32_t), DBC_CMD_WRITE, tx_buffer);
    tx_buffer[DBC_SPI_HDR_SIZE] = (uint8_t)data;
    tx_buffer[DBC_SPI_HDR_SIZE + 1u] = (uint8_t)(data >> 8);
    tx_buffer[DBC_SPI_HDR_SIZE + 2u] = (uint8_t)(data >> 16);
    tx_buffer[DBC_SPI_HDR_SIZE + 3u] = (uint8_t)(data >> 24);

    if (dbus_drv_set_cs_state(dbus_spi_target, true) < 0) {
        LOG_ERR("DBCDRV_writeReg32: failed to assert CS for %s Pico",
                dbus_drv_get_target_name(dbus_spi_target));
        return DBC_ERROR;
    }
    k_usleep(DBCDRV_SPI_CS_SETUP_US);

    ret = dbus_drv_spi_transceive_bytewise(tx_buffer, rx_buffer, sizeof(tx_buffer));

    k_usleep(DBCDRV_SPI_CS_HOLD_US);
    if (dbus_drv_set_cs_state(dbus_spi_target, false) < 0) {
        LOG_ERR("DBCDRV_writeReg32: failed to deassert CS for %s Pico",
                dbus_drv_get_target_name(dbus_spi_target));
        return DBC_ERROR;
    }

    if (ret < 0) {
        LOG_ERR("DBCDRV_writeReg32: SPI transceive failed: %d for addr 0x%x, data 0x%x",
                ret,
                addr,
                data);
        return DBC_ERROR;
    }

    if (speed_write) {
        dbus_drv_speed_write_seq++;
        LOG_INF("DBCDRV_writeReg32 speed seq=%lu target=%s addr=0x%04x data=0x%08x tx=%02x %02x %02x %02x %02x %02x %02x %02x rx=%02x %02x %02x %02x %02x %02x %02x %02x",
                (unsigned long)dbus_drv_speed_write_seq,
                dbus_drv_get_target_name(dbus_spi_target),
                (unsigned)addr,
                (unsigned)data,
                tx_buffer[0], tx_buffer[1], tx_buffer[2], tx_buffer[3],
                tx_buffer[4], tx_buffer[5], tx_buffer[6], tx_buffer[7],
                rx_buffer[0], rx_buffer[1], rx_buffer[2], rx_buffer[3],
                rx_buffer[4], rx_buffer[5], rx_buffer[6], rx_buffer[7]);
    }

    if (DBCDRV_LOG_WRITE_HEXDUMPS) {
        LOG_HEXDUMP_INF(tx_buffer, sizeof(tx_buffer), "DBCDRV_writeReg32 TX:");
        LOG_HEXDUMP_INF(rx_buffer, sizeof(rx_buffer), "DBCDRV_writeReg32 RX:");
    }

    return DBC_OK;
}

enum DBC_Error DBCDRV_readReg32(enum DBC_RegAddr addr, uint32_t *data)
{
    const size_t frame_len = DBC_SPI_HDR_SIZE + sizeof(uint32_t);
    bool sonic_fast_path = ((uint16_t)addr >= 0x5100u) && ((uint16_t)addr < 0x5104u);
    bool log_secondary_read = (dbus_spi_target == DBCDRV_SPI_TARGET_SECONDARY_PICO) &&
                              (((uint16_t)addr == 0x5010u) ||
                               ((uint16_t)addr == 0x5014u) ||
                               ((uint16_t)addr == 0x5030u) ||
                               ((uint16_t)addr == 0x5034u));
    bool defer_secondary_direct_match = log_secondary_read;
    uint32_t cs_hold_us = sonic_fast_path ? 500u : DBCDRV_SPI_CS_HOLD_US;
    uint8_t dummy_retries = sonic_fast_path ? 12u : DBCDRV_SPI_DUMMY_RETRIES;
    uint32_t dummy_gap_us = sonic_fast_path ? 100u : DBCDRV_SPI_DUMMY_GAP_US;
    uint8_t tx_buffer[DBC_SPI_HDR_SIZE + sizeof(uint32_t)] = {0};
    uint8_t rx_buffer[DBC_SPI_HDR_SIZE + sizeof(uint32_t)] = {0};
    uint8_t dummy_tx[DBC_SPI_HDR_SIZE + sizeof(uint32_t)] = {0};
    uint8_t data_rx[DBC_SPI_HDR_SIZE + sizeof(uint32_t)] = {0};
    uint8_t cumulative_rx[(DBCDRV_SPI_DUMMY_RETRIES + 1u) * (DBC_SPI_HDR_SIZE + sizeof(uint32_t))] = {0};
    uint8_t decoded_tail[(DBCDRV_SPI_DUMMY_RETRIES + 1u) * (DBC_SPI_HDR_SIZE + sizeof(uint32_t))] = {0};
    uint8_t matched_data[4] = {0};
    uint8_t provisional_data[4] = {0};
    uint8_t interleaved_candidate[4] = {0};
    uint8_t interleaved_candidate_score = 0u;
    uint8_t expected_addr_high = (uint8_t)((uint16_t)addr >> 8);
    uint8_t expected_addr_low = (uint8_t)addr;
    size_t cumulative_len = 0u;
    int ret;
    bool have_match = false;
    bool have_interleaved_candidate = false;

    if (data == NULL) {
        return DBC_ERROR;
    }

    if (!device_is_ready(dbus_cs_gpio_dev)) {
        LOG_ERR("DBCDRV_readReg32: CS GPIO device not ready");
        return DBC_ERROR;
    }

    DBCDRV_setSpiFrameHdr(addr, sizeof(uint32_t), DBC_CMD_READ, tx_buffer);

    if (dbus_drv_set_cs_state(dbus_spi_target, true) < 0) {
        LOG_ERR("DBCDRV_readReg32: failed to assert CS for %s Pico",
                dbus_drv_get_target_name(dbus_spi_target));
        return DBC_ERROR;
    }
    k_usleep(DBCDRV_SPI_CS_SETUP_US);

    ret = dbus_drv_spi_transceive_bytewise(tx_buffer, rx_buffer, sizeof(tx_buffer));

    k_usleep(cs_hold_us);
    if (dbus_drv_set_cs_state(dbus_spi_target, false) < 0) {
        LOG_ERR("DBCDRV_readReg32: failed to deassert CS for %s Pico",
                dbus_drv_get_target_name(dbus_spi_target));
        return DBC_ERROR;
    }

    if (ret < 0) {
        LOG_ERR("DBCDRV_readReg32: cmd exchange failed: %d for addr 0x%x", ret, addr);
        return DBC_ERROR;
    }

    if (log_secondary_read) {
        LOG_INF("DBCDRV_readReg32 cmd target=%s addr=0x%04x tx=%02x %02x %02x %02x %02x %02x %02x %02x rx=%02x %02x %02x %02x %02x %02x %02x %02x",
                dbus_drv_get_target_name(dbus_spi_target),
                (unsigned)addr,
                tx_buffer[0], tx_buffer[1], tx_buffer[2], tx_buffer[3],
                tx_buffer[4], tx_buffer[5], tx_buffer[6], tx_buffer[7],
                rx_buffer[0], rx_buffer[1], rx_buffer[2], rx_buffer[3],
                rx_buffer[4], rx_buffer[5], rx_buffer[6], rx_buffer[7]);
    }

    memcpy(cumulative_rx, rx_buffer, frame_len);
    cumulative_len = frame_len;

    for (uint8_t attempt = 1u; attempt <= dummy_retries; attempt++) {
        /* After the initial READ command, send pure dummy clocks so the Pico
         * can return the already-queued response frame without parsing a fresh
         * command on every retry. */
        memset(dummy_tx, 0, sizeof(dummy_tx));
        memset(data_rx, 0, sizeof(data_rx));

        if (dbus_drv_set_cs_state(dbus_spi_target, true) < 0) {
            LOG_ERR("DBCDRV_readReg32: failed to assert dummy CS for %s Pico",
                    dbus_drv_get_target_name(dbus_spi_target));
            return DBC_ERROR;
        }
        k_usleep(DBCDRV_SPI_CS_SETUP_US);

        ret = dbus_drv_spi_transceive_bytewise(dummy_tx, data_rx, sizeof(dummy_tx));

        k_usleep(cs_hold_us);
        if (dbus_drv_set_cs_state(dbus_spi_target, false) < 0) {
            LOG_ERR("DBCDRV_readReg32: failed to deassert dummy CS for %s Pico",
                    dbus_drv_get_target_name(dbus_spi_target));
            return DBC_ERROR;
        }

        if (ret < 0) {
            LOG_ERR("DBCDRV_readReg32: dummy exchange attempt %u failed: %d for addr 0x%x",
                    attempt,
                    ret,
                    addr);
            return DBC_ERROR;
        }

        if (log_secondary_read && (attempt <= 8u)) {
            LOG_INF("DBCDRV_readReg32 dummy target=%s addr=0x%04x attempt=%u tx=%02x %02x %02x %02x %02x %02x %02x %02x rx=%02x %02x %02x %02x %02x %02x %02x %02x",
                dbus_drv_get_target_name(dbus_spi_target),
                (unsigned)addr,
                (unsigned)attempt,
                dummy_tx[0], dummy_tx[1], dummy_tx[2], dummy_tx[3],
                dummy_tx[4], dummy_tx[5], dummy_tx[6], dummy_tx[7],
                data_rx[0], data_rx[1], data_rx[2], data_rx[3],
                data_rx[4], data_rx[5], data_rx[6], data_rx[7]);
        }

        memcpy(&cumulative_rx[cumulative_len], data_rx, frame_len);
        cumulative_len += frame_len;

        if (DBCDRV_LOG_READ_RETRY_HEXDUMPS) {
            LOG_HEXDUMP_INF(data_rx, sizeof(data_rx), "DBCDRV_readReg32 retry RX:");
        }

        if (!defer_secondary_direct_match || attempt >= 8u) {
            if (dbus_drv_extract_read_response(data_rx,
                                               sizeof(data_rx),
                                               expected_addr_high,
                                               expected_addr_low,
                                               matched_data) ||
                dbus_drv_extract_read_response_autodecode(data_rx,
                                                          sizeof(data_rx),
                                                          expected_addr_high,
                                                          expected_addr_low,
                                                          matched_data)) {
                have_match = true;
                break;
            }

            if (dbus_drv_extract_read_response(cumulative_rx,
                                               cumulative_len,
                                               expected_addr_high,
                                               expected_addr_low,
                                               matched_data) ||
                dbus_drv_extract_read_response_autodecode(cumulative_rx,
                                                          cumulative_len,
                                                          expected_addr_high,
                                                          expected_addr_low,
                                                          matched_data) ||
                dbus_drv_extract_read_response_bitshifted(cumulative_rx,
                                                          cumulative_len,
                                                          expected_addr_high,
                                                          expected_addr_low,
                                                          matched_data)) {
                have_match = true;
                break;
            }
        }

        if (dbus_drv_extract_read_response_interleaved_byte_lanes(cumulative_rx,
                                                                  cumulative_len,
                                                                  frame_len,
                                                                  expected_addr_high,
                                                                  expected_addr_low,
                                                                  provisional_data) ||
            dbus_drv_extract_read_response_interleaved_diagonals(cumulative_rx,
                                                                 cumulative_len,
                                                                 frame_len,
                                                                 expected_addr_high,
                                                                 expected_addr_low,
                                                                 provisional_data)) {
            uint8_t candidate_score = dbus_drv_score_provisional_payload(provisional_data);

            if (!have_interleaved_candidate || candidate_score >= interleaved_candidate_score) {
                memcpy(interleaved_candidate, provisional_data, sizeof(interleaved_candidate));
                interleaved_candidate_score = candidate_score;
                have_interleaved_candidate = true;
            }

        #if DBCDRV_LOG_PROVISIONAL_INTERLEAVED
                LOG_INF("DBCDRV_readReg32 provisional interleaved match attempt=%u score=%u payload=%02x %02x %02x %02x",
                    (unsigned)attempt,
                    (unsigned)candidate_score,
                    provisional_data[0],
                    provisional_data[1],
                    provisional_data[2],
                    provisional_data[3]);
        #endif
        }

        k_usleep(dummy_gap_us);
    }

    if (!have_match && have_interleaved_candidate) {
        memcpy(matched_data, interleaved_candidate, sizeof(matched_data));
        have_match = true;
    #if DBCDRV_LOG_PROVISIONAL_INTERLEAVED
        LOG_INF("DBCDRV_readReg32 using best provisional interleaved payload score=%u",
            (unsigned)interleaved_candidate_score);
    #endif
    }

    if (!have_match) {
        size_t tail_len = cumulative_len;
        size_t tail_offset;

        if (tail_len > DBCDRV_FAIL_DIAG_TAIL_BYTES) {
            tail_len = DBCDRV_FAIL_DIAG_TAIL_BYTES;
        }
        tail_offset = cumulative_len - tail_len;

        dbus_drv_decode_serial_ror1_stream(cumulative_rx, decoded_tail, cumulative_len);
        LOG_ERR("DBCDRV_readReg32 summary: addr=0x%x target=%s result=error attempts=%u",
                addr,
                dbus_drv_get_target_name(dbus_spi_target),
            (unsigned)dummy_retries);
#if DBCDRV_LOG_FAIL_HEXDUMPS
        LOG_HEXDUMP_INF(&cumulative_rx[tail_offset], tail_len, "DBCDRV_readReg32 fail tail raw:");
        LOG_HEXDUMP_INF(&decoded_tail[tail_offset], tail_len, "DBCDRV_readReg32 fail tail decoded:");
#endif
        dbus_drv_log_interleaved_lane_tails(cumulative_rx, cumulative_len, frame_len);
        dbus_drv_log_expected_byte_hits(cumulative_rx,
                                        cumulative_len,
                                        frame_len,
                                        expected_addr_high,
                                        expected_addr_low);
        return DBC_ERROR;
    }

    *data = ((uint32_t)matched_data[3] << 24) |
            ((uint32_t)matched_data[2] << 16) |
            ((uint32_t)matched_data[1] << 8) |
            (uint32_t)matched_data[0];

    if (!sonic_fast_path) {
        LOG_INF("DBCDRV_readReg32 summary: addr=0x%x target=%s result=ok value=0x%08x",
                addr,
                dbus_drv_get_target_name(dbus_spi_target),
                *data);
    }
    return DBC_OK;
}
