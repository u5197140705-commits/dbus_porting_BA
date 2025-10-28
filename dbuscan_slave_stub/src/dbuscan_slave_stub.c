/*
 * dbuscan_slave_stub.c
 *
 * Simple SPI slave implementation to simulate the DBusCAN chip behavior.
 * Used when running Zephyr on STM32G071 (slave) to communicate
 * with an FRDM-RW612 master.
 */

 #include <zephyr/kernel.h>
 #include <zephyr/device.h>
 #include <zephyr/drivers/spi.h>
 #include <zephyr/drivers/gpio.h>
 #include <zephyr/logging/log.h>
 #include <string.h>
 
 LOG_MODULE_REGISTER(dbuscan_slave, LOG_LEVEL_DBG);
 
 /* =======================================================
  *  SPI Device and Configuration
  * ======================================================= */
 #define SPI_SLAVE_NODE DT_ALIAS(spi_slave)
 #if !DT_NODE_HAS_STATUS(SPI_SLAVE_NODE, okay)
 #error "No alias 'spi_slave' defined in device tree overlay"
 #endif
 
 static const struct device *spi_dev = DEVICE_DT_GET(SPI_SLAVE_NODE);
 
 /* Configure SPI in slave mode */
 static const struct spi_config spi_cfg = {
    .operation = SPI_OP_MODE_SLAVE | SPI_WORD_SET(8),
    .frequency = 1000000U,
    .slave = 0,
    .cs = NULL,
};

 
 /* =======================================================
  *  SPI Buffers
  * ======================================================= */
 #define DBUSCAN_BUF_SIZE 8
 
 static uint8_t tx_buf[DBUSCAN_BUF_SIZE];
 static uint8_t rx_buf[DBUSCAN_BUF_SIZE];
 
 static struct spi_buf tx_bufs[] = {
     { .buf = tx_buf, .len = sizeof(tx_buf) },
 };
 static struct spi_buf rx_bufs[] = {
     { .buf = rx_buf, .len = sizeof(rx_buf) },
 };
 static const struct spi_buf_set tx_set = { .buffers = tx_bufs, .count = 1 };
 static const struct spi_buf_set rx_set = { .buffers = rx_bufs, .count = 1 };
 
 /* =======================================================
  *  Example register emulation (like DBusCAN registers)
  * ======================================================= */
 static uint32_t scratchpad = 0x00000000;
 #define DBC_SCRATCHPAD_ADDR 0x1C
 #define DBC_MOPC_ADDR       0x800
 #define DBC_IF_ADDR         0x820
 
 /* =======================================================
  *  Simple slave logic: decode address, store or echo data
  * ======================================================= */
 static void process_spi_command(void)
 {
     uint8_t cmd = rx_buf[0];  // 0x60 = write, 0x40 = read
     uint16_t addr = ((uint16_t)rx_buf[1] << 8) | rx_buf[2];
     uint32_t value = ((uint32_t)rx_buf[4] |
                       ((uint32_t)rx_buf[5] << 8) |
                       ((uint32_t)rx_buf[6] << 16) |
                       ((uint32_t)rx_buf[7] << 24));
 
     LOG_HEXDUMP_DBG(rx_buf, sizeof(rx_buf), "RX data");
 
     if (cmd == 0x60) {
         /* Write */
         if (addr == DBC_SCRATCHPAD_ADDR) {
             scratchpad = value;
         }
         LOG_INF("SPI WRITE addr=0x%03x val=0x%08x", addr, value);
     } else if (cmd == 0x40) {
         /* Read */
         uint32_t response = 0x00000000;
 
         if (addr == DBC_SCRATCHPAD_ADDR)
             response = scratchpad;
         else if (addr == DBC_MOPC_ADDR)
             response = 0x00000040;  // Example: mode bits
         else if (addr == DBC_IF_ADDR)
             response = 0x00000001;  // Example: POR flag
 
         /* Prepare TX buffer */
         tx_buf[0] = 0x00;
         tx_buf[1] = 0x00;
         tx_buf[2] = 0x00;
         tx_buf[3] = 0x00;
         tx_buf[4] = response & 0xFF;
         tx_buf[5] = (response >> 8) & 0xFF;
         tx_buf[6] = (response >> 16) & 0xFF;
         tx_buf[7] = (response >> 24) & 0xFF;
 
         LOG_HEXDUMP_DBG(tx_buf, sizeof(tx_buf), "TX data (next response)");
         LOG_INF("SPI READ addr=0x%03x -> val=0x%08x", addr, response);
     } else {
         LOG_WRN("Unknown SPI command 0x%02x", cmd);
     }
 }
 
 /* =======================================================
  *  Main SPI Slave Thread
  * ======================================================= */
 void dbuscan_slave_thread(void)
 {
     LOG_INF("DBusCAN Slave Stub started. Waiting for SPI master...");
     k_msleep(500);  /* Allow master to boot first */
 
     while (1) {
         memset(rx_buf, 0, sizeof(rx_buf));
         memset(tx_buf, 0, sizeof(tx_buf));
 
         int ret = spi_transceive(spi_dev, &spi_cfg, &tx_set, &rx_set);
         if (ret == 0) {
             process_spi_command();
         } else {
             LOG_ERR("spi_transceive() failed: %d", ret);
             k_msleep(100);
         }
 
         k_msleep(1);
     }
 }
 
 /* Start thread */
 K_THREAD_DEFINE(dbuscan_slave_tid, 2048, dbuscan_slave_thread, NULL, NULL, NULL,
                 7, 0, 0);
 