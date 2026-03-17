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

// Define the CS GPIO device and pin directly from the overlay
#define DBUS_CS_GPIO_NODE DT_NODELABEL(hsgpio0)
#define DBUS_CS_GPIO_PIN 6
#define DBUS_CS_GPIO_FLAGS GPIO_OUTPUT

static const struct device *dbus_cs_gpio_dev = DEVICE_DT_GET(DBUS_CS_GPIO_NODE);

static struct spi_config dbus_spi_cfg = {
    .frequency = 20000, // Slower clock for SPI slave timing bring-up
    .operation = SPI_OP_MODE_MASTER | SPI_WORD_SET(8) | SPI_TRANSFER_MSB,
    .slave = 0, // Assuming slave select 0
    .cs = NULL, // Disable Zephyr CS control, using manual GPIO
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
    LOG_DBG("MEXTI_init called for channel %u.", channel);

    if (!device_is_ready(mextid3_gpio_dev)) {
        printk("MEXTI_init: GPIO device not ready!\n");
        return MCAL_ERROR;
    }

    handle->channel = channel; // Store channel in handle

    // Initialize the gpio_callback structure
    gpio_init_callback(&handle->gpio_cb, zephyr_gpio_callback, BIT(MEXTID3_GPIO_PIN));

    // Add the callback to the GPIO pin
    int ret = gpio_add_callback(mextid3_gpio_dev, &handle->gpio_cb);
    if (ret < 0) {
        printk("MEXTI_init: Failed to add GPIO callback: %d\n", ret);
        return MCAL_ERROR;
    }

    // Configure the interrupt trigger type
    gpio_flags_t flags = GPIO_INPUT | GPIO_PULL_UP; // Default to input with pull-up
    if (config->trigger == MEXTI_TRIGGER_FALLING) {
        flags |= GPIO_INT_EDGE_FALLING;
    } else if (config->trigger == MEXTI_TRIGGER_RISING) {
        flags |= GPIO_INT_EDGE_RISING;
    } else {
        printk("MEXTI_init: Unsupported MEXTI trigger type.\n");
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
    gpio_pin_set(dbus_cs_gpio_dev, DBUS_CS_GPIO_PIN, 0);
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
    gpio_pin_set(dbus_cs_gpio_dev, DBUS_CS_GPIO_PIN, 1);
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

static int DBCDRV_spiTransceiveBytewise(const uint8_t *tx_data, uint8_t *rx_data, size_t len)
{
    for (size_t i = 0; i < len; i++) {
        uint8_t tx_byte = tx_data[i];
        uint8_t rx_byte = 0;

        struct spi_buf tx_spi_buf = {
            .buf = &tx_byte,
            .len = 1,
        };
        struct spi_buf_set tx_bufs = {
            .buffers = &tx_spi_buf,
            .count = 1,
        };

        struct spi_buf rx_spi_buf = {
            .buf = &rx_byte,
            .len = 1,
        };
        struct spi_buf_set rx_bufs = {
            .buffers = &rx_spi_buf,
            .count = 1,
        };

        int ret = spi_transceive(dbus_spi_bus, &dbus_spi_cfg, &tx_bufs, &rx_bufs);
        if (ret) {
            return ret;
        }

        rx_data[i] = rx_byte;
        k_usleep(25);
    }

    return 0;
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
    LOG_DBG("DBCDRV_setSpiMode: CPOL=%u CPHA=%u operation=0x%04x", cpol, cpha, dbus_spi_cfg.operation);
}

uint16_t DBCDRV_getSpiMode(void)
{
    return (dbus_spi_cfg.operation & (SPI_MODE_CPOL | SPI_MODE_CPHA));
}

// DBCDRV_readReg32 function
enum DBC_Error DBCDRV_readReg32(enum DBC_RegAddr addr, uint32_t *data)
{
    LOG_DBG("[FLASHCHK_V3_2026_03_17] Reading register 0x%x", addr);

    if (!device_is_ready(dbus_cs_gpio_dev)) {
        LOG_ERR("DBCDRV_readReg32: CS GPIO device not ready");
        return DBC_ERROR;
    }

    uint8_t tx_buffer[DBC_SPI_HDR_SIZE + sizeof(uint32_t)] = {0};
    uint8_t rx_buffer[DBC_SPI_HDR_SIZE + sizeof(uint32_t)] = {0};

    // Prepare the SPI header for a read command
    DBCDRV_setSpiFrameHdr(addr, sizeof(uint32_t), DBC_CMD_READ, tx_buffer);

    /* Exchange 1: send the read command. The Pico slave cannot prepare the
     * register value in time for this exchange — it only updates its TX FIFO
     * AFTER receiving all 8 bytes. The response bytes here are stale/default
     * and must be discarded. */
    gpio_pin_set(dbus_cs_gpio_dev, DBUS_CS_GPIO_PIN, 0);
    k_usleep(2);

    int ret = DBCDRV_spiTransceiveBytewise(tx_buffer, rx_buffer, sizeof(tx_buffer));

    k_usleep(2);
    gpio_pin_set(dbus_cs_gpio_dev, DBUS_CS_GPIO_PIN, 1);

    if (ret) {
        LOG_ERR("DBCDRV_readReg32: SPI transceive failed: %d for addr 0x%x", ret, addr);
        return DBC_ERROR;
    }
    LOG_DBG("DBCDRV_readReg32: cmd exchange for addr 0x%x", addr);
    LOG_HEXDUMP_DBG(tx_buffer, sizeof(tx_buffer), "DBCDRV_readReg32 TX:");
    LOG_HEXDUMP_DBG(rx_buffer, sizeof(rx_buffer), "DBCDRV_readReg32 RX (discarded):");

    /* Exchange 2: send dummy zeros. On the current Pico slave implementation,
     * this may still return stale/default bytes.
     */
    uint8_t dummy_tx[DBC_SPI_HDR_SIZE + sizeof(uint32_t)] = {0};
    uint8_t data_rx_stage1[DBC_SPI_HDR_SIZE + sizeof(uint32_t)] = {0};
    uint8_t data_rx[DBC_SPI_HDR_SIZE + sizeof(uint32_t)] = {0};

    gpio_pin_set(dbus_cs_gpio_dev, DBUS_CS_GPIO_PIN, 0);
    k_usleep(2);

    ret = DBCDRV_spiTransceiveBytewise(dummy_tx, data_rx_stage1, sizeof(dummy_tx));

    k_usleep(2);
    gpio_pin_set(dbus_cs_gpio_dev, DBUS_CS_GPIO_PIN, 1);

    if (ret) {
        LOG_ERR("DBCDRV_readReg32: dummy exchange stage1 failed: %d for addr 0x%x", ret, addr);
        return DBC_ERROR;
    }

    /* Exchange 3: send a second dummy frame and use this result as payload.
     * Empirically, Pico's prepared response appears one transaction later. */
    gpio_pin_set(dbus_cs_gpio_dev, DBUS_CS_GPIO_PIN, 0);
    k_usleep(2);

    ret = DBCDRV_spiTransceiveBytewise(dummy_tx, data_rx, sizeof(dummy_tx));

    k_usleep(2);
    gpio_pin_set(dbus_cs_gpio_dev, DBUS_CS_GPIO_PIN, 1);

    if (ret) {
        LOG_ERR("DBCDRV_readReg32: dummy exchange stage2 failed: %d for addr 0x%x", ret, addr);
        return DBC_ERROR;
    }

    LOG_DBG("DBCDRV_readReg32: SPI transceive successful for addr 0x%x. Read %u bytes.", addr, sizeof(data_rx));
    LOG_HEXDUMP_DBG(dummy_tx, sizeof(dummy_tx), "DBCDRV_readReg32 dummy TX stage1:");
    LOG_HEXDUMP_DBG(data_rx_stage1, sizeof(data_rx_stage1), "DBCDRV_readReg32 data RX stage1 (discarded):");
    LOG_HEXDUMP_DBG(dummy_tx, sizeof(dummy_tx), "DBCDRV_readReg32 dummy TX stage2:");
    LOG_HEXDUMP_DBG(data_rx, sizeof(data_rx), "DBCDRV_readReg32 data RX stage2:");

    *data = (uint32_t)data_rx[DBC_SPI_HDR_SIZE + 3] << 24 |
            (uint32_t)data_rx[DBC_SPI_HDR_SIZE + 2] << 16 |
            (uint32_t)data_rx[DBC_SPI_HDR_SIZE + 1] << 8 |
            (uint32_t)data_rx[DBC_SPI_HDR_SIZE];

    return DBC_OK;
}

// DBCDRV_writeReg32 function
enum DBC_Error DBCDRV_writeReg32(enum DBC_RegAddr addr, uint32_t data)
{
    LOG_DBG("[FLASHCHK_V3_2026_03_17] Writing 0x%x to register 0x%x", data, addr);

    if (!device_is_ready(dbus_cs_gpio_dev)) {
        LOG_ERR("DBCDRV_writeReg32: CS GPIO device not ready");
        return DBC_ERROR;
    }

    uint8_t tx_buffer[DBC_SPI_HDR_SIZE + sizeof(uint32_t)] = {0};
    uint8_t rx_buffer[DBC_SPI_HDR_SIZE + sizeof(uint32_t)] = {0}; // Required for spi_transceive, even if data is not used

    // Prepare the SPI header for a write command
    DBCDRV_setSpiFrameHdr(addr, sizeof(uint32_t), DBC_CMD_WRITE, tx_buffer);

    // Copy the 32-bit data into the transmit buffer after the header
    tx_buffer[DBC_SPI_HDR_SIZE]     = (uint8_t)(data);
    tx_buffer[DBC_SPI_HDR_SIZE + 1] = (uint8_t)(data >> 8);
    tx_buffer[DBC_SPI_HDR_SIZE + 2] = (uint8_t)(data >> 16);
    tx_buffer[DBC_SPI_HDR_SIZE + 3] = (uint8_t)(data >> 24);

    gpio_pin_set(dbus_cs_gpio_dev, DBUS_CS_GPIO_PIN, 0);
    k_usleep(2);

    int ret = DBCDRV_spiTransceiveBytewise(tx_buffer, rx_buffer, sizeof(tx_buffer));

    k_usleep(2);
    gpio_pin_set(dbus_cs_gpio_dev, DBUS_CS_GPIO_PIN, 1);

    if (ret) {
        LOG_ERR("DBCDRV_writeReg32: SPI transceive failed: %d for addr 0x%x, data 0x%x", ret, addr, data);
        return DBC_ERROR;
    }
    LOG_DBG("DBCDRV_writeReg32: SPI transceive successful for addr 0x%x, data 0x%x.", addr, data);
    LOG_HEXDUMP_DBG(tx_buffer, sizeof(tx_buffer), "DBCDRV_writeReg32 TX:");
    LOG_HEXDUMP_DBG(rx_buffer, sizeof(rx_buffer), "DBCDRV_writeReg32 RX:");

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
        printk("DBCDRV_sendSpiFrameNbl: SPI data length exceeds max allowed: %u\n", len);
        return DBC_ERROR;
    }

    // Prepare transmit buffer
    DBCDRV_setSpiFrameHdr(address, len, command, DBCDRV_spiBuff.array);
    if (txBuff != NULL && len > 0) {
        memcpy(&DBCDRV_spiBuff.array[DBC_SPI_HDR_SIZE], txBuff, len);
    }

#ifdef DBUSCAN_SPI_CRC_USED
    uint16_t crc = DBCDRV_calculateCrc(DBCDRV_spiBuff.array, DBC_SPI_HDR_SIZE + len);
    DBCDRV_spiBuff.array[DBC_SPI_HDR_SIZE + len] = (uint8_t)(crc >> BYTE_SIZE);
    DBCDRV_spiBuff.array[DBC_SPI_HDR_SIZE + len + 1] = (uint8_t)crc;
    uint32_t total_len = DBC_SPI_HDR_SIZE + len + DBC_SPI_CRC_SIZE;
#else
    uint32_t total_len = DBC_SPI_HDR_SIZE + len;
#endif

    // Perform non-blocking SPI transfer (currently mapped to blocking)
    enum MCAL_Error mcal_err = MSPI_transferDma(NULL, DBCDRV_spiBuff.array, total_len, DBCDRV_readHdrBuf, total_len);
    if (mcal_err != MCAL_OK) {
        printk("DBCDRV_sendSpiFrameNbl: SPI non-blocking transfer failed: %d\n", mcal_err);
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
        printk("DBCDRV_sendSpiFrameNbl: SPI CRC mismatch in NBL! Received: 0x%x, Calculated: 0x%x\n", received_crc, calculated_crc);
        // DBCDRV_spiCrcReadError = true; // Removed unused variable
        return DBC_ERROR;
    } else {
        // DBCDRV_spiCrcReadError = false; // Removed unused variable
    }
#endif

    return DBC_OK;
}

// DBCDRV_setPowerMode function
enum DBC_Error DBCDRV_setPowerMode(enum DBC_PowerMode mode)
{
    uint32_t regVal;
    DBC_RETURN_ON_ERROR(DBCDRV_readReg32(DBC_MOPC_ADDR, &regVal));
    LOG_DBG("DBCDRV_setPowerMode: Initial read of DBC_MOPC_ADDR (0x%x): 0x%x", DBC_MOPC_ADDR, regVal);
    uint32_t chipMode = (regVal & DBC_MOPC_MODE_SEL_MASK) >> DBC_MOPC_MODE_SEL_POS;
    const uint32_t INVALID_REG_VAL = ~0uL;
    if(regVal == INVALID_REG_VAL)
    {
        printk("DBCDRV_setPowerMode: Read INVALID_REG_VAL (0x%x) from DBC_MOPC_ADDR (0x%x)\n", INVALID_REG_VAL, DBC_MOPC_ADDR);
        return DBC_ERROR;
    }
    if (chipMode != (uint32_t)mode)
    {
        if ((DBC_POWER_MODE_NORMAL == mode) && (chipMode != (uint32_t)DBC_POWER_MODE_STANDBY))
        {   // changing to NORMAL mode is only possible from STANDBY mode
            LOG_DBG("DBCDRV_setPowerMode: Current mode is %u, target is NORMAL. Setting to STANDBY first.", chipMode);
            regVal &= ~DBC_MOPC_MODE_SEL_MASK;
            regVal |= DBCDRV_MOPC_STANDBY_MODE_MASK;
            DBC_RETURN_ON_ERROR(DBCDRV_writeReg32(DBC_MOPC_ADDR, regVal));
            LOG_DBG("DBCDRV_setPowerMode: Wrote 0x%x to DBC_MOPC_ADDR (0x%x) for STANDBY transition.", regVal, DBC_MOPC_ADDR);
        }
        regVal &= ~DBC_MOPC_MODE_SEL_MASK;
        regVal |= (uint32_t)mode << DBC_MOPC_MODE_SEL_POS;
        DBC_RETURN_ON_ERROR(DBCDRV_writeReg32(DBC_MOPC_ADDR, regVal));
        LOG_DBG("DBCDRV_setPowerMode: Wrote 0x%x to DBC_MOPC_ADDR (0x%x) for target mode %u.", regVal, DBC_MOPC_ADDR, mode);
        uint32_t read_back_regVal;
        DBC_RETURN_ON_ERROR(DBCDRV_readReg32(DBC_MOPC_ADDR, &read_back_regVal));
        LOG_DBG("DBCDRV_setPowerMode: Read back 0x%x from DBC_MOPC_ADDR (0x%x).", read_back_regVal, DBC_MOPC_ADDR);
        regVal &= DBC_MOPC_MODE_SEL_MASK;
        return ((read_back_regVal & DBC_MOPC_MODE_SEL_MASK) != regVal) ? DBC_ERROR : DBC_OK;
    }
    LOG_DBG("DBCDRV_setPowerMode: Chip already in desired mode %u.", mode);
    return DBC_OK;
}

// DBCDRV_setPowerModeStandby function
enum DBC_Error DBCDRV_setPowerModeStandby(void)
{
    uint32_t regVal;
    // The original code sends the command twice, once without CRC and once with CRC,
    // to ensure the chip enters standby mode correctly, especially if the CRC setting is unknown.
    // DBCDRV_writeReg32 handles CRC internally if enabled.
    DBC_RETURN_ON_ERROR(DBCDRV_writeReg32(DBC_MOPC_ADDR, DBCDRV_MOPC_STANDBY_MODE_MASK));
    DBC_RETURN_ON_ERROR(DBCDRV_writeReg32(DBC_MOPC_ADDR, DBCDRV_MOPC_STANDBY_MODE_MASK));

    // Verify the chip is in STANDBY mode
    DBC_RETURN_ON_ERROR(DBCDRV_readReg32(DBC_MOPC_ADDR, &regVal));
    if ((regVal & DBC_MOPC_MODE_SEL_MASK) != DBCDRV_MOPC_STANDBY_MODE_MASK)
    {
        printk("DBCDRV_setPowerModeStandby: Chip is not in STANDBY mode after setting.\n");
        return DBC_ERROR; // the chip is not in STANDBY mode
    }
    return DBC_OK;
}

// DBCDRV_reset function
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
        // DBCDRV_setSpiHdrSize(0u); // This function is not used in Zephyr port
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

// DBCDRV_doReset function
enum DBC_Error DBCDRV_doReset(bool *internalEepromError)
{
    uint32_t regVal;
    DBC_RETURN_ON_ERROR(DBCDRV_readReg32(DBC_IF_ADDR, &regVal));
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
    uint32_t dpc_reg_val;
    DBC_RETURN_ON_ERROR(DBCDRV_readReg32(DBC_DBUS_DPC_ADDR, &dpc_reg_val));
    bool isAdvPwrMgmt = (0u != (dpc_reg_val & DBC_DBUS_DPC_ADV_PWR_MGMT_MASK));
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

// DBCDRV_isAllFeatureRevision function
extern bool DBCDRV_isAllFeatureRevision(void)
{
    uint32_t revision_val;
    DBC_RETURN_ON_ERROR(DBCDRV_readReg32(DBC_REVISION_ADDR, &revision_val));
    return (revision_val >= DBC_REVISION_WITH_ALL_FEATURES) ? true : false;
}

// DBCDRV_enableAndClearIrqFlags function
enum DBC_Error DBCDRV_enableAndClearIrqFlags(uint32_t flags)
{
    LOG_DBG("DBCDRV_enableAndClearIrqFlags placeholder called. Flags: 0x%x", flags);
    uint32_t regVal;
    DBC_RETURN_ON_ERROR(DBCDRV_readReg32(DBC_IE_ADDR, &regVal));
    regVal &= ~DBC_IF_UVCC_MASK; // disable interrupt on UVCC event (as this flag cannot be cleared by SW)
    DBC_RETURN_ON_ERROR(DBCDRV_writeReg32(DBC_IE_ADDR, regVal));
    DBC_RETURN_ON_ERROR(DBCDRV_writeReg32(DBC_DBUS_IE_ADDR, (DBC_DBUS_IE_ALL_BIT_MASK & ~DBC_DBUS_IE_DBUSSLNT_EN_MASK))); //enable all DBus interrupts except DBus silent flag
    // Clear global interrupt and SPI status flags
    DBC_RETURN_ON_ERROR(DBCDRV_readReg32(DBC_IF_ADDR, &regVal));
    DBC_RETURN_ON_ERROR(DBCDRV_writeReg32(DBC_IF_ADDR, regVal));
    DBC_RETURN_ON_ERROR(DBCDRV_readReg32(DBC_STATUS_ADDR, &regVal));
    DBC_RETURN_ON_ERROR(DBCDRV_writeReg32(DBC_STATUS_ADDR, regVal));
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
    uint32_t regVal;
    // Configuration of DBus buffers in chip's RAM
    // DBCDRV_setSpiFrameHdr(DBC_DBUS_BSA_ADDR, (DBCDRV_REG_SIZE * 3u), DBC_WRITE_L, DBCDRV_spiBuff.array); // DBCDRV_REG_SIZE is not defined here
    // For now, directly write to registers
    DBC_RETURN_ON_ERROR(DBCDRV_writeReg32(DBC_DBUS_BSA_ADDR, DBCDRV_DBUS_BASE_RAM_ADDR)); // Dbus Base Address reg (0x4060) => set start address of DBusCAN's RAM allocated for DBus buffers
    DBC_RETURN_ON_ERROR(DBCDRV_writeReg32(DBC_DBUS_BSC0_ADDR, ((uint32_t)DBCDRV_DBUS_RX_FIFO_SIZE << DBC_DBUS_BSC0_RX_BUF_SIZE_POS) + DBCDRV_DBUS_TX_FIFO_SIZE)); // DBus Buffer Size Control 0 reg (0x4064) => set DBus Rx & Tx FIFO size
    DBC_RETURN_ON_ERROR(DBCDRV_writeReg32(DBC_DBUS_BSC1_ADDR, DBCDRV_DBUS_TXS_FIFO_SIZE)); // Dbus buff Size Control 1 reg (0x4068) => set DBus Tx Status FIFO size

    // Allow maximum number of DBus Node ID filters
    DBC_RETURN_ON_ERROR(DBCDRV_readReg32(DBC_DBUS_SIDFC_ADDR, &regVal));
    regVal |= DBC_DBUS_SIDFC_LSS_MASK;
    DBC_RETURN_ON_ERROR(DBCDRV_writeReg32(DBC_DBUS_SIDFC_ADDR, regVal));
    // Enable message reception for all subsystems by using node filter
    // Assuming APP_VARIANT behavior for now
    // DBCDBUS_setMultipleAddresses(); // This function is not defined yet
    DBC_RETURN_ON_ERROR(DBCDRV_writeReg32(DBC_DBUS_NF0_ADDR, DBC_DBUS_NF_ALL_SUBSYS_EN_MASK | ((uint32_t)config->NODE_ID << DBC_DBUS_NF_PID_POS)));

    return DBC_OK;
}

// DBCDRV_calculateCrc function
uint16_t DBCDRV_calculateCrc(uint8_t *data, uint32_t len)
{
    uint16_t crc = CRC_Ccitt_Init();
    for(; len != 0u; len--)
    {
        crc = CRC_Ccitt_UpdateByte(crc, *data);
        data++;
    }
    return (uint16_t)(crc << BYTE_SIZE) | (crc >> BYTE_SIZE);
}

// DBCDRV_enableSpiCrc function
enum DBC_Error DBCDRV_enableSpiCrc(void)
{
    bool isSpiCrcEnabled, isSpiError;
    const uint32_t SPI_CRC_CFG_MASK = ((uint32_t)DBC_SPI_CRC_POLY_CCITT | (uint32_t)DBC_SPI_CRC_SEED_0XFFFF | DBC_SPI_CRC_CFG_EN_MASK); // Mask for SPI CRC configuration - CRC-16/CCITT-FALSE
    uint32_t regVal;
    DBC_RETURN_ON_ERROR(DBCDRV_readReg32(DBC_SPI_CRC_CFG_ADDR, &regVal)); // read CRC configuration
    isSpiCrcEnabled = (0u != (regVal & DBC_SPI_CRC_CFG_EN_MASK)) ? true : false;
    DBC_RETURN_ON_ERROR(DBCDRV_readReg32(DBC_STATUS_ADDR, &regVal));
    isSpiError = (0u != (regVal & DBC_IF_SPIERR_MASK)) ? true : false;
    // DBCDRV_setSpiHdrSize((isSpiCrcEnabled) ? DBC_SPI_CRC_SIZE : 0u); // This function is not used in Zephyr port
    if(isSpiError)
    {
        if(isSpiCrcEnabled)
        {
            uint32_t crcSeedVal;
            DBC_RETURN_ON_ERROR(DBCDRV_readReg32(DBC_SPI_CRC_SEED_ADDR, &crcSeedVal));
            if((regVal != SPI_CRC_CFG_MASK) || (0u != crcSeedVal))
            {
                printk("DBCDRV_enableSpiCrc: Non-expected CRC configuration detected, returning error.\n");
                return DBC_ERROR; // For now, just return error
            }
        }
        DBC_RETURN_ON_ERROR(DBCDRV_writeReg32(DBC_STATUS_ADDR, regVal)); // clear SPI error flag - still returns DBC_ERROR
    }
    if(!isSpiCrcEnabled)
    {
        DBC_RETURN_ON_ERROR(DBCDRV_writeReg32(DBC_SPI_CRC_SEED_ADDR, 0u));              // set CRC seed value to 0
        DBC_RETURN_ON_ERROR(DBCDRV_writeReg32(DBC_SPI_CRC_CFG_ADDR, SPI_CRC_CFG_MASK)); // enable CRC
        // DBCDRV_setSpiHdrSize(DBC_SPI_CRC_SIZE); // This function is not used in Zephyr port
        DBC_RETURN_ON_ERROR(DBCDRV_readReg32(DBC_SPI_CRC_CFG_ADDR, &regVal)); // read CRC configuration
        // DBCDRV_spiCrcReadError = false; // This flag is not used in Zephyr port
        // DBCDRV_checkSpiCrcInReadOp(DBCDRV_spiBuff.array); // This function is not used in Zephyr port
        uint32_t crcSeedVal;
        DBC_RETURN_ON_ERROR(DBCDRV_readReg32(DBC_SPI_CRC_SEED_ADDR, &crcSeedVal));
        if((regVal != SPI_CRC_CFG_MASK) || (0u != crcSeedVal))
        {
            printk("DBCDRV_enableSpiCrc: CRC is not correctly enabled, returning error.\n");
            return DBC_ERROR;   // CRC is not correctly enabled
        }
    }
    return DBC_OK;
}

// DBCDRV_disableSpiCrc function
enum DBC_Error DBCDRV_disableSpiCrc(void)
{
    // DBCDRV_setSpiHdrSize(0u); // This function is not used in Zephyr port
    uint32_t regVal;
    DBC_RETURN_ON_ERROR(DBCDRV_writeReg32(DBC_SPI_CRC_CFG_ADDR, 0u)); // Disable CRC
    DBC_RETURN_ON_ERROR(DBCDRV_readReg32(DBC_SPI_CRC_CFG_ADDR, &regVal)); // read CRC configuration
    return (0u == (regVal & DBC_SPI_CRC_CFG_EN_MASK)) ? DBC_OK : DBC_ERROR; // check if CRC was disabled
}

// DBCDRV_dmaInit function
enum MCAL_Error DBCDRV_dmaInit(void)
{
    LOG_DBG("MDMA_init placeholder called.");
    return MCAL_OK;
}

// DBCDRV_dmaCbFunction function
void DBCDRV_dmaCbFunction(void *obj, uint32_t flags, const void *eventResponse)
{
    LOG_DBG("DBCDRV_dmaCbFunction placeholder called.");
}

enum DBC_Error DBCDRV_initComChannels(MCAL_CallbackFunction_t irqHandleCbFunc)
{
    /* Configuration of IO pin used for IRQ sensing from DBusCAN */
    const struct MDIO_Channel *intPin = MEXTI_getPin(DBCDRV_getMextiChannel());

    if (!device_is_ready(dbus_cs_gpio_dev)) {
        printk("DBCDRV_initComChannels: Error point N - CS GPIO device not ready!\n");
        return DBC_ERROR;
    }

    int ret = gpio_pin_configure(dbus_cs_gpio_dev, DBUS_CS_GPIO_PIN, DBUS_CS_GPIO_FLAGS);
    if (ret < 0) {
        printk("DBCDRV_initComChannels: Error point O - Failed to configure CS GPIO pin: %d\n", ret);
        return DBC_ERROR;
    }
    gpio_pin_set(dbus_cs_gpio_dev, DBUS_CS_GPIO_PIN, 1); // Ensure CS is de-asserted initially
    k_msleep(1); // Small delay after configuring CS

    if(MCAL_OK != MDIO_init(intPin, &MDIO_INPUT_PULL_UP))
    {
        printk("DBCDRV_initComChannels: Error point P - MDIO_init failed.\n");
        return DBC_ERROR;
    }

#ifdef APP_VARIANT
    const struct MEXTI_Config mextiCfg = {.trigger = MEXTI_TRIGGER_FALLING};
    if(MCAL_OK == MEXTI_init(&DBCDRV_mextiHandle, DBCDRV_getMextiChannel(), &mextiCfg))
    {
        MCAL_initCallback(&DBCDRV_cbIrqHandle, irqHandleCbFunc, (void*)&DBCDRV_mextiHandle);
        LOG_DBG("DBCDRV_init: Enabling MEXTI event.");
        DBC_RETURN_ON_ERROR(MEXTI_enableEvent(&DBCDRV_mextiHandle, &DBCDRV_cbIrqHandle));
        LOG_DBG("DBCDRV_init: MEXTI event enabled.");
    }
    else
    {
        printk("DBCDRV_initComChannels: Error point Q - MEXTI_init failed.\n");
        return DBC_ERROR;
    }
#else
    (void)irqHandleCbFunc; //lint !e920 Cast from pointer to void - callback function is not used in bootloader variants
    DBCDRV_mspiHandle.status = MCAL_STATUS_RESET; // set reset status explicitly (necessary for bootloader variants due to the uninitialized RAM)
#endif //APP_VARIANT

    if(MCAL_OK != MSPI_init(&DBCDRV_mspiHandle, &DBCDRV_mspiChannel, &DBCDRV_mspiCfg))
    {
        printk("DBCDRV_initComChannels: Error point R - MSPI_init failed.\n");
        return DBC_ERROR;
    }
    // #ifdef DBUSCAN_DMA_USED // DMA is not used in this Zephyr port
    // DBC_RETURN_ON_ERROR(DBCDRV_dmaInit());
    // #endif
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
    uint32_t regVal;

    printk("DBCDRV_init: Entry point.\n");
    printk("DBCDRV_init: Checking SPI_DEV_NODE readiness.\n");
    if (!device_is_ready(dbus_spi_bus)) {
        printk("DBCDRV_init: SPI_DEV_NODE (flexcomm1) is NOT ready!\n");
        return DBC_ERROR;
    } else {
        printk("DBCDRV_init: SPI_DEV_NODE (flexcomm1) is ready.\n");
    }

    printk("DBCDRV_init: Checking DBUS_CS_GPIO_NODE readiness.\n");
    if (!device_is_ready(dbus_cs_gpio_dev)) {
        printk("DBCDRV_init: DBUS_CS_GPIO_NODE (hsgpio0) is NOT ready!\n");
        return DBC_ERROR;
    } else {
        printk("DBCDRV_init: DBUS_CS_GPIO_NODE (hsgpio0) is ready.\n");
    }

    // 1. Introduce initial stabilization delay
    printk("DBCDRV_init: Starting initial 100ms delay.\n");
    k_msleep(100); // Wait 100ms for chip to stabilize after power-up
    printk("DBCDRV_init: Initial 100ms delay completed.\n");

    if(DBCDRV_isInternalEepromError)
    {
        printk("DBCDRV_init: Error point A - Internal EEPROM error detected previously. Aborting initialization.\n");
        return DBC_ERROR;
    }
    printk("DBCDRV_init: After Error point A check.\n");

    printk("DBCDRV_init: Before CS GPIO device ready check.\n");

    // Ensure CS is de-asserted before starting SPI communication
    if (!device_is_ready(dbus_cs_gpio_dev)) {
        printk("DBCDRV_init: Error point B - CS GPIO device not ready at start of init!\n");
        return DBC_ERROR;
    }
    gpio_pin_set(dbus_cs_gpio_dev, DBUS_CS_GPIO_PIN, 1);
    k_msleep(1); // Small delay
    printk("DBCDRV_init: After CS de-assertion and delay.\n");

    // 2. Add a dummy SPI read/write to help "wake up" the peripheral or synchronize the SPI bus
    uint32_t dummy_data = 0;
    printk("DBCDRV_init: Performing dummy read from DBC_SCRATCHPAD_ADDR (0x%x).\n", DBC_SCRATCHPAD_ADDR);
    (void)DBCDRV_readReg32(DBC_SCRATCHPAD_ADDR, &dummy_data); // Dummy read, ignore error for now
    printk("DBCDRV_init: Dummy read returned 0x%x.\n", dummy_data);

    printk("DBCDRV_init: Calling DBCDRV_initComChannels.\n");
    DBC_RETURN_ON_ERROR(DBCDRV_initComChannels((MCAL_CallbackFunction_t)DBCDRV_irqCallback)); // Cast to correct type
    printk("DBCDRV_init: DBCDRV_initComChannels returned OK.\n");

    // 3. Verify Power-On Reset (POR) Status
    printk("DBCDRV_init: Reading DBC_IF_ADDR for POR status.\n");
    DBC_RETURN_ON_ERROR(DBCDRV_readReg32(DBC_IF_ADDR, &regVal));
    printk("DBCDRV_init: DBC_IF_ADDR read returned 0x%x.\n", regVal);
    if (0u != (regVal & DBC_IF_PWRON_MASK)) {
        printk("DBCDRV_init: Power-On Reset (POR) flag detected (0x%x).\n", regVal);
        DBCDRV_isPwrOnReset = true;
    } else {
        printk("DBCDRV_init: Power-On Reset (POR) flag not set (0x%x).\n", regVal);
        DBCDRV_isPwrOnReset = false;
    }

    printk("DBCDRV_init: Setting power mode to STANDBY.\n");
    DBC_RETURN_ON_ERROR(DBCDRV_setPowerMode(DBC_POWER_MODE_STANDBY)); // Set to standby mode
    printk("DBCDRV_init: Power mode set to STANDBY.\n");
#ifdef DBUSCAN_SPI_CRC_USED
    printk("DBCDRV_init: Enabling SPI CRC.\n");
    DBC_RETURN_ON_ERROR(DBCDRV_enableSpiCrc());
    printk("DBCDRV_init: SPI CRC enabled.\n");
#else
    printk("DBCDRV_init: Disabling SPI CRC.\n");
    DBC_RETURN_ON_ERROR(DBCDRV_disableSpiCrc());
    printk("DBCDRV_init: SPI CRC disabled.\n");
#endif
    printk("DBCDRV_init: Performing DBCDRV_doReset.\n");
    DBC_RETURN_ON_ERROR(DBCDRV_doReset(&DBCDRV_isInternalEepromError));
    printk("DBCDRV_init: DBCDRV_doReset completed.\n");
    printk("DBCDRV_init: Configuring DBus driver.\n");
    DBC_RETURN_ON_ERROR(DBCDRV_configure(cfg));
    printk("DBCDRV_init: DBus driver configured.\n");
    printk("DBCDRV_init: Writing IPEC register for NWKRQ_DELAY.\n");
    DBC_RETURN_ON_ERROR(DBCDRV_writeRegIpec(0uL, DBC_IPEC_NWKRQ_DELAY_POS, DBC_IPEC_NWKRQ_DELAY_MASK)); // clear NWKRQ_DELAY bit to disable nWKRQ pin delayed de-assertion in sleep mode
    printk("DBCDRV_init: IPEC register updated.\n");
    // DBC_RETURN_ON_ERROR(DBCDRV_enableCfgDbus()); // This is handled within DBCDRV_configure
    printk("DBCDRV_init: Enabling and clearing IRQ flags.\n");
    DBC_RETURN_ON_ERROR(DBCDRV_enableAndClearIrqFlags(0)); // Placeholder for flags
    printk("DBCDRV_init: IRQ flags enabled and cleared.\n");
    printk("DBCDRV_init: Configuring rest for DBus.\n");
    DBC_RETURN_ON_ERROR(DBCDRV_configureRestForDbus(&cfg));
    printk("DBCDRV_init: Rest for DBus configured.\n");
    printk("DBCDRV_init: Setting power mode to NORMAL.\n");
    DBC_RETURN_ON_ERROR(DBCDRV_setPowerMode(DBC_POWER_MODE_NORMAL)); // Set to normal mode
    printk("DBCDRV_init: Power mode set to NORMAL.\n");
    printk("DBCDRV_init: Starting 10ms delay after setting to normal mode.\n");
    k_msleep(10); // Add delay after setting to normal mode
    printk("DBCDRV_init: 10ms delay completed.\n");

#ifdef APP_VARIANT
    printk("DBCDRV_init: Enabling MEXTI event.\n");
    DBC_RETURN_ON_ERROR(MEXTI_enableEvent(&DBCDRV_mextiHandle, &DBCDRV_cbIrqHandle));
    printk("DBCDRV_init: MEXTI event enabled.\n");
#endif //APP_VARIANT
    printk("DBCDRV_init: Initialization complete.\n");

    DBCDRV_isPwrOnReset = false; // Reset flag after successful initialization

    return DBC_OK;
}