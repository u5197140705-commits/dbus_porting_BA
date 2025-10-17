#ifndef STD_CRC_H
#define STD_CRC_H

#include <stdint.h>
#include <stdbool.h>

// Define initial CRC values
#define CRC_CCITT_INIT 0xFFFFu
#define CRC_ETHERNET_INIT 0xFFFFFFFFu
#define CRC_ETHERNET_XOR_OUT 0xFFFFFFFFu

// Function prototypes for CRC-16/CCITT-FALSE
uint16_t CRC_Ccitt_Init(void);
uint16_t CRC_Ccitt_UpdateByte(uint16_t crc, uint8_t value);
uint16_t CRC_Ccitt_Update(uint16_t crc, uint32_t len, const uint8_t *data);
uint16_t CRC_Ccitt_Finish(uint16_t crc);
uint16_t CRC_Ccitt_Calc(uint32_t len, const uint8_t *data);

// Function prototypes for CRC-32/Ethernet
uint32_t CRC_Ethernet_Init(void);
uint32_t CRC_Ethernet_UpdateByte(uint32_t crc, uint8_t value);
uint32_t CRC_Ethernet_Update(uint32_t crc, uint32_t len, const uint8_t *data);
uint32_t CRC_Ethernet_Finish(uint32_t crc);
uint32_t CRC_Ethernet_Calc(uint32_t len, const uint8_t *data);

#endif // STD_CRC_H