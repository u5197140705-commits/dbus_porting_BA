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
 *  COMP_ABBREV      DBC
 ******************************************************************************/

#ifndef DBUSCAN_H
#define DBUSCAN_H

#ifdef __cplusplus
extern "C"
{
#endif


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *  \ingroup dbuscan
 *
 *  \brief   Header file with register and bit definitions for the DBusCAN chip
 *
 */


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include <stdbool.h>
#include "LibDefinesUc.h" // PACKED, TbitField


/******************************************************************************/
/* PUBLIC DEFINITIONS                                                         */
/******************************************************************************/
/* Minor issues:
 * Warning 657: Unusual (nonportable) anonymous struct or union
 * Warning 658: Anonymous union assumed (use flag +fan)
 * Note 9018: union declared [MISRA 2012 Rule 19.2, advisory]
 */
/*lint -save -e657 -e658 -e9018*/

/* Base addresses */
#define DBC_DBUS_NF_BASE       (0x4200u)  ///< Base address of Node Filter registers
#define DBC_DBUS_TXSF_BASE     (0x4300u)  ///< Base address of DBus Tx Status FIFO buffer
#define DBC_DBUS_TXF_BASE      (0x4400u)  ///< Base address of DBus Tx FIFO buffer
#define DBC_DBUS_RXF_BASE      (0x4400u)  ///< Base address of DBus Rx FIFO buffer
#define DBC_RAM_BASE           (0x8000u)  ///< Base address of RAM memory

/* Size definitions */
#define DBC_RAM_SIZE             (2048u)  ///< The number of bytes of RAM memory inside the chip
#define DBC_DBUS_FIFO_MAX_MSG_CNT   (8u)  ///< DBus Tx/Rx FIFO can hold up to 8 DBus frames
#define DBC_SPI_HDR_SIZE            (4u)  ///< Each SPI frame starts with 4-byte SPI header (1 command byte + 2-byte address + 1 data length byte)
#ifdef DBUSCAN_SPI_CRC_USED
#define DBC_SPI_CRC_SIZE            (4u)  ///< The number of bytes for CRC in SPI frame if CRC is enabled
#else
#define DBC_SPI_CRC_SIZE            (0u)  ///< The number of bytes for CRC in SPI frame if CRC is disabled
#endif


/** Register addresses of the chip */
enum DBC_RegAddr
{
    /* DIFR - Device ID and Interrupt/Diagnostic Flag Registers: addresses 0x0000- 0x002F */
    DBC_DEVICE_ID1_ADDR  = 0x0000u,
    DBC_DEVICE_ID2_ADDR  = 0x0004u,
    DBC_REVISION_ADDR    = 0x0008u,
    DBC_STATUS_ADDR      = 0x000Cu,    /* Device Status register */
    DBC_SPI_ERR_MASK_ADDR= 0x0010u,    /* SPI Error status Mask register */
    DBC_SPI_CRC_CFG_ADDR = 0x0014u,    /* SPI CRC Configuration register */
    DBC_SPI_CRC_SEED_ADDR= 0x0018u,    /* SPI CRC Seed value register */
    DBC_SCRATCHPAD_ADDR  = 0x001Cu,    /* Scratchpad register */
    /* DCR - Device Configuration Registers, addresses: 0x0800 - 0x08FF */
    DBC_MOPC_ADDR        = 0x0800u,    /* Modes of Operation and Pin Configuration register */
    DBC_TSP_ADDR         = 0x0804u,    /* TimeStamp Prescalar register*/
    DBC_TEST_SP_ADDR     = 0x0808u,    /* Test and ScratchPad register */
    DBC_ECC_ADDR         = 0x080Cu,    /* ECC register */
    DBC_IPEC_ADDR        = 0x0814u,    /* IP Enable and Control register */
    DBC_EEPP_ADDR        = 0x0818u,    /* EEPROM Pending register */
    DBC_EEPC_ADDR        = 0x081Cu,    /* EEPROM Current register */
    DBC_IF_ADDR          = 0x0820u,    /* Device Interrupt Flags register */
    DBC_CAN_IF_ADDR      = 0x0824u,    /* CAN Interrupt Flags register */
    DBC_DBUS_IF_ADDR     = 0x0828u,    /* DBus Interrupt Flags register */
    DBC_IE_ADDR          = 0x0830u,    /* Interrupt Enable register*/
    /* CAN registers, addresses: 0x1000 - 0x10FF */
    DBC_CAN_CREL_ADDR    = 0x1000u,    /* Core Release register */
    DBC_CAN_ENDN_ADDR    = 0x1004u,    /* Endian register */
    DBC_CAN_CUST_ADDR    = 0x1008u,    /* Customer register */
    DBC_CAN_DBTP_ADDR    = 0x100Cu,    /* Data Bit Timing & Prescaler register */
    DBC_CAN_TEST_ADDR    = 0x1010u,    /* Test register */
    DBC_CAN_RWD_ADDR     = 0x1014u,    /* RAM Watchdog register */
    DBC_CAN_CCCR_ADDR    = 0x1018u,    /* Control Register */
    DBC_CAN_NBTP_ADDR    = 0x101Cu,    /* Nominal Bit Timing & Prescaler register */
    DBC_CAN_TSCC_ADDR    = 0x1020u,    /* TimeStamp Counter Configuration register */
    DBC_CAN_TSCV_ADDR    = 0x1024u,    /* TimeStamp Counter Value register */
    DBC_CAN_TOCC_ADDR    = 0x1028u,    /* TimeOut Counter Configuration register */
    DBC_CAN_TOCV_ADDR    = 0x102Cu,    /* TimeOut Counter Value register */
    DBC_CAN_ECR_ADDR     = 0x1040u,    /* Error Counter Register */
    DBC_CAN_PSR_ADDR     = 0x1044u,    /* Protocol Status Register */
    DBC_CAN_TDCR_ADDR    = 0x1048u,    /* Transmitter Delay Compensation Register */
    DBC_CAN_IR_ADDR      = 0x1050u,    /* Interrupt Register */
    DBC_CAN_IE_ADDR      = 0x1054u,    /* Interrupt Enable register */
    DBC_CAN_ILS_ADDR     = 0x1058u,    /* Interrupt Line Select register */
    DBC_CAN_ILE_ADDR     = 0x105Cu,    /* Interrupt Line Enable register */
    DBC_CAN_GFC_ADDR     = 0x1080u,    /* Global Filter Configuration register */
    DBC_CAN_SIDFC_ADDR   = 0x1084u,    /* Standard ID Filter Configuration register */
    DBC_CAN_XIDFC_ADDR   = 0x1088u,    /* Extended ID Filter Configuration register */
    DBC_CAN_XIDAM_ADDR   = 0x1090u,    /* Extended ID And Mask register */
    DBC_CAN_HPMS_ADDR    = 0x1094u,    /* High Priority Message Status register */
    DBC_CAN_NDAT1_ADDR   = 0x1098u,    /* New Data 1 register */
    DBC_CAN_NDAT2_ADDR   = 0x109Cu,    /* New Data 2 register */
    DBC_CAN_RXF0C_ADDR   = 0x10A0u,    /* Rx FIFO 0 Configuration register */
    DBC_CAN_RXF0S_ADDR   = 0x10A4u,    /* Rx FIFO 0 Status register */
    DBC_CAN_RXF0A_ADDR   = 0x10A8u,    /* Rx FIFO 0 Acknowledge register */
    DBC_CAN_RXBC_ADDR    = 0x10ACu,    /* Rx Buffer Configuration register */
    DBC_CAN_RXF1C_ADDR   = 0x10B0u,    /* Rx FIFO 1 Configuration register */
    DBC_CAN_RXF1S_ADDR   = 0x10B4u,    /* Rx FIFO 1 Status register */
    DBC_CAN_RXF1A_ADDR   = 0x10B8u,    /* Rx FIFO 1 Acknowledge register */
    DBC_CAN_RXESC_ADDR   = 0x10BCu,    /* Rx Buffer/FIFO Element Size Configuration register */
    DBC_CAN_TXBC_ADDR    = 0x10C0u,    /* Tx Buffer Configuration register */
    DBC_CAN_TXFQS_ADDR   = 0x10C4u,    /* Tx FIFO/Queue Status register */
    DBC_CAN_TXESC_ADDR   = 0x10C8u,    /* Tx Buffer/FIFO Element Size Configuration register */
    DBC_CAN_TXBRP_ADDR   = 0x10CCu,    /* Tx Buffer Request Pending register */
    DBC_CAN_TXBAR_ADDR   = 0x10D0u,    /* Tx Buffer Add Request register*/
    DBC_CAN_TXBCR_ADDR   = 0x10D4u,    /* Tx Buffer Cancellation Request register*/
    DBC_CAN_TXBTO_ADDR   = 0x10D8u,    /* Tx Buffer Transmission Occurred register */
    DBC_CAN_TXBCF_ADDR   = 0x10DCu,    /* Tx Buffer Cancellation Finished register */
    DBC_CAN_TXBTIE_ADDR  = 0x10E0u,    /* Tx Buffer Transmission Interrupt Enable register */
    DBC_CAN_TXBCIE_ADDR  = 0x10E4u,    /* Tx Buffer Cancellation finished Interrupt Enable register */
    DBC_CAN_TXEFC_ADDR   = 0x10F0u,    /* Tx Event FIFO Configuration register */
    DBC_CAN_TXEFS_ADDR   = 0x10F4u,    /* Tx Event FIFO Status register */
    DBC_CAN_TXEFA_ADDR   = 0x10F8u,    /* Tx Event FIFO Acknowledge register */
    DBC_CAN_SWCR_ADDR    = 0x2010u,    /* Selective Wake Control Register */
    /* DBus registers, addresses: 0x4000 - 0x4300 */
    DBC_DBUS_CREL_ADDR   = 0x4000u,    /* Core Release register                      Address offset: 0x4000 */
    DBC_DBUS_ENDN_ADDR   = 0x4004u,    /* Endianness test value register             Address offset: 0x4004 */
    DBC_DBUS_TEST_ADDR   = 0x4010u,    /* Test register                              Address offset: 0x4010 */
    DBC_DBUS_CCCR_ADDR   = 0x4018u,    /* CC Control Register                        Address offset: 0x4018 */
    DBC_DBUS_TSCC_ADDR   = 0x4020u,    /* TimeStamp Counter Configuration register   Address offset: 0x4020 */
    DBC_DBUS_TSCV_ADDR   = 0x4024u,    /* TimeStamp Counter Value register           Address offset: 0x4024 */
    DBC_DBUS_TOCC_ADDR   = 0x4028u,    /* TimeOut Counter Configuration register     Address offset: 0x4028 */
    DBC_DBUS_TOCV_ADDR   = 0x402Cu,    /* TimeOut Counter Value register             Address offset: 0x402C */
    DBC_DBUS_RXC_ADDR    = 0x4034u,    /* Receive Control register                   Address offset: 0x4034 */
    DBC_DBUS_RW_ADDR     = 0x403Cu,    /* RW test register                           Address offset: 0x403C */
    DBC_DBUS_PSR_ADDR    = 0x4044u,    /* Protocol Status Register                   Address offset: 0x4044 */
    DBC_DBUS_DBR_ADDR    = 0x404Cu,    /* DBus Baudrate Register                     Address offset: 0x404C */
    DBC_DBUS_IR_ADDR     = 0x4050u,    /* Interrupt Register                         Address offset: 0x4050 */
    DBC_DBUS_IE_ADDR     = 0x4054u,    /* Interrupt Enable register                  Address offset: 0x4054 */
    DBC_DBUS_BSA_ADDR    = 0x4060u,    /* Base SRAM Address register                 Address offset: 0x4060 */
    DBC_DBUS_BSC0_ADDR   = 0x4064u,    /* Buffer Size Control 0 register             Address offset: 0x4064 */
    DBC_DBUS_BSC1_ADDR   = 0x4068u,    /* Buffer Size Control 1 register             Address offset: 0x4068 */
    DBC_DBUS_BCC_ADDR    = 0x406Cu,    /* Buffer Clear Control register              Address offset: 0x406C */
    DBC_DBUS_DPA_ADDR    = 0x4070u,    /* DBus Power Action register                 Address offset: 0x4070 */
    DBC_DBUS_DPC_ADDR    = 0x4074u,    /* DBus Power Control register                Address offset: 0x4074 */
    DBC_DBUS_DPS_ADDR    = 0x4078u,    /* DBus Power Status register                 Address offset: 0x4078 */
    DBC_DBUS_SIDFC_ADDR  = 0x4084u,    /* Node Filter Configuration Register         Address offset: 0x4084 */
    DBC_DBUS_RXF0S_ADDR  = 0x40A4u,    /* Rx FIFO Status register                    Address offset: 0x40A4 */
    DBC_DBUS_TXFQS_ADDR  = 0x40C4u,    /* Tx FIFO Queue Status register              Address offset: 0x40C4 */
    DBC_DBUS_TXBTIE_ADDR = 0x40E0u,    /* Tx Buffer Transmission Interrupt Enable    Address offset: 0x40E0 */
    DBC_DBUS_TXEFS_ADDR  = 0x40F4u,    /* Tx Event (status) FIFO Status register     Address offset: 0x40F4 */
    DBC_DBUS_NF0_ADDR    = DBC_DBUS_NF_BASE,       /* DBus Node Filter 0              Address offset: 0x4200 */
    DBC_DBUS_NF1_ADDR    = DBC_DBUS_NF_BASE + 4u,  /* DBus Node Filter 1              Address offset: 0x4204 */
    DBC_DBUS_NF2_ADDR    = DBC_DBUS_NF_BASE + 8u,  /* DBus Node Filter 2              Address offset: 0x4208 */
    DBC_DBUS_NF3_ADDR    = DBC_DBUS_NF_BASE + 12u, /* DBus Node Filter 3              Address offset: 0x420C */
    DBC_DBUS_NF4_ADDR    = DBC_DBUS_NF_BASE + 16u, /* DBus Node Filter 4              Address offset: 0x4210 */
    DBC_DBUS_NF5_ADDR    = DBC_DBUS_NF_BASE + 20u, /* DBus Node Filter 5              Address offset: 0x4214 */
    DBC_DBUS_NF6_ADDR    = DBC_DBUS_NF_BASE + 24u, /* DBus Node Filter 6              Address offset: 0x4218 */
    DBC_DBUS_NF7_ADDR    = DBC_DBUS_NF_BASE + 28u, /* DBus Node Filter 7              Address offset: 0x421C */
    DBC_DBUS_NF8_ADDR    = DBC_DBUS_NF_BASE + 32u, /* DBus Node Filter 8              Address offset: 0x4220 */
    DBC_DBUS_NF9_ADDR    = DBC_DBUS_NF_BASE + 36u, /* DBus Node Filter 9              Address offset: 0x4224 */
    DBC_DBUS_NF10_ADDR   = DBC_DBUS_NF_BASE + 40u, /* DBus Node Filter 10             Address offset: 0x4228 */
    DBC_DBUS_NF11_ADDR   = DBC_DBUS_NF_BASE + 44u, /* DBus Node Filter 11             Address offset: 0x422C */
    DBC_DBUS_NF12_ADDR   = DBC_DBUS_NF_BASE + 48u, /* DBus Node Filter 12             Address offset: 0x4230 */
    DBC_DBUS_NF13_ADDR   = DBC_DBUS_NF_BASE + 52u, /* DBus Node Filter 13             Address offset: 0x4234 */
    DBC_DBUS_NF14_ADDR   = DBC_DBUS_NF_BASE + 56u, /* DBus Node Filter 14             Address offset: 0x4238 */
    DBC_DBUS_TXSF_ADDR   = DBC_DBUS_TXSF_BASE,     /* DBus Transmit Status FIFO       Address offset: 0x4300 */
    DBC_DBUS_RX_TX_FIFO_ADDR = DBC_DBUS_TXF_BASE   /* DBus Receive/Transmit FIFO      Address offset: 0x4400 */
};


/** DIFR (Device ID and Interrupt/Diagnostic Flag Registers) address space (0x0000- 0x002F) */

/* DIFR->REVISION (0x0008) */
#define DBC_REVISION_WITH_ALL_FEATURES          0x03000200u      ///< Starting revision of DBusCAN chip which supports all features

/* DIFR->STATUS (0x000C) */
/* Definitions of Bit-Masks */
#define DBC_STATUS_INRERR_MASK                  0x20000000u      ///< Mask for Internal Read Error
#define DBC_STATUS_INWERR_MASK                  0x10000000u      ///< Mask for Internal Write Error
#define DBC_STATUS_INERRLW_MASK                 0x08000000u      ///< Mask for Internal Error Log Write
#define DBC_STATUS_RFUF_MASK                    0x04000000u      ///< Mask for Read FIFO Underflow after 1 or more read data words returned
#define DBC_STATUS_RFE_MASK                     0x02000000u      ///< Mask for Read FIFO Empty for first read data word to return
#define DBC_STATUS_WFOVF_MASK                   0x01000000u      ///< Mask for Write FIFO Overflow
#define DBC_STATUS_SPIEERR_MASK                 0x00200000u      ///< Mask for SPI End Error (SPI transfer did not end on a byte boundary)
#define DBC_STATUS_SPIICMD_MASK                 0x00100000u      ///< Mask for SPI Invalid Command received
#define DBC_STATUS_SPIWOVF_MASK                 0x00080000u      ///< Mask for SPI Write Overflow (SPI write sequence had continue requests after the data transfer was completed)
#define DBC_STATUS_SPIWUF_MASK                  0x00040000u      ///< Mask for SPI Write Underflow (SPI write sequence ended with less data transferred then requested)
#define DBC_STATUS_SPIROVF_MASK                 0x00020000u      ///< Mask for SPI Read Overflow (SPI read sequence had continue requests after the data transfer was completed)
#define DBC_STATUS_SPIRUF_MASK                  0x00010000u      ///< Mask for SPI Read Underflow (SPI read sequence ended with less data transferred then requested)
#define DBC_STATUS_WFA_MASK                     0x00000020u      ///< Mask for Write FIFO Available- RO
#define DBC_STATUS_RFA_MASK                     0x00000010u      ///< Mask for Read FIFO Available- RO
#define DBC_STATUS_INAA_MASK                    0x00000008u      ///< Mask for Internal Access Active- RO
#define DBC_STATUS_INERR_MASK                   0x00000004u      ///< Mask for unmasked Internal Error interrupt set- RO
#define DBC_STATUS_SPIERR_MASK                  0x00000002u      ///< Mask for unmasked SPI Error interrupt set- RO
#define DBC_STATUS_IR_MASK                      0x00000001u      ///< Mask for value of Interrupt input level (active low)- RO
/* Definitions of Bit-Positions */
#define DBC_STATUS_INRERR_POS                           29u      ///< Position for Internal Read Error
#define DBC_STATUS_INWERR_POS                           28u      ///< Position for Internal Write Error
#define DBC_STATUS_INERRLW_POS                          27u      ///< Position for Internal Error Log Write
#define DBC_STATUS_RFUF_POS                             26u      ///< Position for Read FIFO Underflow after 1 or more read data words returned
#define DBC_STATUS_RFE_POS                              25u      ///< Position for Read FIFO Empty for first read data word to return
#define DBC_STATUS_WFOVF_POS                            24u      ///< Position for Write FIFO Overflow
#define DBC_STATUS_SPIEERR_POS                          21u      ///< Position for SPI End Error (SPI transfer did not end on a byte boundary)
#define DBC_STATUS_SPIICMD_POS                          20u      ///< Position for SPI Invalid Command received
#define DBC_STATUS_SPIWOVF_POS                          19u      ///< Position for SPI Write Overflow (SPI write sequence had continue requests after the data transfer was completed)
#define DBC_STATUS_SPIWUF_POS                           18u      ///< Position for SPI Write Underflow (SPI write sequence ended with less data transferred then requested)
#define DBC_STATUS_SPIROVF_POS                          17u      ///< Position for SPI Read Overflow (SPI read sequence had continue requests after the data transfer was completed)
#define DBC_STATUS_SPIRUF_POS                           16u      ///< Position for SPI Read Underflow (SPI read sequence ended with less data transferred then requested)
#define DBC_STATUS_WFA_POS                               5u      ///< Position for Write FIFO Available- RO
#define DBC_STATUS_RFA_POS                               4u      ///< Position for Read FIFO Available- RO
#define DBC_STATUS_INAA_POS                              3u      ///< Position for Internal Access Active- RO
#define DBC_STATUS_INERR_POS                             2u      ///< Position for unmasked Internal Error interrupt set- RO
#define DBC_STATUS_SPIERR_POS                            1u      ///< Position for unmasked SPI Error interrupt set- RO
#define DBC_STATUS_IR_POS                                0u      ///< Position for value of Interrupt input level (active low)- RO

/* DIFR->SPI_ERR_MASK (0x0010) */
/* When any of the following bits is set the corresponding error bit will be masked */
/* Definitions of Bit-Masks */
#define DBC_SPI_ERR_MASK_INRERR_MASK            0x20000000u      ///< Mask for Mask Internal Read Error
#define DBC_SPI_ERR_MASK_INWERR_MASK            0x10000000u      ///< Mask for Mask Internal Write Error
#define DBC_SPI_ERR_MASK_INERRLW_MASK           0x08000000u      ///< Mask for Mask Internal Error Log Write
#define DBC_SPI_ERR_MASK_RFUF_MASK              0x04000000u      ///< Mask for Mask Read FIFO Underflow
#define DBC_SPI_ERR_MASK_RFE_MASK               0x02000000u      ///< Mask for Mask Read FIFO Empty
#define DBC_SPI_ERR_MASK_WFOVF_MASK             0x01000000u      ///< Mask for Mask Write FIFO Overflow
#define DBC_SPI_ERR_MASK_SPIEERR_MASK           0x00200000u      ///< Mask for Mask SPI End Error
#define DBC_SPI_ERR_MASK_SPIICMD_MASK           0x00100000u      ///< Mask for Mask Invalid Command
#define DBC_SPI_ERR_MASK_SPIWOVF_MASK           0x00080000u      ///< Mask for Mask Write Overflow
#define DBC_SPI_ERR_MASK_SPIWUF_MASK            0x00040000u      ///< Mask for Mask Write Underflow
#define DBC_SPI_ERR_MASK_SPIROVF_MASK           0x00020000u      ///< Mask for Mask Read Overflow
#define DBC_SPI_ERR_MASK_SPIRUF_MASK            0x00010000u      ///< Mask for Mask Read Underflow
/* Definitions of Bit-Positions */
#define DBC_SPI_ERR_MASK_INRERR_POS                     29u      ///< Position for Mask Internal Read Error
#define DBC_SPI_ERR_MASK_INWERR_POS                     28u      ///< Position for Mask Internal Write Error
#define DBC_SPI_ERR_MASK_INERRLW_POS                    27u      ///< Position for Mask Internal Error Log Write
#define DBC_SPI_ERR_MASK_RFUF_POS                       26u      ///< Position for Mask Read FIFO Underflow
#define DBC_SPI_ERR_MASK_RFE_POS                        25u      ///< Position for Mask Read FIFO Empty
#define DBC_SPI_ERR_MASK_WFOVF_POS                      24u      ///< Position for Mask Write FIFO Overflow
#define DBC_SPI_ERR_MASK_SPIEERR_POS                    21u      ///< Position for Mask SPI End Error
#define DBC_SPI_ERR_MASK_SPIICMD_POS                    20u      ///< Position for Mask Invalid Command
#define DBC_SPI_ERR_MASK_SPIWOVF_POS                    19u      ///< Position for Mask Write Overflow
#define DBC_SPI_ERR_MASK_SPIWUF_POS                     18u      ///< Position for Mask Write Underflow
#define DBC_SPI_ERR_MASK_SPIROVF_POS                    17u      ///< Position for Mask Read Overflow
#define DBC_SPI_ERR_MASK_SPIRUF_POS                     16u      ///< Position for Mask Read Underflow

/* DIFR->SPI_CRC_CFG (0x0014) */
/* Definitions of Bit-Masks */
#define DBC_SPI_CRC_CFG_BYTE_ORDER_MASK         0x00000020u      ///< Mask for CRC Byte Order
#define DBC_SPI_CRC_CFG_POLY_MASK               0x00000018u      ///< Mask for CRC Polynomial used for CRC calculations
#define DBC_SPI_CRC_CFG_SEED_CFG_MASK           0x00000006u      ///< Mask for CRC Seed used for CRC calculations
#define DBC_SPI_CRC_CFG_EN_MASK                 0x00000001u      ///< Mask for CRC Enable
/* Definitions of Bit-Positions */
#define DBC_SPI_CRC_CFG_BYTE_ORDER_POS                   5u      ///< Position for CRC Byte Order
#define DBC_SPI_CRC_CFG_POLY_POS                         3u      ///< Position for CRC Polynomial used for CRC calculations
#define DBC_SPI_CRC_CFG_SEED_CFG_POS                     1u      ///< Position for CRC Seed used for CRC calculations
#define DBC_SPI_CRC_CFG_EN_POS                           0u      ///< Position for CRC Enable

/** Possible values for DBC_SPI_CRC_CFG_BYTE_ORDER bitfield of the SPI_CRC_CFG register */
enum DBC_SpiCrcByteOrder {
    DBC_SPI_CRC_ORDER_MSB = 0u,
    DBC_SPI_CRC_ORDER_LSB = 0x20u
};

/** Possible values for DBC_SPI_CRC_CFG_POLY bitfield of the SPI_CRC_CFG register */
enum DBC_SpiCrcPolynomial {
    DBC_SPI_CRC_POLY_IBM      = 0,       ///< 0x8005 (CRC-16-IBM)
    DBC_SPI_CRC_POLY_CCITT    = 0x08u,   ///< 0x1021 (CRC-16-CCITT)
    DBC_SPI_CRC_POLY_CDMA2000 = 0x10u,   ///< 0xC867 (CRC-16-CDMA2000)
    DBC_SPI_CRC_POLY_DECT     = 0x18u    ///< 0x0589 (CRC-16-DECT)
};

/** Possible values for DBC_SPI_CRC_CFG_SEED_CFG bitfield of the SPI_CRC_CFG register */
enum DBC_SpiCrcSeed {
    DBC_SPI_CRC_SEED_0        = 0u,      ///< Zero seed value
    DBC_SPI_CRC_SEED_0XFFFF   = 0x02u,   ///< 0xFFFF seed value
    DBC_SPI_CRC_SEED_USER     = 0x04u    ///< Use value in DBC_SPI_CRC_SEED register
};


/** DCR (Device Configuration Registers) address space (0x0800- 0x08FF) */

/* DCR->MOPC (0x0800) */
/* Definitions of Bit-Masks */
#define DBC_MOPC_MODE_SEL_MASK                  0x000000C0u      ///< Mask for Mode of operation select
#define DBC_MOPC_DEVICE_RESET_MASK              0x00000004u      ///< Mask for Device reset
/* Definitions of Bit-Positions */
#define DBC_MOPC_MODE_SEL_POS                            6u      ///< Position for Mode of operation select
#define DBC_MOPC_DEVICE_RESET_POS                        2u      ///< Position for Device reset

/* DCR->IPEC (0x0814) */
#define DBC_IPEC_EP_CC_VAL                          0x17CAu      ///< Value for EEPROM Control Code
/* Definitions of Bit-Masks */
#define DBC_IPEC_EP_WR_MASK                     0x80000000u      ///< Mask for EEPROM Write bit
#define DBC_IPEC_EP_CC_MASK                     0x7FFF0000u      ///< Mask for EEPROM Control Code
#define DBC_IPEC_VELIO_EN_MASK                  0x00008000u      ///< Mask for CAN Transceiver VeLIO enable bit
#define DBC_IPEC_CLK_SEL_MASK                   0x00002000u      ///< Mask for Clock Source Selection bit
#define DBC_IPEC_CLK_FORCE_MASK                 0x00001000u      ///< Mask for Clock Source Override bit
#define DBC_IPEC_CAN_BIAS_MASK                  0x00000800u      ///< Mask for CAN Sleep Mode Bias Voltage
#define DBC_IPEC_CAN_RST_EN_MASK                0x00000400u      ///< Mask for CAN Reset frame Enable bit
#define DBC_IPEC_DBUS2CAN_MASK                  0x00000200u      ///< Mask for DBus TXD/RXD to CAN transceiver enable bit
#define DBC_IPEC_NWKRQ_DELAY_MASK               0x00000100u      ///< Mask for nWKRQ delayed de-assertion in sleep mode enable bit
#define DBC_IPEC_CLK_INT_MASK                   0x00000080u      ///< Mask for Clock Status (Internal or External) bit
#define DBC_IPEC_CLK_XTAL_MASK                  0x00000040u      ///< Mask for Clock Crystal Detected bit
#define DBC_IPEC_DBUS_EN_MASK                   0x00000004u      ///< Mask for DBus IP enable bit
#define DBC_IPEC_MCAN_EN_MASK                   0x00000002u      ///< Mask for CAN IP enable bit
#define DBC_IPEC_CCE_MASK                       0x00000001u      ///< Mask for Configuration Change Enable
/* Definitions of Bit-Positions */
#define DBC_IPEC_EP_WR_POS                              31u      ///< Position for EEPROM Write bit
#define DBC_IPEC_EP_CC_POS                              16u      ///< Position for EEPROM Control Code
#define DBC_IPEC_VELIO_EN_POS                           15u      ///< Position for CAN Transceiver VeLIO enable bit
#define DBC_IPEC_CLK_SEL_POS                            13u      ///< Position for Clock Source Selection bit
#define DBC_IPEC_CLK_FORCE_POS                          12u      ///< Position for Clock Source Override bit
#define DBC_IPEC_CAN_BIAS_POS                           11u      ///< Position for CAN Sleep Mode Bias Voltage
#define DBC_IPEC_CAN_RST_EN_POS                         10u      ///< Position for CAN Reset frame Enable bit
#define DBC_IPEC_DBUS2CAN_POS                            9u      ///< Position for DBus TXD/RXD to CAN transceiver enable bit
#define DBC_IPEC_NWKRQ_DELAY_POS                         8u      ///< Position for nWKRQ delayed de-assertion in sleep mode enable bit
#define DBC_IPEC_CLK_INT_POS                             7u      ///< Position for Clock Status (Internal or External) bit
#define DBC_IPEC_CLK_XTAL_POS                            6u      ///< Position for Clock Crystal Detected bit
#define DBC_IPEC_DBUS_EN_POS                             2u      ///< Position for DBus IP enable bit
#define DBC_IPEC_MCAN_EN_POS                             1u      ///< Position for CAN IP enable bit
#define DBC_IPEC_CCE_POS                                 0u      ///< Position for Configuration Change Enable

/** DCR->EEPP/EEPC - EEPROM Pending / Current register structure (0x0818 /0x081C) */
typedef struct {
    union {
        //! @brief Full register as single 32-bit word
        uint32_t word;
        //! @brief Full register as bitfield
        struct {
            TbitField BVD_TO_NWKRQ : 1;      ///< BVD_TO_NWKRQ:[0]   BVD to nWKRQ pin enable bit
            TbitField MCAN_EN      : 1;      ///< MCAN_EN:[1]        CAN IP enable bit
            TbitField DBUS_EN      : 1;      ///< DBUS_EN:[2]        DBus IP enable bit
            TbitField CAN_BIAS     : 1;      ///< CAN_BIAS:[3]       CAN Sleep Mode Bias Voltage
            TbitField FD_DR        : 1;      ///< FD_DR:[4]          CAN FD bus data rate ratio vs CAN data rate
            TbitField CAN_DR       : 3;      ///< CAN_DR:[7:5]       CAN Bus Data Rate
            TbitField DBUS2CAN     : 1;      ///< DBUS2CAN:[8]       DBus TXD/RXD to CAN transceiver enable bit
            TbitField GP_MEM       : 7;      ///< GP_MEM:[15:9]      General purpose memory bits
            TbitField SUBNODE_ID   : 4;      ///< SUBNODE_ID:[19:16] DBus Subnode ID
            TbitField NODE_ID      : 4;      ///< NODE_ID:[23:20]    DBus Node ID
            TbitField ADV_PWR_MGMT : 1;      ///< ADV_PWR_MGMT:[24]  Advanced Power Management frames enable
            TbitField BVD_THLD     : 1;      ///< BVD_THLD:[25]      Bus Voltage Detector Threshold Voltage
            TbitField BVD_WAIT_EN  : 1;      ///< BVD_WAIT_EN:[26]   Bus Voltage Detector Wait Enable
            TbitField CLKIN        : 2;      ///< CLKIN:[28:27]      Clock Input
            TbitField DBR          : 3;      ///< DBR:[31:29]        DBus Baud Rate
        };
    };
} DBC_EEP_t;

/* DCR->EEPP/EEPC EEPROM Pending / Current register (0x0818) */
/* Definitions of Bit-Masks */
#define DBC_EEP_DBR_MASK                        0xE0000000u      ///< Mask for DBus Baud Rate
#define DBC_EEP_CLKIN_MASK                      0x18000000u      ///< Mask for Clock Input
#define DBC_EEP_BVD_WAIT_EN_MASK                0x04000000u      ///< Mask for Bus Voltage Detector Wait Enable
#define DBC_EEP_BVD_THLD_MASK                   0x02000000u      ///< Mask for Bus Voltage Detector Threshold Voltage
#define DBC_EEP_ADV_PWR_MGMT_MASK               0x01000000u      ///< Mask for Advanced Power Management frames enable
#define DBC_EEP_NODE_ID_MASK                    0x00F00000u      ///< Mask for DBus Node ID
#define DBC_EEP_SUBNODE_ID_MASK                 0x000F0000u      ///< Mask for DBus Subnode ID
#define DBC_EEP_GP_MEM_MASK                     0x0000FE00u      ///< Mask for General purpose memory bits
#define DBC_EEP_DBUS2CAN_MASK                   0x00000100u      ///< Mask for DBus TXD/RXD to CAN transceiver enable bit
#define DBC_EEP_CAN_DR_MASK                     0x000000E0u      ///< Mask for CAN Bus Data Rate
#define DBC_EEP_FD_DR_MASK                      0x00000010u      ///< Mask for CAN FD bus data rate ratio vs CAN data rate
#define DBC_EEP_CAN_BIAS_MASK                   0x00000008u      ///< Mask for CAN Sleep Mode Bias Voltage
#define DBC_EEP_DBUS_EN_MASK                    0x00000004u      ///< Mask for DBus IP enable bit
#define DBC_EEP_MCAN_EN_MASK                    0x00000002u      ///< Mask for CAN IP enable bit
#define DBC_EEP_BVD_TO_NWKRQ_MASK               0x00000001u      ///< Mask for BVD to nWKRQ pin enable bit
#define DBC_EEP_BITS_REG_IPEC_MASK              (DBC_EEP_MCAN_EN_MASK | DBC_EEP_DBUS_EN_MASK | DBC_EEP_DBUS2CAN_MASK | DBC_EEP_CAN_BIAS_MASK) ///< Mask for bits (pulled) from the IPEC register
#define DBC_EEP_BITS_REG_CAN_MASK               (DBC_EEP_CAN_DR_MASK | DBC_EEP_FD_DR_MASK)                                                    ///< Mask for bits (pulled) from the CAN registers
#define DBC_EEP_BITS_REG_DBUS_MASK              (~(DBC_EEP_BITS_REG_IPEC_MASK | DBC_EEP_BITS_REG_CAN_MASK))                                   ///< Mask for bits (pulled) from DBUS registers
#define DBC_EEP_BITS_REG_DBUS_DPC_MASK          (DBC_EEP_ADV_PWR_MGMT_MASK | DBC_EEP_BVD_WAIT_EN_MASK | DBC_EEP_BVD_THLD_MASK | DBC_EEP_BVD_TO_NWKRQ_MASK) ///< Mask for bits (pulled) from the DBUS_DPC register
#define DBC_EEP_BITS_REG_DBUS_SIDFC_MASK        (DBC_EEP_SUBNODE_ID_MASK | DBC_EEP_NODE_ID_MASK)                                            ///< Mask for bits (pulled) from the DBUS_SIDFC register
#define DBC_EEP_BITS_REG_DBUS_DBR_MASK          (DBC_EEP_DBR_MASK | DBC_EEP_CLKIN_MASK)                                                     ///< Mask for bits (pulled) from the DBUS_DBR register
/* Definitions of Bit-Positions */
#define DBC_EEP_GP_MEM_POS                               9u      ///< Position for General purpose memory bits

/* DCR->IF - Interrupts flags (0x0820) */
/* Definitions of Bit-Masks */
#define DBC_IF_EEPROM_ECC_MASK                  0x80000000u      ///< Mask for Uncorrectable customer EEPROM ECC error detected
#define DBC_IF_MODE_SLEEP_MASK                  0x40000000u      ///< Mask for Sleep mode indication
#define DBC_IF_CANHCANL_MASK                    0x20000000u      ///< Mask for CANH and CANL are shorted together
#define DBC_IF_CANHVDD_MASK                     0x10000000u      ///< Mask for CANH shorted to VDD
#define DBC_IF_CANLGND_MASK                     0x08000000u      ///< Mask for CANL shorted to GND
#define DBC_IF_CANBUSOPEN_MASK                  0x04000000u      ///< Mask for CAN bus open
#define DBC_IF_CANBUSGND_MASK                   0x02000000u      ///< Mask for CANH shorted to GND or both CANH & CANL shorted to GND
#define DBC_IF_CANBUSVDD_MASK                   0x01000000u      ///< Mask for CANH shorted to VDD or both CANH and CANL shorted to VDD
#define DBC_IF_SMS_MASK                         0x00800000u      ///< Mask for Sleep Mode Status
#define DBC_IF_UVIO_MASK                        0x00200000u      ///< Mask for Under Voltage VIO
#define DBC_IF_PWRON_MASK                       0x00100000u      ///< Mask for Power On Reset interrupt
#define DBC_IF_TSD_MASK                         0x00080000u      ///< Mask for Thermal Shutdown
#define DBC_IF_BUS_RST_MASK                     0x00040000u      ///< Mask for CAN or DBus reset frame
#define DBC_IF_UVCC_MASK                        0x00020000u      ///< Mask for Under Voltage VCC
#define DBC_IF_ECCERR_MASK                      0x00010000u      ///< Mask for Uncorrectable ECC error detected
#define DBC_IF_CANINT_MASK                      0x00008000u      ///< Mask for Can Bus Wake Up Interrupt
#define DBC_IF_WKERR_MASK                       0x00002000u      ///< Mask for Wake Error
#define DBC_IF_FRAME_OVF_MASK                   0x00001000u      ///< Mask for Selective Wake Frame Overflow
#define DBC_IF_ECCERR_INT_MASK                  0x00000800u      ///< Mask for Uncorrectable Internal EEPROM ECC error detected
#define DBC_IF_CANSLNT_MASK                     0x00000400u      ///< Mask for CAN Silent
#define DBC_IF_CANDOM_MASK                      0x00000100u      ///< Mask for CAN Stuck Dominant
#define DBC_IF_GLOBALERR_MASK                   0x00000080u      ///< Mask for Global Error (Any Fault)
#define DBC_IF_WKRQ_MASK                        0x00000040u      ///< Mask for Wake Request
#define DBC_IF_CANERR_MASK                      0x00000020u      ///< Mask for CAN Error
#define DBC_IF_SPIERR_MASK                      0x00000008u      ///< Mask for SPI Error
#define DBC_IF_SWERR_MASK                       0x00000004u      ///< Mask for Selective Wake Error
#define DBC_IF_DBUS_CAN_MASK                    0x00000002u      ///< Mask for A logical OR of the DBus and CAN global interrupt
#define DBC_IF_VT_MASK                          0x00000001u      ///< Mask for Global Voltage, Temp or ECC erros
#define DBC_IF_GLOBAL_FAULT_FLAG_MASK           0x000000FFu      ///< Mask for Global Read-only Flags
/* Definitions of Bit-Positions */
#define DBC_IF_EEPROM_ECC_POS                           31u      ///< Position for Uncorrectable customer EEPROM ECC error detected
#define DBC_IF_MODE_SLEEP_POS                           30u      ///< Position for Sleep mode indication
#define DBC_IF_CANHCANL_POS                             29u      ///< Position for CANH and CANL are shorted together
#define DBC_IF_CANHVDD_POS                              28u      ///< Position for CANH shorted to VDD
#define DBC_IF_CANLGND_POS                              27u      ///< Position for CANL shorted to GND
#define DBC_IF_CANBUSOPEN_POS                           26u      ///< Position for CAN bus open
#define DBC_IF_CANBUSGND_POS                            25u      ///< Position for CANH shorted to GND or both CANH & CANL shorted to GND
#define DBC_IF_CANBUSVDD_POS                            24u      ///< Position for CANH shorted to VDD or both CANH and CANL shorted to VDD
#define DBC_IF_SMS_POS                                  23u      ///< Position for Sleep Mode Status
#define DBC_IF_UVDD_POS                                 22u      ///< Position for Under Voltage VDD
#define DBC_IF_UVIO_POS                                 21u      ///< Position for Under Voltage VIO
#define DBC_IF_PWRON_POS                                20u      ///< Position for Power On Reset interrupt
#define DBC_IF_TSD_POS                                  19u      ///< Position for Thermal Shutdown
#define DBC_IF_BUS_RST_POS                              18u      ///< Position for CAN or DBus reset frame
#define DBC_IF_UVCC_POS                                 17u      ///< Position for Under Voltage VCC
#define DBC_IF_ECCERR_POS                               16u      ///< Position for Uncorrectable ECC error detected
#define DBC_IF_CANINT_POS                               15u      ///< Position for Can Bus Wake Up Interrupt
#define DBC_IF_WKERR_POS                                13u      ///< Position for Wake Error
#define DBC_IF_FRAME_OVF_POS                            12u      ///< Position for Selective Wake Frame Overflow
#define DBC_IF_ECCERR_INT_POS                           11u      ///< Position for Uncorrectable Internal EEPROM ECC error detected
#define DBC_IF_CANSLNT_POS                              10u      ///< Position for CAN Silent
#define DBC_IF_CANDOM_POS                                8u      ///< Position for CAN Stuck Dominant
#define DBC_IF_GLOBALERR_POS                             7u      ///< Position for Global Error (Any Fault)
#define DBC_IF_WKRQ_POS                                  6u      ///< Position for Wake Request
#define DBC_IF_CANERR_POS                                5u      ///< Position for CAN Error
#define DBC_IF_SPIERR_POS                                3u      ///< Position for SPI Error
#define DBC_IF_SWERR_POS                                 2u      ///< Position for Selective Wake Error
#define DBC_IF_DBUS_CAN_POS                              1u      ///< Position for A logical OR of the DBus and CAN global interrupt
#define DBC_IF_VT_POS                                    0u      ///< Position for Global Voltage, Temp or ECC erros

/* DCR->DBUS_IF (0x0828) */
/* Definitions of Bit-Masks */
// The same masks as for DBUS->IR
/* Definitions of Bit-Positions */
// The same positions as for DBUS->IR


/** DBus address space (0x4000- 0x40FF) */

/* DBC_DBUS->CREL (0x4000) */
#define DBC_DBUS_CREL_CORE_RELEASE_MASK         0xF0000000u      ///< Mask for Core Release
#define DBC_DBUS_CREL_CORE_STEP_MASK            0x0F000000u      ///< Mask for Step of Core Release
#define DBC_DBUS_CREL_CORE_SUB_STEP_MASK        0x00F00000u      ///< Mask for Sub-Step of Core Release
#define DBC_DBUS_CREL_YEAR_MASK                 0x000F0000u      ///< Mask for Time Stamp Year
#define DBC_DBUS_CREL_MONTH_MASK                0x0000FF00u      ///< Mask for Time Stamp Month
#define DBC_DBUS_CREL_DAY_MASK                  0x000000FFu      ///< Mask for Time Stamp Day

/* DBC_DBUS->ENDN (0x4004) */
#define DBC_DBUS_ENDN_VAL                       0x87654321u      ///< DBus Endian register Value

/* DBC_DBUS->TEST (0x4010) */
/* Definitions of Bit-Masks */
#define DBC_DBUS_TEST_PM_FTO_MASK               0x00000200u      ///< Mask for Power Management Fast TimeOut flag
#define DBC_DBUS_TEST_PM_NOTO_MASK              0x00000100u      ///< Mask for Power Management No TimeOut flag
#define DBC_DBUS_TEST_RX_PIN_MASK               0x00000080u      ///< Mask for Rx Pin state monitoring
#define DBC_DBUS_TEST_TX_PIN_MASK               0x00000060u      ///< Mask for Tx Pin state monitoring
#define DBC_DBUS_TEST_TX_PIN_RECESSIVE_MASK     0x00000060u      ///< Mask for Tx Pin recessive level (log. 1)
#define DBC_DBUS_TEST_TX_PIN_DOMINANT_MASK      0x00000040u      ///< Mask for Tx Pin dominant level (log. 0)
#define DBC_DBUS_TEST_LBS_MASK                  0x00000010u      ///< Mask for LoopBack Snoop mode
#define DBC_DBUS_TEST_LBA_MASK                  0x00000008u      ///< Mask for LoopBack Analog mode
#define DBC_DBUS_TEST_LBD_MASK                  0x00000004u      ///< Mask for LoopBack Digital mode
/* Definitions of Bit-Positions */
#define DBC_DBUS_TEST_PM_FTO_POS                         9u      ///< Position for Power Management Fast TimeOut flag
#define DBC_DBUS_TEST_PM_NOTO_POS                        8u      ///< Position for Power Management No TimeOut flag
#define DBC_DBUS_TEST_RX_PIN_POS                         7u      ///< Position for Rx Pin state monitoring
#define DBC_DBUS_TEST_TX_PIN_POS                         5u      ///< Position for Tx Pin state monitoring
#define DBC_DBUS_TEST_LBS_POS                            4u      ///< Position for LoopBack Snoop mode
#define DBC_DBUS_TEST_LBA_POS                            3u      ///< Position for LoopBack Analog mode
#define DBC_DBUS_TEST_LBD_POS                            2u      ///< Position for LoopBack Digital mode

/* DBC_DBUS->CCCR (0x4018) */
/* Definitions of Bit-Masks */
#define DBC_DBUS_CCCR_EDGE_FLT_MASK             0x00002000u      ///< Mask for Edge Filtering Enable
#define DBC_DBUS_CCCR_TEST_MODE_EN_MASK         0x00000080u      ///< Mask for Test Mode Enable
#define DBC_DBUS_CCCR_DAR_MASK                  0x00000040u      ///< Mask for Disable Automatic Retransmission
#define DBC_DBUS_CCCR_TIDLE_CONST_DIS_MASK      0x00000020u      ///< Mask for Fixed Idle Time Disable
#define DBC_DBUS_CCCR_CCE_MASK                  0x00000002u      ///< Mask for Configuration Change Enable
#define DBC_DBUS_CCCR_INIT_MASK                 0x00000001u      ///< Mask for Initialization mode
/* Definitions of Bit-Positions */
#define DBC_DBUS_CCCR_EDGE_FLT_POS                      13u      ///< Position for Edge Filtering Enable
#define DBC_DBUS_CCCR_TEST_MODE_EN_POS                   7u      ///< Position for Test Mode Enable
#define DBC_DBUS_CCCR_DAR_POS                            6u      ///< Position for Disable Automatic Retransmission
#define DBC_DBUS_CCCR_TIDLE_CONST_DIS_POS                5u      ///< Position for Fixed Idle Time Disable
#define DBC_DBUS_CCCR_CCE_POS                            1u      ///< Position for Configuration Change Enable
#define DBC_DBUS_CCCR_INIT_POS                           0u      ///< Position for Initialization mode

/* DBC_DBUS->TSCC (0x4020) */
/* Definitions of Bit-Masks */
#define DBC_DBUS_TSCC_TCP_MASK                  0x000F0000u      ///< Mask for Timestamp Counter Prescaler
#define DBC_DBUS_TSCC_TS_MASK                   0x00000001u      ///< Mask for Timestamp Select
/* Definitions of Bit-Positions */
#define DBC_DBUS_TSCC_TCP_POS                           16u      ///< Position for Timestamp Counter Prescaler
#define DBC_DBUS_TSCC_TS_POS                             0u      ///< Position for Timestamp Select

/* DBC_DBUS->TSCV (0x4024) */
#define DBC_DBUS_TSCV_TC_MASK                   0x0000FFFFu      ///< Mask for Timestamp Counter value

/* DBC_DBUS->TOCC (0x4028) */
/* Definitions of Bit-Masks */
#define DBC_DBUS_TOCC_TOP_MASK                  0xFFFF0000u      ///< Mask for TimeOut Period
#define DBC_DBUS_TOCC_TOS_MASK                  0x00000006u      ///< Mask for TimeOut Select
#define DBC_DBUS_TOCC_TOCE_MASK                 0x00000001u      ///< Mask for TimeOut Counter Enable
/* Definitions of Bit-Positions */
#define DBC_DBUS_TOCC_TOP_POS                           16u      ///< Position for TimeOut Period
#define DBC_DBUS_TOCC_TOS_POS                            1u      ///< Position for TimeOut Select
#define DBC_DBUS_TOCC_TOCE_POS                           0u      ///< Position for TimeOut Counter Enable

/* DBC_DBUS->TOCV (0x402C) */
#define DBC_DBUS_TOCV_TOCV_MASK                 0x0000FFFFu      ///< Mask for TimeOut Counter Value

/* DBC_DBUS->RXC (0x4034) */
/* Definitions of Bit-Masks */
#define DBC_DBUS_RXC_RX_MODE_MASK               0x01000000u      ///< Mask for Receive Mode
#define DBC_DBUS_RXC_RX_SAMPLE_OFFSET_MASK      0x00030000u      ///< Mask for Receive Sample Offset
#define DBC_DBUS_RXC_IGNORE_CRC_MASK            0x00000100u      ///< Mask for Ignore CRC for received frames
#define DBC_DBUS_RXC_ACCEPT_NON_MATCH_MASK      0x00000004u      ///< Mask for Accept Non-matching Frames
#define DBC_DBUS_RXC_RX_FLTR_MASK               0x00000003u      ///< Mask for Receive Filter
#define DBC_DBUS_RXC_RX_FLTR_W_ACK_OK           0x00000002u      ///< Definition for Receive Filter value- receive addressed and acknowledged frames
#define DBC_DBUS_RXC_RX_FLTR_WO_ACK_OK          0x00000001u      ///< Definition for Receive Filter value- receive addressed but not acknowledged frames
/* Definitions of Bit-Positions */
#define DBC_DBUS_RXC_RX_MODE_POS                        24u      ///< Position for Receive Mode
#define DBC_DBUS_RXC_RX_SAMPLE_OFFSET_POS               16u      ///< Position for Receive Sample Offset
#define DBC_DBUS_RXC_IGNORE_CRC_POS                      8u      ///< Position for Ignore CRC for received frames
#define DBC_DBUS_RXC_ACCEPT_NON_MATCH_POS                2u      ///< Position for Accept Non-matching Frames
#define DBC_DBUS_RXC_RX_FLTR_POS                         0u      ///< Position for Receive Filter

/* DBC_DBUS->PSR (0x4044) */
/* Definitions of Bit-Masks */
#define DBC_DBUS_PSR_PEE_MASK                   0x00004000u      ///< Mask for Protocol Exception Event flag
#define DBC_DBUS_PSR_LEC_MASK                   0x00000007u      ///< Mask for type of Last Error Code
/* Definitions of Bit-Positions */
#define DBC_DBUS_PSR_PEE_POS                            14u      ///< Position for Protocol Exception Event flag
#define DBC_DBUS_PSR_LEC_POS                             0u      ///< Position for type of Last Error Code

/* DBC_DBUS_DBR (0x404C) */
/* Definitions of Bit-Masks */
#define DBC_DBUS_DBR_CB_EN_MASK                 0x80000000u      ///< Mask for Custom Baud rate divider Enable
#define DBC_DBUS_DBR_CB_INT_MASK                0x0FFF0000u      ///< Mask for Integer portion of the Custom Baud rate divisor
#define DBC_DBUS_DBR_CB_FRAC_MASK               0x0000FF00u      ///< Mask for Fractional portion of the Custom Baud rate divisor
#define DBC_DBUS_DBR_CLKIN_MASK                 0x00000018u      ///< Mask for Clock Input
#define DBC_DBUS_DBR_DBR_MASK                   0x00000007u      ///< Mask for DBus Baud Rate
/* Definitions of Bit-Positions */
#define DBC_DBUS_DBR_CB_EN_POS                          31u      ///< Position for Custom Baud rate divider Enable
#define DBC_DBUS_DBR_CB_INT_POS                         16u      ///< Position for Integer portion of the Custom Baud rate divisor
#define DBC_DBUS_DBR_CB_FRAC_POS                         8u      ///< Position for Fractional portion of the Custom Baud rate divisor
#define DBC_DBUS_DBR_CLKIN_POS                           3u      ///< Position for Clock Input
#define DBC_DBUS_DBR_DBR_POS                             0u      ///< Position for DBus Baud Rate

/* DBUS->IR (0x4050) */
/* Definitions of Bit-Masks */
#define DBC_DBUS_IR_ALL_BIT_MASK                0xF335DE0Fu      ///< Mask for (clearing) all interrupts of DBUS_IR register
#define DBC_DBUS_IR_WK_EVENT_MASK               0x80000000u      ///< Mask for Wake Event
#define DBC_DBUS_IR_WUP_MASK                    0x40000000u      ///< Mask for Wake Up Pulse Detected
#define DBC_DBUS_IR_ARA_MASK                    0x20000000u      ///< Mask for Access to a reserved address status bit
#define DBC_DBUS_IR_DBUSSLNT_MASK               0x10000000u      ///< Mask for DBus has been silent for tSILENCE amount of time
#define DBC_DBUS_IR_BVD_MASK                    0x02000000u      ///< Mask for BVD Interrupt
#define DBC_DBUS_IR_BVD_FAIL_MASK               0x01000000u      ///< Mask for BVD Fail from a TX Wake Pulse
#define DBC_DBUS_IR_BEU_MASK                    0x00200000u      ///< Mask for Bit Error Uncorrected
#define DBC_DBUS_IR_BEC_MASK                    0x00100000u      ///< Mask for Bit Error Corrected
#define DBC_DBUS_IR_TOO_MASK                    0x00040000u      ///< Mask for Timeout Occurred
#define DBC_DBUS_IR_TSW_MASK                    0x00010000u      ///< Mask for Timestamp Wraparound
#define DBC_DBUS_IR_TEFL_MASK                   0x00008000u      ///< Mask for Tx Status FIFO Element Lost
#define DBC_DBUS_IR_TEFF_MASK                   0x00004000u      ///< Mask for Tx Status FIFO Full
#define DBC_DBUS_IR_TEFN_MASK                   0x00001000u      ///< Mask for Tx Status FIFO New Entry
#define DBC_DBUS_IR_TFE_MASK                    0x00000800u      ///< Mask for Tx FIFO Empty
#define DBC_DBUS_IR_TFL_MASK                    0x00000400u      ///< Mask for Tx FIFO Message Lost
#define DBC_DBUS_IR_TC_MASK                     0x00000200u      ///< Mask for Transmission Completed
#define DBC_DBUS_IR_RF0L_MASK                   0x00000008u      ///< Mask for Rx FIFO Message Lost
#define DBC_DBUS_IR_RF0F_MASK                   0x00000004u      ///< Mask for Rx FIFO Full
#define DBC_DBUS_IR_RBLFE_MASK                  0x00000002u      ///< Mask for Rx Bootloader Framing Error
#define DBC_DBUS_IR_RF0N_MASK                   0x00000001u      ///< Mask for Rx FIFO New Message
/* Definitions of Bit-Positions */
#define DBC_DBUS_IR_WK_EVENT_POS                        31u      ///< Position for Wake Event
#define DBC_DBUS_IR_WUP_POS                             30u      ///< Position for Wake Up Pulse Detected
#define DBC_DBUS_IR_ARA_POS                             29u      ///< Position for Access to a reserved address status bit
#define DBC_DBUS_IR_DBUSSLNT_POS                        28u      ///< Position for DBus has been silent for tSILENCE amount of time
#define DBC_DBUS_IR_BVD_POS                             25u      ///< Position for BVD Interrupt
#define DBC_DBUS_IR_BVD_FAIL_POS                        24u      ///< Position for BVD Fail from a TX Wake Pulse
#define DBC_DBUS_IR_BEU_POS                             21u      ///< Position for Bit Error Uncorrected
#define DBC_DBUS_IR_BEC_POS                             20u      ///< Position for Bit Error Corrected
#define DBC_DBUS_IR_TOO_POS                             18u      ///< Position for Timeout Occurred
#define DBC_DBUS_IR_TSW_POS                             16u      ///< Position for Timestamp Wraparound
#define DBC_DBUS_IR_TEFL_POS                            15u      ///< Position for Tx Status FIFO Element Lost
#define DBC_DBUS_IR_TEFF_POS                            14u      ///< Position for Tx Status FIFO Full
#define DBC_DBUS_IR_TEFN_POS                            12u      ///< Position for Tx Status FIFO New Entry
#define DBC_DBUS_IR_TFE_POS                             11u      ///< Position for Tx FIFO Empty
#define DBC_DBUS_IR_TFL_POS                             10u      ///< Position for Tx FIFO Message Lost
#define DBC_DBUS_IR_TC_POS                               9u      ///< Position for Transmission Completed
#define DBC_DBUS_IR_RF0L_POS                             3u      ///< Position for Rx FIFO Message Lost
#define DBC_DBUS_IR_RF0F_POS                             2u      ///< Position for Rx FIFO Full
#define DBC_DBUS_IR_RBLFE_POS                            1u      ///< Position for Rx Bootloader Framing Error
#define DBC_DBUS_IR_RF0N_POS                             0u      ///< Position for Rx FIFO New Message

/* DBC_DBUS->IE (0x4054) */
/* Definitions of Bit-Masks */
#define DBC_DBUS_IE_ALL_BIT_MASK                0xF035DE0Fu      ///< Mask for (enabling) all interrupts of DBUS_IE register
#define DBC_DBUS_IE_WK_EVENT_EN_MASK            0x80000000u      ///< Mask for Wake Event Enable
#define DBC_DBUS_IE_WUP_EN_MASK                 0x40000000u      ///< Mask for Wake Up Pulse Interrupt Enable
#define DBC_DBUS_IE_ARA_EN_MASK                 0x20000000u      ///< Mask for Access to Reserved Address Event Enable
#define DBC_DBUS_IE_DBUSSLNT_EN_MASK            0x10000000u      ///< Mask for DBus Silent Flag Enable
#define DBC_DBUS_IE_BEU_EN_MASK                 0x00200000u      ///< Mask for Bit Error Uncorrected Event Enable
#define DBC_DBUS_IE_BEC_EN_MASK                 0x00100000u      ///< Mask for Bit Error Corrected Event Enable
#define DBC_DBUS_IE_TOO_EN_MASK                 0x00040000u      ///< Mask for Timeout Occurred
#define DBC_DBUS_IE_TSW_EN_MASK                 0x00010000u      ///< Mask for Timestamp Wraparound Event Enable
#define DBC_DBUS_IE_TEFL_EN_MASK                0x00008000u      ///< Mask for Tx Status FIFO Element Lost Event Enable
#define DBC_DBUS_IE_TEFF_EN_MASK                0x00004000u      ///< Mask for Tx Status FIFO Full Event Enable
#define DBC_DBUS_IE_TEFN_EN_MASK                0x00001000u      ///< Mask for Tx Status FIFO New Entry Event Enable
#define DBC_DBUS_IE_TFE_EN_MASK                 0x00000800u      ///< Mask for Tx FIFO Empty Event Enable
#define DBC_DBUS_IE_TFL_EN_MASK                 0x00000400u      ///< Mask for Tx FIFO Message Lost Event Enable
#define DBC_DBUS_IE_TC_EN_MASK                  0x00000200u      ///< Mask for Transmission Completed Event Enable
#define DBC_DBUS_IE_RF0L_EN_MASK                0x00000008u      ///< Mask for Rx FIFO Message Lost Event Enable
#define DBC_DBUS_IE_RF0F_EN_MASK                0x00000004u      ///< Mask for Rx FIFO Full Event Enable
#define DBC_DBUS_IE_RBLFE_EN_MASK               0x00000002u      ///< Mask for Rx Bootloader Framing Error Event Enable
#define DBC_DBUS_IE_RF0N_EN_MASK                0x00000001u      ///< Mask for Rx FIFO FIFO New Message Event Enable
/* Definitions of Bit-Positions */
#define DBC_DBUS_IE_WK_EVENT_EN_POS                     31u      ///< Position for Wake Event Enable
#define DBC_DBUS_IE_WUP_EN_POS                          30u      ///< Position for Wake Up Pulse Interrupt Enable
#define DBC_DBUS_IE_ARA_EN_POS                          29u      ///< Position for Access to a reserved Address Event Enable
#define DBC_DBUS_IE_DBUSSLNT_EN_POS                     28u      ///< Position for DBus silent Event Enable
#define DBC_DBUS_IE_BVD_EN_POS                          25u      ///< Position for BVD Interrupt Enable
#define DBC_DBUS_IE_BVD_FAIL_EN_POS                     24u      ///< Position for BVD Fail from a TX Wake Pulse Event Enable
#define DBC_DBUS_IE_BEU_EN_POS                          21u      ///< Position for Bit Error Uncorrected Event Enable
#define DBC_DBUS_IE_BEC_EN_POS                          20u      ///< Position for Bit Error Corrected Event Enable
#define DBC_DBUS_IE_TOO_EN_POS                          18u      ///< Position for Timeout Occurred Event Enable
#define DBC_DBUS_IE_TSW_EN_POS                          16u      ///< Position for Timestamp Wraparound Event Enable
#define DBC_DBUS_IE_TEFL_EN_POS                         15u      ///< Position for Tx Status FIFO Element Lost Event Enable
#define DBC_DBUS_IE_TEFF_EN_POS                         14u      ///< Position for Tx Status FIFO Full Event Enable
#define DBC_DBUS_IE_TEFN_EN_POS                         12u      ///< Position for Tx Status FIFO New Entry Event Enable
#define DBC_DBUS_IE_TFE_EN_POS                          11u      ///< Position for Tx FIFO Empty Event Enable
#define DBC_DBUS_IE_TFL_EN_POS                          10u      ///< Position for Tx FIFO Message Lost Event Enable
#define DBC_DBUS_IE_TC_EN_POS                            9u      ///< Position for Transmission Completed Event Enable
#define DBC_DBUS_IE_RF0L_EN_POS                          3u      ///< Position for Rx FIFO Message Lost Event Enable
#define DBC_DBUS_IE_RF0F_EN_POS                          2u      ///< Position for Rx FIFO Full Event Enable
#define DBC_DBUS_IE_RBLFE_EN_POS                         1u      ///< Position for Rx Bootloader Framing Error Event Enable
#define DBC_DBUS_IE_RF0N_EN_POS                          0u      ///< Position for Rx FIFO New Message Event Enable

/* DBC_DBUS->BSA (0x4060) */
#define DBC_DBUS_BSA_RAM_ADDR_MASK              0x00003FFFu      ///< Mask for base RAM Address

/* DBC_DBUS->BSC0 (0x4064) */
/* Definitions of Bit-Masks */
#define DBC_DBUS_BSC0_RX_BUF_SIZE_MASK          0x0FFF0000u      ///< Mask for RX Buffer Size
#define DBC_DBUS_BSC0_TX_BUF_SIZE_MASK          0x00000FFFu      ///< Mask for TX Buffer Size
/* Definitions of Bit-Positions */
#define DBC_DBUS_BSC0_RX_BUF_SIZE_POS                   16u      ///< Position for RX Buffer Size
#define DBC_DBUS_BSC0_TX_BUF_SIZE_POS                    0u      ///< Position for TX Buffer Size

/* DBC_DBUS->BSC1 (0x4064) */
/* Definitions of Bit-Masks */
#define DBC_DBUS_BSC0_TXS_BUF_SIZE_MASK         0x0000007Fu      ///< Mask for Tx Status Buffer Size

/* DBC_DBUS->BCC (0x406C) */
/* Definitions of Bit-Masks */
#define DBC_DBUS_BCC_CSR_RST_MASK               0x00000200u      ///< Mask for DBus CSR protocol Reset
#define DBC_DBUS_BCC_RST_MASK                   0x00000100u      ///< Mask for DBus protocol Reset
#define DBC_DBUS_BCC_RXFIFO_CLR_MASK            0x00000004u      ///< Mask for RX Buffer Clear
#define DBC_DBUS_BCC_TXFIFO_CLR_MASK            0x00000002u      ///< Mask for TX Buffer Clear
#define DBC_DBUS_BCC_TXSFIFO_CLR_MASK           0x00000001u      ///< Mask for TX Status Buffer Clear
/* Definitions of Bit-Positions */
#define DBC_DBUS_BCC_CSR_RST_POS                         9u      ///< Position for DBus CSR protocol Reset
#define DBC_DBUS_BCC_RST_POS                             8u      ///< Position for DBus protocol Reset
#define DBC_DBUS_BCC_RXFIFO_CLR_POS                      2u      ///< Position for RX Buffer Clear
#define DBC_DBUS_BCC_TXFIFO_CLR_POS                      1u      ///< Position for TX Buffer Clear
#define DBC_DBUS_BCC_TXSFIFO_CLR_POS                     0u      ///< Position for TX Status Buffer Clear

/* DBUS->DPA (0x4070) */
/* Definitions of Bit-Masks */
#define DBC_DBUS_DPA_TX_WK_PULSE_MASK           0x00000001u      ///< Mask for Transmit Wake Pulse
/* Definitions of Bit-Positions */
#define DBC_DBUS_DPA_TX_WK_PULSE_POS                     0u      ///< Position for Transmit Wake Pulse

/* DBUS->DPC (0x4074) */
/* Definitions of Bit-Masks */
#define DBC_DBUS_DPC_BVD_TO_NWKRQ_MASK          0x04000000u      ///<  Mask for BVD to nWKRQ Pin
#define DBC_DBUS_DPC_BVD_THLD_MASK              0x02000000u      ///<  Mask for Bus Voltage Detector Threshold Voltage
#define DBC_DBUS_DPC_BVD_WAIT_EN_MASK           0x01000000u      ///<  Mask for Bus Voltage Detector Wait Enable
#define DBC_DBUS_DPC_ADV_PWR_MGMT_MASK          0x00010000u      ///<  Mask for Advanced Power Management
#define DBC_DBUS_DPC_WK_FM_NU_MASK              0x00000002u      ///<  Mask for Wakeup Frame Not Used
/* Definitions of Bit-Positions */
#define DBC_DBUS_DPC_BVD_TO_NWKRQ_POS                   26u      ///<  Position for BVD to nWKRQ Pin
#define DBC_DBUS_DPC_BVD_THLD_POS                       25u      ///<  Position for Bus Voltage Detector Threshold Voltage
#define DBC_DBUS_DPC_BVD_WAIT_EN_POS                    24u      ///<  Position for Bus Voltage Detector Wait Enable
#define DBC_DBUS_DPC_ADV_PWR_MGMT_POS                   16u      ///<  Position for Advanced Power Management
#define DBC_DBUS_DPC_WK_FM_NU_POS                        1u      ///<  Position for Wakeup Frame Not Used

/* DBUS->DPS (0x4078) */
/* Definitions of Bit-Masks */
#define DBC_DBUS_DPS_BVD_FULL_PWR_MASK          0x00010000u      ///< Mask for BVD is at full power (above the BVD threshold)
#define DBC_DBUS_DPS_DP_CS_MASK                 0x00000C00u      ///< Mask for The current power state
#define DBC_DBUS_DPS_RX_WK_CS_MASK              0x00000300u      ///< Mask for The current state of RX Wake
#define DBC_DBUS_DPS_WPC_MASK                   0x00000070u      ///< Mask for Wake Pulse Counter
#define DBC_DBUS_DPS_TX_WK_CS_MASK              0x00000007u      ///< Mask for the Current State of TX Wake
/* Definitions of Bit-Positions */
#define DBC_DBUS_DPS_BVD_FULL_PWR_POS                   16u      ///< Position for BVD is at full power (above the BVD threshold)
#define DBC_DBUS_DPS_DP_CS_POS                          10u      ///< Position for The current power state
#define DBC_DBUS_DPS_RX_WK_CS_POS                        8u      ///< Position for The current state of RX Wake
#define DBC_DBUS_DPS_WPC_POS                             4u      ///< Position for Wake Pulse Counter
#define DBC_DBUS_DPS_TX_WK_CS_POS                        0u      ///< Position for the Current State of TX Wake

/* DBUS->SIDFC (0x4084) */
/* Definitions of Bit-Masks */
#define DBC_DBUS_SIDFC_ACK_EN_MASK              0x40000000u      ///< Mask for Acknowledge Enable for a message that matches this filter (PID and SID value)
#define DBC_DBUS_SIDFC_LSS_MASK                 0x000F0000u      ///< Mask for List Size Standard - Number of standard Node filters
#define DBC_DBUS_SIDFC_PID_MASK                 0x000000F0u      ///< Mask for Partner ID for this node
#define DBC_DBUS_SIDFC_SID_MASK                 0x0000000Fu      ///< Mask for Subnode ID for this node
/* Definitions of Bit-Positions */
#define DBC_DBUS_SIDFC_ACK_EN_POS                       30u      ///< Position for Acknowledge Enable for a message that matches this filter (PID and SID value)
#define DBC_DBUS_SIDFC_LSS_POS                          16u      ///< Position for List Size Standard - Number of standard Node filters
#define DBC_DBUS_SIDFC_PID_POS                           4u      ///< Position for Partner ID for this node
#define DBC_DBUS_SIDFC_SID_POS                           0u      ///< Position for Subnode ID for this node

/* DBC_DBUS->RXF0S (0x40A4) */
/* Definitions of Bit-Masks */
#define DBC_DBUS_RXF0S_RF0L_MASK                0x02000000u      ///< Mask for Message Lost flag- mirror of DBC_DBUS_IR_RF0L
#define DBC_DBUS_RXF0S_RF0F_MASK                0x01000000u      ///< Mask for Rx FIFO/Queue Full flag
#define DBC_DBUS_RXF0S_RF0MS_MASK               0x0007FF00u      ///< Mask for Rx FIFO message size in words (message at the top of FIFO)
#define DBC_DBUS_RXF0S_RF0FL_MASK               0x0000000Fu      ///< Mask for Rx FIFO Fill Level- range 0 to 8
/* Definitions of Bit-Positions */
#define DBC_DBUS_RXF0S_RF0L_POS                         25u      ///< Position for Message Lost flag- mirror of DBC_DBUS_IR_RF0L
#define DBC_DBUS_RXF0S_RF0F_POS                         24u      ///< Position for Rx FIFO/Queue Full flag
#define DBC_DBUS_RXF0S_RF0MS_POS                         8u      ///< Position for Rx FIFO message size in words (message at the top of FIFO)
#define DBC_DBUS_RXF0S_RF0FL_POS                         0u      ///< Position for Rx FIFO Fill Level- range 0 to 8

/* DBC_DBUS->TXFQS (0x40C4) */
/* Definitions of Bit-Masks */
#define DBC_DBUS_TXFQS_TFL_MASK                 0x00400000u      ///< Mask for Tx FIFO message Lost flag- mirror of DBC_DBUS_IR_TFL
#define DBC_DBUS_TXFQS_TFF_MASK                 0x00200000u      ///< Mask for Tx FIFO Full flag
#define DBC_DBUS_TXFQS_TFDA_MASK                0x000FFF00u      ///< Mask for Tx FIFO Data Available (number of bytes in Tx FIFO)
#define DBC_DBUS_TXFQS_TFFL_MASK                0x0000000Fu      ///< Mask for Tx FIFO Free Level- range 0 to 8
/* Definitions of Bit-Positions */
#define DBC_DBUS_TXFQS_TFL_POS                          22u      ///< Position for Tx FIFO message Lost flag- mirror of DBC_DBUS_IR_TFL
#define DBC_DBUS_TXFQS_TFF_POS                          21u      ///< Position for Tx FIFO Full flag
#define DBC_DBUS_TXFQS_TFDA_POS                          8u      ///< Position for Tx FIFO Data Available (number of bytes  in Tx FIFO)
#define DBC_DBUS_TXFQS_TFFL_POS                          0u      ///< Position for Tx FIFO Free Level- range 0 to 8

/* DBC_DBUS->TXBTIE (0x40E0) */
#define DBC_DBUS_TXBTIE_TIE_MASK                0x00000001u      ///< Mask for Transmission Interupt Enable

/* DBC_DBUS->TXEFS (0x40F4) */
#define DBC_DBUS_TXEFS_TEFFL_MAX                         8u      ///< Tx Event (status) FIFO Fill Level is at maximum
/* Definitions of Bit-Masks */
#define DBC_DBUS_TXEFS_TEFL_MASK                0x02000000u      ///< Mask for Tx Event (status) FIFO Element Lost flag- mirror of DBC_DBUS_IR_TEFL
#define DBC_DBUS_TXEFS_TEFF_MASK                0x01000000u      ///< Mask for Tx Event (status) FIFO Full flag - mirror of DBC_DBUS_IR_TEFF
#define DBC_DBUS_TXEFS_TEFFL_MASK               0x0000000Fu      ///< Mask for Tx Event (status) FIFO Fill Level- value range 0-8
/* Definitions of Bit-Positions */
#define DBC_DBUS_TXEFS_TEFL_POS                         25u      ///< Position for Tx Event (status) FIFO Element Lost flag- mirror of DBC_DBUS_IR_TEFL
#define DBC_DBUS_TXEFS_TEFF_POS                         24u      ///< Position for Tx Event (status) FIFO Full flag - mirror of DBC_DBUS_IR_TEFF
#define DBC_DBUS_TXEFS_TEFFL_POS                         0u      ///< Position for Tx Event (status) FIFO Fill Level- range 0 to 8

/* DBus Node Filter Register (0x4200- 0x423B) */
#define DBC_DBUS_NF_COUNT        (DBC_DBUS_SIDFC_LSS_MASK >> DBC_DBUS_SIDFC_LSS_POS) ///< Number of DBus node ID filters available in chip
/* Definitions of Bit-Masks */
#define DBC_DBUS_NF_FLT_VALID_MASK              0x80000000u      ///< Mask for Valid filter word
#define DBC_DBUS_NF_ACK_EN_MASK                 0x40000000u      ///< Mask for Acknowledge enable for a message that matches this filter
#define DBC_DBUS_NF_PID_MASK                    0x0F000000u      ///< Mask for Partner ID to filter received message for
#define DBC_DBUS_NF_SUBSYS_ALL_MASK             0x0000FFFFu      ///< Mask for Accept/Reject received message for All subsystems
#define DBC_DBUS_NF_SUBSYS15_MASK               0x00008000u      ///< Mask for Accept/Reject received message for Subsystem 15
#define DBC_DBUS_NF_SUBSYS14_MASK               0x00004000u      ///< Mask for Accept/Reject received message for Subsystem 14
#define DBC_DBUS_NF_SUBSYS13_MASK               0x00002000u      ///< Mask for Accept/Reject received message for Subsystem 13
#define DBC_DBUS_NF_SUBSYS12_MASK               0x00001000u      ///< Mask for Accept/Reject received message for Subsystem 12
#define DBC_DBUS_NF_SUBSYS11_MASK               0x00000800u      ///< Mask for Accept/Reject received message for Subsystem 11
#define DBC_DBUS_NF_SUBSYS10_MASK               0x00000400u      ///< Mask for Accept/Reject received message for Subsystem 10
#define DBC_DBUS_NF_SUBSYS9_MASK                0x00000200u      ///< Mask for Accept/Reject received message for Subsystem 9
#define DBC_DBUS_NF_SUBSYS8_MASK                0x00000100u      ///< Mask for Accept/Reject received message for Subsystem 8
#define DBC_DBUS_NF_SUBSYS7_MASK                0x00000080u      ///< Mask for Accept/Reject received message for Subsystem 7
#define DBC_DBUS_NF_SUBSYS6_MASK                0x00000040u      ///< Mask for Accept/Reject received message for Subsystem 6
#define DBC_DBUS_NF_SUBSYS5_MASK                0x00000020u      ///< Mask for Accept/Reject received message for Subsystem 5
#define DBC_DBUS_NF_SUBSYS4_MASK                0x00000010u      ///< Mask for Accept/Reject received message for Subsystem 4
#define DBC_DBUS_NF_SUBSYS3_MASK                0x00000008u      ///< Mask for Accept/Reject received message for Subsystem 3
#define DBC_DBUS_NF_SUBSYS2_MASK                0x00000004u      ///< Mask for Accept/Reject received message for Subsystem 2
#define DBC_DBUS_NF_SUBSYS1_MASK                0x00000002u      ///< Mask for Accept/Reject received message for Subsystem 1
#define DBC_DBUS_NF_SUBSYS0_MASK                0x00000001u      ///< Mask for Accept/Reject received message for Subsystem 0
#define DBC_DBUS_NF_ALL_SUBSYS_EN_MASK          (DBC_DBUS_NF_FLT_VALID_MASK | DBC_DBUS_NF_ACK_EN_MASK | DBC_DBUS_NF_SUBSYS_ALL_MASK) ///< Mask for configuration of activated node filter to receive messages for all subsystems and to send acknowledge
/* Definitions of Bit-Positions */
#define DBC_DBUS_NF_FLT_VALID_POS                       31u      ///< Position for Valid filter word
#define DBC_DBUS_NF_ACK_EN_POS                          30u      ///< Position for Acknowledge enable for a message that matches this filter
#define DBC_DBUS_NF_PID_POS                             24u      ///< Position for Partner ID to filter received message for
#define DBC_DBUS_NF_SUBSYS15_POS                        15u      ///< Position for Accept/Reject received message for Subsystem 15
#define DBC_DBUS_NF_SUBSYS14_POS                        14u      ///< Position for Accept/Reject received message for Subsystem 14
#define DBC_DBUS_NF_SUBSYS13_POS                        13u      ///< Position for Accept/Reject received message for Subsystem 13
#define DBC_DBUS_NF_SUBSYS12_POS                        12u      ///< Position for Accept/Reject received message for Subsystem 12
#define DBC_DBUS_NF_SUBSYS11_POS                        11u      ///< Position for Accept/Reject received message for Subsystem 11
#define DBC_DBUS_NF_SUBSYS10_POS                        10u      ///< Position for Accept/Reject received message for Subsystem 10
#define DBC_DBUS_NF_SUBSYS9_POS                          9u      ///< Position for Accept/Reject received message for Subsystem 9
#define DBC_DBUS_NF_SUBSYS8_POS                          8u      ///< Position for Accept/Reject received message for Subsystem 8
#define DBC_DBUS_NF_SUBSYS7_POS                          7u      ///< Position for Accept/Reject received message for Subsystem 7
#define DBC_DBUS_NF_SUBSYS6_POS                          6u      ///< Position for Accept/Reject received message for Subsystem 6
#define DBC_DBUS_NF_SUBSYS5_POS                          5u      ///< Position for Accept/Reject received message for Subsystem 5
#define DBC_DBUS_NF_SUBSYS4_POS                          4u      ///< Position for Accept/Reject received message for Subsystem 4
#define DBC_DBUS_NF_SUBSYS3_POS                          3u      ///< Position for Accept/Reject received message for Subsystem 3
#define DBC_DBUS_NF_SUBSYS2_POS                          2u      ///< Position for Accept/Reject received message for Subsystem 2
#define DBC_DBUS_NF_SUBSYS1_POS                          1u      ///< Position for Accept/Reject received message for Subsystem 1
#define DBC_DBUS_NF_SUBSYS0_POS                          0u      ///< Position for Accept/Reject received message for Subsystem 0

/** DBus Transmit Status FIFO register structure (0x4300) */
typedef union
{
    //! @brief Full register as a bitfield
    struct _statusField
    {
        /* Word 0 */
        uint16_t TX_TS;                    ///< Time stamp when message was transmitted
        uint8_t  FRAME_LEN;                ///< Frame length
        uint8_t  TARGET_ADDR;              ///< Target address
        /* Word 1 */
        uint8_t  TX_BYTES;                 ///< The number of bytes transmitted
        uint8_t  ACK;                      ///< The raw acknowledge byte sent or received
        //! @brief Acknowledge status field structure
        PACKED struct
        {
            bool ackRcvd          : 1; ///< Acknowledge received
            bool ackFramingError  : 1; ///< Acknowledge framing error
            bool noAckRcvd        : 1; ///< No acknowledge received
            bool                  : 1; ///< Reserved bit
            bool ackSent          : 1; ///< Acknowledge sent,
            bool ackSentCollision : 1; ///< Acknowledge sent, collision
            bool                  : 1; ///< Reserved bit
            bool                  : 1; ///< Reserved bit
        } ACK_STATUS;
        //! @brief Frame Status field structure
        PACKED struct
        {
            bool succFrameTx      : 1; ///< Successful frame transmit
            bool unsuccFrameTx    : 1; ///< Unsuccessful frame transmit
            bool frameCollision   : 1; ///< Frame collision
            bool lastTxAttempt    : 1; ///< Last Transmit Attempt
            bool reTxCollision    : 1; ///< Retransmission, collision
            bool reTxAckBusy      : 1; ///< Retransmission, ACK_BUSY
            bool broadcast        : 1; ///< Broadcast
            bool                  : 1; ///< Reserved bit
        } FRAME_STATUS;
    } statusField;                         ///< Full register as a bitfield
    //! @brief Full register as an array
    uint8_t statusArray[sizeof(struct _statusField)];
} DBC_DBUS_TXSF_t;

#define DBC_DBUS_TXSF_SIZE    ((uint16_t)sizeof(DBC_DBUS_TXSF_t))    ///< The number of bytes in DBus Transmit Status FIFO buffer


/** DBus BootLoader Transmit Status FIFO register structure (0x4300) */
typedef struct
{
    TbitField FRAME_LEN    : 13;  /* Frame Length [12:0]         Address offset: 0x4300 */
    TbitField RESERVED1    : 19;  /* Reserved [31:13]                                   */
    TbitField TX_BYTES     : 13;  /* Transmitted Bytes [12:0]    Address offset: 0x4304 */
    TbitField RESERVED2    : 11;  /* Reserved [23:13]                                   */
    TbitField FRAME_STATUS : 8;   /* Frame Status [31:24]        Address offset: 0x4307 */
} DBC_DBUS_BL_TXSF_t;


/** Structure of DBus Transmit FIFO buffer overhead data (0x4400) */
typedef PACKED struct
{
    TbitField NUM_RTRY    : 4;   /* Number of Retries [3:0]    Address offset: 0x4400 */
    TbitField RESERVED1   : 4;   /* Reserved [7:4]                                    */
    TbitField CRC         : 1;   /* CRC in buffer [8]          Address offset: 0x4401 */
    TbitField RESERVED2   : 23;  /* Reserved [31:9]                                   */
/* TX frame */
    uint8_t   MSG_LEN;           /* Message Length             Address offset: 0x4404 */
    uint8_t   TARGET_ADDR;       /* Target Address             Address offset: 0x4405 */
//    uint16_t MSG_ID;           /* Message ID                 Address offset: 0x4406 */
//    uint8_t DATA[N];           /* Message Data               Address offset: 0x4408 */
//    uint8_t CRC[2];            /* CRC                        Address offset: dependent on DATA length */
} DBC_DBUS_TXF_t;

#define DBC_DBUS_CRC_SIZE                 (2u)                          ///< The number of bytes of DBus message CRC
#define DBC_DBUS_TXF_HDR_SIZE             (6u)                          ///< The number of bytes of overhead data in DBus Transmit FIFO buffer (@c DBC_DBUS_TXF_t)
#define DBC_DBUS_TXF_MSG_ID_HIGH           DBC_DBUS_TXF_HDR_SIZE        ///< Byte offset of DBus message ID high byte in Tx FIFO
#define DBC_DBUS_TXF_MSG_ID_LOW           (DBC_DBUS_TXF_HDR_SIZE + 1u)  ///< Byte offset of DBus message ID low byte in Tx FIFO
#define DBC_DBUS_TXF_DATA_OFFSET          (DBC_DBUS_TXF_HDR_SIZE + 2u)  ///< Byte offset of DBus message data in Tx FIFO
#define DBC_DBUS_TXF_CRC_IN_BUFFER        (1u)                          ///< CRC is put into Tx FIFO together with DBus frame data
#define DBC_DBUS_TXF_CRC_HW_CALCULATED    (0u)                          ///< CRC over DBus frame is calculated/added by the DBusCAN chip


/** Structure of DBus BootLoader Transmit FIFO buffer overhead data (0x4400) */
typedef PACKED struct
{
    TbitField MSG_LEN : 13;      /* Message Length [12:0]     Address offset: 0x4400 */
    TbitField RSVD    : 19;      /* Reserved [31:13]                                 */
//    uint8_t DATA[N];           /* Data                      Address offset: 0x4404 */
} DBC_DBUS_BL_TXF_t;

#define DBC_DBUS_BL_TXF_HDR_SIZE          (4u)  ///< The number of bytes of overhead data in DBus BootLoader Transmit FIFO buffer (@c DBC_DBUS_BL_TXF_t)
#define DBC_DBUS_BL_TXF_DATA_OFFSET       DBC_DBUS_BL_TXF_HDR_SIZE  ///< Offset of data in DBus BootLoader Transmit FIFO buffer (@c DBC_DBUS_BL_TXF_t)


/** Structure of DBus Receive FIFO buffer overhead data (0x4400) */
typedef PACKED union
{
    //! @brief Full register as a bitfield
    PACKED struct _rxfField
    {
        /* Word 0 */
        uint16_t RX_TS;                     ///< Received frame timestamp
        uint8_t  FRAME_LEN;                 ///< Frame length - Valid values 4, 6-255. (ie. RX frame: Message length + 4). Should be the same as RX_BYTES unless an error occured
        uint8_t  TARGET_ADDR;               ///< Target address of received frame
        /* Word 1 */
        uint8_t RX_BYTES;                   ///< Number of bytes received. Should be the same as FRAME_LEN unless an error occured
        uint8_t ACK;                        ///< Acknowledge value received
        //! @brief Acknowledge status field structure
        PACKED struct
        {
            bool ackRcvd           : 1; ///< Acknowledge received
            bool ackFramingError   : 1; ///< Acknowledge framing error
            bool noAckRcvd         : 1; ///< No acknowledge received
            bool                   : 1; ///< Reserved bit
            bool ackSent           : 1; ///< Acknowledge sent
            bool ackSentCollision  : 1; ///< Acknowledge sent, collision
            bool                   : 1; ///< Reserved bit
            bool                   : 1; ///< Reserved bit
        } ACK_STATUS;
        //! @brief Frame status field structure
        PACKED struct
        {
            bool succFrameRx       : 1; ///< Successful frame receieve
            bool unsuccFrameRx     : 1; ///< Unsuccessful frame receieve
            bool framingError      : 1; ///< Framing error
            bool crcError          : 1; ///< CRC error
            bool lengthError       : 1; ///< Length error
            bool frameMatch        : 1; ///< Frame match
            bool frameNonMatching  : 1; ///< Frame non-matching
            bool broadcast         : 1; ///< Broadcast
        } FRAME_STATUS;
        /* Word 2 */
        uint8_t  MSG_LEN;                  ///< The number of data bytes received. Valid values 2-251
        uint8_t  PID_SID;                  ///< Partner ID/subsystem ID target of frame
//   uint16_t MSG_ID;                      // Message ID of received message
//    uint8_t DATA[N];                     // Message Data
//    uint8_t CRC[2];                      // Message CRC
    } rxfField;                            ///< Full register as a bitfield
    //! @brief Full register as an array
    uint8_t rxfArray[sizeof(struct _rxfField)];
} DBC_DBUS_RXF_t;

#define DBC_DBUS_RXF_HDR_SIZE             ((uint16_t)sizeof(DBC_DBUS_RXF_t))  ///< The number of bytes of overhead data in DBus Receive FIFO buffer (@c DBC_DBUS_RXF_t)


/** CAN address space (0x1000- 0x10FF) */

/**
 * @brief DBC_CAN_DBTP: CAN Data Bit Timing & Prescaler register structure (0x100C)
 * Used to setup the data timing parameters of the CAN module
 */
typedef struct {
    union {
        //! @brief Full register as single 32-bit word
        uint32_t word;
        struct {
            //! @brief DSJW: Data (Re)Synchronization Jump Width. Value interpreted as 1:x
            TbitField DSJW   : 4;
            //! @brief DTSEG2: Data time Segment after sample point
            TbitField DTSEG2 : 4;
            //! @brief DTSEG1: Data time Segment before sample point
            TbitField DTSEG1 : 5;
            //! Reserved
            TbitField        : 3;
            //! @brief DBRP: Data Bit Rate Prescaler, interpreted as 1:x
            TbitField DBRP   : 5;
            //! Reserved
            TbitField        : 2;
            //! @brief TDC: Transmitter Delay Compensation
            TbitField TDC    : 1;
            //! Reserved
            TbitField        : 8;
        };
    };
} DBC_CAN_DBTP_t;

/* Definitions of Bit-Masks */
#define DBC_CAN_DBTP_TDC_MASK                   0x00800000u    ///< Mask for Transmitter Delay Compensation flag
#define DBC_CAN_DBTP_DBRP_MASK                  0x001F0000u    ///< Mask for Data Bit Rate Prescaler
#define DBC_CAN_DBTP_DTSEG1_MASK                0x00001F00u    ///< Mask for Data time Segment before sample point
#define DBC_CAN_DBTP_DTSEG2_MASK                0x000000F0u    ///< Mask for Data time Segment after sample point
#define DBC_CAN_DBTP_DSJW_MASK                  0x0000000Fu    ///< Mask for Data (Re)Synchronization Jump Width


/**
 * @brief DBC_CAN_CCCR: CAN Control Register structure (0x1018)
 */
typedef struct {
    union {
        //! @brief Full register as single 32-bit word
        uint32_t word;
        struct {
            //! @brief INIT: Initialization
            TbitField INIT : 1;
            //! @brief CCE: Configuration Change Enable for whole CAN register space
            TbitField CCE : 1;
            //! @brief ASM: Restricted Operation Mode. The device can only listen to CAN traffic and acknowledge, but not send anything.
            TbitField ASM : 1;
            //! @brief CSA: Clock Stop Acknowledge
            TbitField CSA : 1;
            //! @brief CSR: Clock Stop Request
            TbitField CSR : 1;
            //! @brief MON: Bus Monitoring mode. The device may only listen to CAN traffic, and is not allowed to acknowledge or send error frames.
            TbitField MON : 1;
            //! @brief DAR: Disable Automatic Retransmission. If a transmission errors, gets a NACK, or loses arbitration, the CAN controller will NOT try to transmit again
            TbitField DAR : 1;
            //! @brief TEST: CAN Test mode enable
            TbitField TEST : 1;
            //! @brief FDOE: CAN FD mode Enabled, master enable for CAN FD support
            TbitField FDOE : 1;
            //! @brief BRSE: Bit Rate Switch Enabled for CAN FD. Master enable for bit rate switching support
            TbitField BRSE : 1;
            //! @brief Reserved
            TbitField      : 2;
            //! @brief PXHD: Protocol exception handling disable
            //! \n 0 = Protocol exception handling enabled [default]
            //! \n 1 = protocol exception handling disabled
            TbitField PXHD : 1;
            //! @brief EFBI: Edge Filtering during Bus Integration. 0 Disables this [default]
            TbitField EFBI : 1;
            //! @brief TXP: Transmitter Pause: Pause for 2 can bit times before next transmission
            TbitField TXP  : 1;
            //! @brief NISO: Non ISO Operation
            //! \n 0: CAN FD frame format according to ISO 11898-1:2015 [default]
            //! \n 1: CAN FD frame format according to Bosch CAN FD Spec v1
            TbitField NISO : 1;
            //! Reserved
            TbitField      : 16;
        };
    };
} DBC_CAN_CCCR_t;

/* Definitions of Bit-Masks */
#define DBC_CAN_CCCR_NISO_MASK                  0x00008000u    ///< Mask for Non ISO Operation flag
#define DBC_CAN_CCCR_TXP_MASK                   0x00004000u    ///< Mask for Transmitter Pause flag
#define DBC_CAN_CCCR_EFBI_MASK                  0x00002000u    ///< Mask for Edge Filtering during Bus Integration flag
#define DBC_CAN_CCCR_PXHD_MASK                  0x00001000u    ///< Mask for Protocol exception handling disable flag
#define DBC_CAN_CCCR_BRSE_MASK                  0x00000200u    ///< Mask for Bit Rate Switch Enabled for CAN FD flag
#define DBC_CAN_CCCR_FDOE_MASK                  0x00000100u    ///< Mask for CAN FD mode Enabled flag
#define DBC_CAN_CCCR_TEST_MASK                  0x00000080u    ///< Mask for CAN Test mode enable flag
#define DBC_CAN_CCCR_DAR_MASK                   0x00000040u    ///< Mask for Disable Automatic Retransmission flag
#define DBC_CAN_CCCR_MON_MASK                   0x00000020u    ///< Mask for Bus Monitoring mode flag
#define DBC_CAN_CCCR_CSR_MASK                   0x00000010u    ///< Mask for Clock Stop Request flag
#define DBC_CAN_CCCR_CSA_MASK                   0x00000008u    ///< Mask for Clock Stop Acknowledge flag
#define DBC_CAN_CCCR_ASM_MASK                   0x00000004u    ///< Mask for Restricted Operation Mode flag
#define DBC_CAN_CCCR_CCE_MASK                   0x00000002u    ///< Mask for Configuration Change Enable flag
#define DBC_CAN_CCCR_INIT_MASK                  0x00000001u    ///< Mask for Initialization flag


/**
 * @brief DBC_CAN_NBTP: CAN Nominal Bit Timing & Prescaler register structure (0x101C)
 * Used to setup the nominal timing parameters of the CAN module
 */
typedef struct {
    union {
        //! @brief Full register as single 32-bit word
        uint32_t word;
        struct {
            //! @brief NTSEG2: Nominal Time Segment after Sample point
            //!\n Valid values are: 1 to 127
            TbitField NTSEG2 : 7;
            //! Reserved
            TbitField        : 1;
            //! @brief NTSEG1: Nominal Time Segment before sample point
            //! \n Valid values are: 1 to 255
            TbitField NTSEG1 : 8;
            //! @brief NBRP: Nominal Bit Rate Prescaler. Value interpreted as 1:x
            //! \n Valid range is: 0 to 511
            TbitField NBRP   : 9;
            //! @brief NSJW: Nominal (Re)Synchronization Jump Width. Value interpreted as 1:x
            //! \n Valid range is: 0 to 127
            TbitField NSJW   : 7;
        };
    };
} DBC_CAN_NBTP_t;

/* Definitions of Bit-Masks */
#define DBC_CAN_NBTP_NSJW_MASK                  0xFE000000u    ///< Mask for Nominal (Re)Synchronization Jump Width
#define DBC_CAN_NBTP_NBRP_MASK                  0x01FF0000u    ///< Mask for Nominal Bit Rate Prescaler
#define DBC_CAN_NBTP_NTSEG1_MASK                0x0000FF00u    ///< Mask for Nominal Time Segment before Sample point
#define DBC_CAN_NBTP_NTSEG2_MASK                0x0000007Fu    ///< Mask for Nominal Time Segment after Sample point


/* CAN TSCC - CAN TimeStamp Counter Configuration register (0x1020) */
/* Definitions of Bit-Masks */
#define DBC_CAN_TSCC_TCP_MASK                   0x000F0000u    ///< Mask for Timestamp Counter Prescaler
#define DBC_CAN_TSSC_TSS_MASK                   0x00000003u    ///< Mask for Timestamp Select
/* Definitions of Bit-Positions */
#define DBC_CAN_TSCC_TCP_POS                            16u    ///< Position for Timestamp Counter Prescaler
#define DBC_CAN_TSCC_TSS_POS                             0u    ///< Position for Timestamp Select
/* Defines of valid values of TSS bit of TSCC register */
#define DBC_CAN_TSCC_TSS_COUNTER_ALWAYS_0                0u    ///< Timestamp Counter is always zero
#define DBC_CAN_TSCC_TSS_COUNTER_USE_TCP                 1u    ///< Use Timestamp Counter Prescaler for counting time stamp
#define DBC_CAN_TSCC_TSS_COUNTER_EXTERNAL                2u    ///< Use External counter for counting time stamp


/**
 * @brief DBC_CAN_TDCR: CAN Transmitter Delay Compensation Register structure (0x1048)
 * Used to setup the timing parameters of the CAN module
 */
typedef struct {
    union {
        //! @brief Full register as single 32-bit word
        uint32_t word;
        struct {
            //! @brief TDCF: Transmitter Delay Compensation Filter window length
            //! \n Valid values are 0 to 127 mtq
            TbitField TDCF : 7;
            //! Reserved
            TbitField      : 1;
            //! @brief TDCO: Transmitter Delay Compensation Offset
            //! \n Valid values are 0 to 127 mtq
            TbitField TDCO : 7;
            //! Reserved
            TbitField      : 17;
        };
    };
} DBC_CAN_TDCR_t;

/* Definitions of Bit-Masks */
#define DBC_CAN_TDCR_TDCO_MASK                  0x00007F00u    ///< Mask for Transmitter Delay Compensation Offset
#define DBC_CAN_TDCR_TDCF_MASK                  0x0000007Fu    ///< Mask for Transmitter Delay Compensation Filter
/* Definitions of Bit-Positions */
#define DBC_CAN_TDCR_TDCO_POS                           16u    ///< Position for Transmitter Delay Compensation Offset
#define DBC_CAN_TDCR_TDCF_POS                            0u    ///< Position for Transmitter Delay Compensation Filter


/**
 * @brief DBC_CAN_IR: CAN Interrupt Register structure (0x1050)
 */
typedef struct {
    union {
        //! @brief Full register as single 32-bit word
        uint32_t word;
        struct {
            //! @brief IR[0] RF0N: Rx FIFO 0 new message
            TbitField RF0N : 1;
            //! @brief IR[1] RF0W: Rx FIFO 0 watermark reached
            TbitField RF0W : 1;
            //! @brief IR[2] RF0F: Rx FIFO 0 full
            TbitField RF0F : 1;
            //! @brief IR[3] RF0L: Rx FIFO 0 message lost
            TbitField RF0L : 1;
            //! @brief IR[4] RF1N: Rx FIFO 1 new message
            TbitField RF1N : 1;
            //! @brief IR[5]  RF1W: RX FIFO 1 watermark reached
            TbitField RF1W : 1;
            //! @brief IR[6] RF1F: Rx FIFO 1 full
            TbitField RF1F : 1;
            //! @brief IR[7] RF1L: Rx FIFO 1 message lost
            TbitField RF1L : 1;
            //! @brief IR[8] HPM: High priority message
            TbitField HPM : 1;
            //! @brief IR[9] TC: Transmission completed
            TbitField TC : 1;
            //! @brief IR[10] TCF: Transmission cancellation finished
            TbitField TCF : 1;
            //! @brief IR[11] TFE: Tx FIFO Empty
            TbitField TFE : 1;
            //! @brief IR[12] TEFN: Tx Event FIFO new entry
            TbitField TEFN : 1;
            //! @brief IR[13] TEFW: Tx Event FIFO water mark reached
            TbitField TEFW : 1;
            //! @brief IR[14] TEFF: Tx Event FIFO full
            TbitField TEFF : 1;
            //! @brief IR[15] TEFL: Tx Event FIFO element lost
            TbitField TEFL : 1;
            //! @brief IR[16] TSW: Timestamp wrapped around
            TbitField TSW : 1;
            //! @brief IR[17] MRAF: Message RAM access failure
            TbitField MRAF : 1;
            //! @brief IR[18] TOO: Time out occurred
            TbitField TOO : 1;
            //! @brief IR[19] DRX: Message stored to dedicated RX buffer
            TbitField DRX : 1;
            //! @brief IR[20] BEC: RAM Bit error corrected
            TbitField BEC : 1;
            //! @brief IR[21] BEU: RAM Bit error uncorrected
            TbitField BEU : 1;
            //! @brief IR[22] ELO: Error logging overflow
            TbitField ELO : 1;
            //! @brief IR[23] EP: Error_passive status changed
            TbitField EP : 1;
            //! @brief IR[24] EW: Error_warning status changed
            TbitField EW : 1;
            //! @brief IR[25] BO: Bus_off status changed
            TbitField BO : 1;
            //! @brief IR[26] WDI: RAM Watchdog Interrupt
            TbitField WDI : 1;
            //! @brief IR[27] PEA: Protocol Error in arbitration phase (nominal bit time used)
            TbitField PEA : 1;
            //! @brief IR[28] PED: Protocol error in data phase (data bit time is used)
            TbitField PED : 1;
            //! @brief IR[29] ARA: Access to reserved address
            TbitField ARA : 1;
            //! @brief IR[30:31] Reserved, not writable
            TbitField : 2;
        };
    };
} DBC_CAN_IR_t;

/* Definitions of Bit-Masks */
#define DBC_CAN_IR_ARA_MASK                     0x20000000u    ///< Mask for Access to reserved address flag
#define DBC_CAN_IR_PED_MASK                     0x10000000u    ///< Mask for Protocol error in data phase flag
#define DBC_CAN_IR_PEA_MASK                     0x08000000u    ///< Mask for Protocol Error in arbitration phase flag
#define DBC_CAN_IR_WDI_MASK                     0x04000000u    ///< Mask for RAM Watchdog Interrupt flag
#define DBC_CAN_IR_BO_MASK                      0x02000000u    ///< Mask for Bus_off status changed flag
#define DBC_CAN_IR_EW_MASK                      0x01000000u    ///< Mask for Error_warning status changed flag
#define DBC_CAN_IR_EP_MASK                      0x00800000u    ///< Mask for Error_passive status changed flag
#define DBC_CAN_IR_ELO_MASK                     0x00400000u    ///< Mask for Error_logging overflow flag
#define DBC_CAN_IR_BEU_MASK                     0x00200000u    ///< Mask for RAM Bit error uncorrected flag
#define DBC_CAN_IR_BEC_MASK                     0x00100000u    ///< Mask for RAM Bit error Corrected flag
#define DBC_CAN_IR_DRX_MASK                     0x00080000u    ///< Mask for Message stored to dedicated RX buffer flag
#define DBC_CAN_IR_TOO_MASK                     0x00040000u    ///< Mask for Time out occurred flag
#define DBC_CAN_IR_MRAF_MASK                    0x00020000u    ///< Mask for Message RAM access failure flag
#define DBC_CAN_IR_TSW_MASK                     0x00010000u    ///< Mask for Timestamp wrapped around flag
#define DBC_CAN_IR_TEFL_MASK                    0x00008000u    ///< Mask for Tx Event FIFO element lost flag
#define DBC_CAN_IR_TEFF_MASK                    0x00004000u    ///< Mask for Tx Event FIFO element full flag
#define DBC_CAN_IR_TEFW_MASK                    0x00002000u    ///< Mask for Tx Event FIFO water mark reached flag
#define DBC_CAN_IR_TEFN_MASK                    0x00001000u    ///< Mask for Tx Event FIFO new entry flag
#define DBC_CAN_IR_TFE_MASK                     0x00000800u    ///< Mask for Tx FIFO Empty flag
#define DBC_CAN_IR_TCF_MASK                     0x00000400u    ///< Mask for Transmission cancellation finished flag
#define DBC_CAN_IR_TC_MASK                      0x00000200u    ///< Mask for Transmission completed flag
#define DBC_CAN_IR_HPM_MASK                     0x00000100u    ///< Mask for High priority message flag
#define DBC_CAN_IR_RF1L_MASK                    0x00000080u    ///< Mask for Rx FIFO 1 message lost flag
#define DBC_CAN_IR_RF1F_MASK                    0x00000040u    ///< Mask for Rx FIFO 1 full flag
#define DBC_CAN_IR_RF1W_MASK                    0x00000020u    ///< Mask for RX FIFO 1 watermark reached flag
#define DBC_CAN_IR_RF1N_MASK                    0x00000010u    ///< Mask for Rx FIFO 1 new message flag
#define DBC_CAN_IR_RF0L_MASK                    0x00000008u    ///< Mask for Rx FIFO 0 message lost flag
#define DBC_CAN_IR_RF0F_MASK                    0x00000004u    ///< Mask for Rx FIFO 0 full flag
#define DBC_CAN_IR_RF0W_MASK                    0x00000002u    ///< Mask for RX FIFO 0 watermark reached flag
#define DBC_CAN_IR_RF0N_MASK                    0x00000001u    ///< Mask for Rx FIFO 0 new message flag


/**
 * @brief DBC_CAN_IE: CAN Interrupt Enable register structure (0x1054)
 */
typedef struct {
    union {
        //! Full register as single 32-bit word
        uint32_t word;
        struct {
            //! @brief IE[0] RF0NE: Rx FIFO 0 new message
            TbitField RF0NE : 1;
            //! @brief IE[1] RF0WE: Rx FIFO 0 watermark reached
            TbitField RF0WE : 1;
            //! @brief IE[2] RF0FE: Rx FIFO 0 full
            TbitField RF0FE : 1;
            //! @brief IE[3] RF0LE: Rx FIFO 0 message lost
            TbitField RF0LE : 1;
            //! @brief IE[4] RF1NE: Rx FIFO 1 new message
            TbitField RF1NE : 1;
            //! @brief IE[5]  RF1WE: RX FIFO 1 watermark reached
            TbitField RF1WE : 1;
            //! @brief IE[6] RF1FE: Rx FIFO 1 full
            TbitField RF1FE : 1;
            //! @brief IE[7] RF1LE: Rx FIFO 1 message lost
            TbitField RF1LE : 1;
            //! @brief IE[8] HPME: High priority message
            TbitField HPME : 1;
            //! @brief IE[9] TCE: Transmission completed
            TbitField TCE : 1;
            //! @brief IE[10] TCFE: Transmission cancellation finished
            TbitField TCFE : 1;
            //! @brief IE[11] TFEE: Tx FIFO Empty
            TbitField TFEE : 1;
            //! @brief IE[12] TEFNE: Tx Event FIFO new entry
            TbitField TEFNE : 1;
            //! @brief IE[13] TEFWE Tx Event FIFO watermark reached
            TbitField TEFW : 1;
            //! @brief IE[14] TEFFE: Tx Event FIFO full
            TbitField TEFFE : 1;
            //! @brief IE[15] TEFLE: Tx Event FIFO element lost
            TbitField TEFLE : 1;
            //! @brief IE[16] TSWE: Timestamp wraparound
            TbitField TSWE : 1;
            //! @brief IE[17] MRAFE: Message RAM access failure
            TbitField MRAFE : 1;
            //! @brief IE[18] TOOE: Time out occured
            TbitField TOOE : 1;
            //! @brief IE[19] DRXE: Message stored to dedicated RX buffer
            TbitField DRXE : 1;
            //! @brief IE[20] BECE: RAM Bit error corrected
            TbitField BECE : 1;
            //! @brief IE[21] BEUE: RAM Bit error uncorrected
            TbitField BEUE : 1;
            //! @brief IE[22] ELOE: Error logging overflow
            TbitField ELOE : 1;
            //! @brief IE[23] EPE: Error_passive status changed
            TbitField EPE : 1;
            //! @brief IE[24] EWE: Error_warning status changed
            TbitField EWE : 1;
            //! @brief IE[25] BOE: Bus_off status changed
            TbitField BOE : 1;
            //! @brief IE[26] WDIE: RAM Watchdog Interrupt
            TbitField WDIE : 1;
            //! @brief IE[27] PEAE: Protocol Error in arbitration phase (nominal bit time used)
            TbitField PEAE : 1;
            //! @brief IE[28] PEDE: Protocol error in data phase (data bit time is used)
            TbitField PEDE : 1;
            //! @brief IE[29] ARAE: Access to reserved address
            TbitField ARAE : 1;
            //! @brief IE[30:31] Reserved
            TbitField      : 2;
        };
    };
} DBC_CAN_IE_t;

/* Definitions of Bit-Masks */
#define DBC_CAN_IE_ARAE_MASK                    0x20000000u    ///< Mask for Access to reserved address Enable flag
#define DBC_CAN_IE_PEDE_MASK                    0x10000000u    ///< Mask for Protocol error in data phase Enable flag
#define DBC_CAN_IE_PEAE_MASK                    0x08000000u    ///< Mask for Protocol Error in arbitration phase Enable flag
#define DBC_CAN_IE_WDIE_MASK                    0x04000000u    ///< Mask for  RAM Watchdog Interrupt Enable flag
#define DBC_CAN_IE_BOE_MASK                     0x02000000u    ///< Mask for Bus_off status changed Enable flag
#define DBC_CAN_IE_EWE_MASK                     0x01000000u    ///< Mask for Error_warning status changed Enable flag
#define DBC_CAN_IE_EPE_MASK                     0x00800000u    ///< Mask for Error_passive status changed Enable flag
#define DBC_CAN_IE_ELOE_MASK                    0x00400000u    ///< Mask for Error_logging overflow Enable flag
#define DBC_CAN_IE_BEUE_MASK                    0x00200000u    ///< Mask for RAM Bit error uncorrected Enable flag
#define DBC_CAN_IE_BECE_MASK                    0x00100000u    ///< Mask for RAM Bit error Corrected Enable flag
#define DBC_CAN_IE_DRXE_MASK                    0x00080000u    ///< Mask for Message stored to dedicated RX buffer Enable flag
#define DBC_CAN_IE_TOOE_MASK                    0x00040000u    ///< Mask for Time out occurred Enable flag
#define DBC_CAN_IE_MRAFE_MASK                   0x00020000u    ///< Mask for Message RAM access failure Enable flag
#define DBC_CAN_IE_TSWE_MASK                    0x00010000u    ///< Mask for Timestamp wrapped around Enable flag
#define DBC_CAN_IE_TEFLE_MASK                   0x00008000u    ///< Mask for Tx Event FIFO message lost Enable flag
#define DBC_CAN_IE_TEFFE_MASK                   0x00004000u    ///< Mask for Tx Event FIFO full Enable flag
#define DBC_CAN_IE_TEFWE_MASK                   0x00002000u    ///< Mask for Tx Event FIFO watermark reached Enable flag
#define DBC_CAN_IE_TEFNE_MASK                   0x00001000u    ///< Mask for Tx Event FIFO new entry Enable flag
#define DBC_CAN_IE_TFEE_MASK                    0x00000800u    ///< Mask for Tx Event FIFO empty Enable flag
#define DBC_CAN_IE_TCFE_MASK                    0x00000400u    ///< Mask for Transmission Cancellation Finished Enable flag
#define DBC_CAN_IE_TCE_MASK                     0x00000200u    ///< Mask for Transmission Completed Enable flag
#define DBC_CAN_IE_HPME_MASK                    0x00000100u    ///< Mask for High priority message Enable flag
#define DBC_CAN_IE_RF1LE_MASK                   0x00000080u    ///< Mask for Rx FIFO 1 message lost Enable flag
#define DBC_CAN_IE_RF1FE_MASK                   0x00000040u    ///< Mask for Rx FIFO 1 full Enable flag
#define DBC_CAN_IE_RF1WE_MASK                   0x00000020u    ///< Mask for Rx FIFO 1 watermark reached Enable flag
#define DBC_CAN_IE_RF1NE_MASK                   0x00000010u    ///< Mask for Rx FIFO 1 new message Enable flag
#define DBC_CAN_IE_RF0LE_MASK                   0x00000008u    ///< Mask for RX FIFO 0 message lost Enable flag
#define DBC_CAN_IE_RF0FE_MASK                   0x00000004u    ///< Mask for RX FIFO 0 full Enable flag
#define DBC_CAN_IE_RF0WE_MASK                   0x00000002u    ///< Mask for RX FIFO 0 watermark reached Enable flag
#define DBC_CAN_IE_RF0NE_MASK                   0x00000001u    ///< Mask for Rx FIFO 0 new message Enable flag


/* CAN ILE - CAN Interrupt Line Enable register (0x105C) */
/* Definitions of Bit-Masks */
#define DBC_CAN_ILE_EINT1_MASK                  0x00000002u    ///< Mask for Enable Interrupt Line 1
#define DBC_CAN_ILE_EINT0_MASK                  0x00000001u    ///< Mask for Enable Interrupt Line 0


/**
 * @brief Defines of valid values for ANFS and ANFE bitfields of the @c DBC_CAN_GFC_t struct
 * */
enum DBC_CAN_GFC_NoMatchBehaviour {
    DBC_CAN_GFC_ACCEPT_INTO_RXFIFO0 = 0u,
    DBC_CAN_GFC_ACCEPT_INTO_RXFIFO1 = 1u,
    DBC_CAN_GFC_REJECT              = 2u
};

/**
 * @brief DBC_CAN_GFC:CAN Global Filter Configuration register structure (0x1080)
 */
typedef struct {
    union {
        //! Full word of register
        uint32_t word;
        struct {
            //! @brief GFC[0] RRFE: Reject Remote Frames for Extended IDs
            TbitField RRFE : 1;
            //! @brief GFC[1] RRFS: Reject Remote Frames for Standard IDs
            TbitField RRFS : 1;
            //! @brief GFC[3:2] ANFE: Accept Non-matching Frames Extended
            //! Valid values:
            //! DBC_CAN_GFC_ANF_ACCEPT_INTO_RXFIFO0 : Accept into RXFIFO0
            //! DBC_CAN_GFC_ANF_ACCEPT_INTO_RXFIFO1 : Accept into RXFIFO1
            //! DBC_CAN_GFC_ANF_REJECT              : Reject
            enum DBC_CAN_GFC_NoMatchBehaviour ANFE : 2;
            //! @brief GFC[5:4] ANFS: Accept Non-matching Frames Standard
            //! Valid values:
            //! DBC_CAN_GFC_ANF_ACCEPT_INTO_RXFIFO0 : Accept into RXFIFO0
            //! DBC_CAN_GFC_ANF_ACCEPT_INTO_RXFIFO1 : Accept into RXFIFO1
            //! DBC_CAN_GFC_ANF_REJECT              : Reject
            enum DBC_CAN_GFC_NoMatchBehaviour ANFS : 2;
            //! @brief Reserved
            TbitField : 26;
        };
    };
} DBC_CAN_GFC_t;

/* Definitions of Bit-Masks */
#define DBC_CAN_GFC_ANFS_MASK                   0x00000030u    ///< Mask for Accept Non-matching Frames Standard
#define DBC_CAN_GFC_ANFE_MASK                   0x0000000Cu    ///< Mask for Accept Non-matching Frames Extended
#define DBC_CAN_GFC_RRFS_MASK                   0x00000002u    ///< Mask for flag Reject Remote Frames for Standard IDs
#define DBC_CAN_GFC_RRFE_MASK                   0x00000001u    ///< Mask for flag Reject Remote Frames for Extended IDs


/**
 * @brief DBC_CAN_SIDFC: Standard ID Filter Configuration struct (0x1084)
 */
typedef struct {
    union {
        //! full register as single 32-bit word
        uint32_t word;
        struct {
            //! FLSSA:[15:0] Filter List Standard Start Address
            TbitField FLSSA : 16;
            //! @brief LSS:[23:16] List Size Standard
            TbitField LSS : 8;
            //! @brief reserved[31:24]
            TbitField : 8;
        };
    };
} DBC_CAN_SIDFC_t;

/* Definitions of Bit-Masks */
#define DBC_CAN_SIDFC_LSS_MASK                  0x00FF0000u    ///< Mask for List Size Standard
#define DBC_CAN_SIDFC_FLSSA_MASK                0x0000FFFFu    ///< Mask for Filter List Standard Start Address
/* Definitions of Bit-Positions */
#define DBC_CAN_SIDFC_LSS_POS                           16u    ///< Position for List Size Standard
#define DBC_CAN_SIDFC_FLSSA_POS                          0u    ///< Position for Filter List Standard Start Address


/**
 * @brief DBC_CAN_XIDFC: Extended ID Filter Configuration struct (0x1088)
 */
typedef struct {
    union {
        //! full register as single 32-bit word
        uint32_t word;
        struct {
            //! FLSEA:[15:0] Filter List Extended Start Address
            TbitField FLSEA : 16;
            //! @brief LSE:[22:16] List Size Extended
            TbitField LSE : 7;
            //! @brief Reserved[31:23]
            TbitField : 9;
        };
    };
} DBC_CAN_XIDFC_t;

/* Definitions of Bit-Masks */
#define DBC_CAN_XIDFC_LSE_MASK                  0x007F0000u    ///< Mask for Filter List Extended Start Address
#define DBC_CAN_XIDFC_FLSEA_MASK                0x0000FFFFu    ///< Mask for List Size Extended
/* Definitions of Bit-Positions */
#define DBC_CAN_XIDFC_LSE_POS                           16u    ///< Position for List Size Extended
#define DBC_CAN_XIDFC_FLSEA_POS                          0u    ///< Position for Filter List Extended Start Address

/**
 * @brief Enumeration of CAN Receive FIFO buffers available in chip
 */
typedef enum {
    DBC_CAN_RXFIFO0,
    DBC_CAN_RXFIFO1
} DBC_CanRxFifo_t;

/* DBC_CAN_RXF0C: Rx FIFO 0 Configuration register (0x10A0) */
/* Definitions of Bit-Masks */
#define DBC_CAN_RXF0C_F0OM_MASK                 0x80000000u    ///< Mask for Rx FIFO 0 Operation Mode
#define DBC_CAN_RXF0C_F0WM_MASK                 0x7F000000u    ///< Mask for Rx FIFO 0 Watermark
#define DBC_CAN_RXF0C_F0S_MASK                  0x007F0000u    ///< Mask for Rx FIFO 0 Size
#define DBC_CAN_RXF0C_F0SA_MASK                 0x0000FFFFu    ///< Mask for Rx FIFO 0 Start Address
/* Definitions of Bit-Positions */
#define DBC_CAN_RXF0C_F0OM_POS                          31u    ///< Position for Rx FIFO 0 Operation Mode
#define DBC_CAN_RXF0C_F0WM_POS                          24u    ///< Position for Rx FIFO 0 Watermark
#define DBC_CAN_RXF0C_F0S_POS                           16u    ///< Position for Rx FIFO 0 Size
#define DBC_CAN_RXF0C_F0SA_POS                           0u    ///< Position for Rx FIFO 0 Start Address


/* DBC_CAN_RXF0S: Rx FIFO 0 Status register (0x10A4) */
/* Definitions of Bit-Masks */
#define DBC_CAN_RXF0S_RF0L_MASK                 0x02000000u    ///< Mask for Rx FIFO 0 Message Lost
#define DBC_CAN_RXF0S_F0F_MASK                  0x01000000u    ///< Mask for Rx FIFO 0 Full
#define DBC_CAN_RXF0S_F0PI_MASK                 0x003F0000u    ///< Mask for Rx FIFO 0 Put Index
#define DBC_CAN_RXF0S_F0GI_MASK                 0x00003F00u    ///< Mask for Rx FIFO 0 Get Index
#define DBC_CAN_RXF0S_F0FL_MASK                 0x0000007Fu    ///< Mask for Rx FIFO 0 Fill Level
/* Definitions of Bit-Positions */
#define DBC_CAN_RXF0S_RF0L_POS                          25u    ///< Position for Rx FIFO 0 Message Lost
#define DBC_CAN_RXF0S_F0F_POS                           24u    ///< Position for Rx FIFO 0 Full
#define DBC_CAN_RXF0S_F0PI_POS                          16u    ///< Position for Rx FIFO 0 Put Index
#define DBC_CAN_RXF0S_F0GI_POS                           8u    ///< Position for Rx FIFO 0 Get Index
#define DBC_CAN_RXF0S_F0FL_POS                           0u    ///< Position for Rx FIFO 0 Fill Level


/* DBC_CAN_RXF1C: Rx FIFO 1 Configuration register (0x10B0) */
/* Definitions of Bit-Masks */
#define DBC_CAN_RXF1C_F1OM_MASK                 0x80000000u    ///< Mask for Rx FIFO 1 Operation Mode
#define DBC_CAN_RXF1C_F1WM_MASK                 0x7F000000u    ///< Mask for Rx FIFO 1 Watermark
#define DBC_CAN_RXF1C_F1S_MASK                  0x007F0000u    ///< Mask for Rx FIFO 1 Size
#define DBC_CAN_RXF1C_F1SA_MASK                 0x0000FFFFu    ///< Mask for Rx FIFO 1 Start Address
/* Definitions of Bit-Positions */
#define DBC_CAN_RXF1C_F1OM_POS                          31u    ///< Position for Rx FIFO 1 Operation Mode
#define DBC_CAN_RXF1C_F1WM_POS                          24u    ///< Position for Rx FIFO 1 Watermark
#define DBC_CAN_RXF1C_F1S_POS                           16u    ///< Position for Rx FIFO 1 Size
#define DBC_CAN_RXF1C_F1SA_POS                           0u    ///< Position for Rx FIFO 1 Start Address


/* DBC_CAN_RXF1S: Rx FIFO 1 Status register (0x10AB) */
/* Definitions of Bit-Masks */
#define DBC_CAN_RXF1S_DMS_MASK                  0x80000000u    ///< Mask for Rx FIFO 1 Debug Message Status
#define DBC_CAN_RXF1S_RF1L_MASK                 0x02000000u    ///< Mask for Rx FIFO 1 Message Lost
#define DBC_CAN_RXF1S_F1F_MASK                  0x01000000u    ///< Mask for Rx FIFO 1 Full
#define DBC_CAN_RXF1S_F1PI_MASK                 0x003F0000u    ///< Mask for Rx FIFO 1 Put Index
#define DBC_CAN_RXF1S_F1GI_MASK                 0x00003F00u    ///< Mask for Rx FIFO 1 Get Index
#define DBC_CAN_RXF1S_F1FL_MASK                 0x0000007Fu    ///< Mask for Rx FIFO 1 Fill Level
/* Definitions of Bit-Positions */
#define DBC_CAN_RXF1S_DMS_POS                           31u    ///< Position for Rx FIFO 1 Debug Message Status
#define DBC_CAN_RXF1S_RF1L_POS                          25u    ///< Position for Rx FIFO 1 Message Lost
#define DBC_CAN_RXF1S_F1F_POS                           24u    ///< Position for Rx FIFO 1 Full
#define DBC_CAN_RXF1S_F1PI_POS                          16u    ///< Position for Rx FIFO 1 Put Index
#define DBC_CAN_RXF1S_F1GI_POS                           8u    ///< Position for Rx FIFO 1 Get Index
#define DBC_CAN_RXF1S_F1FL_POS                           0u    ///< Position for Rx FIFO 1 Fill Level


/* DBC_CAN_RXESC: Rx Buffer/FIFO Element Size Configuration register (0x10BC) */
/* Definitions of Bit-Masks */
#define DBC_CAN_RXESC_RBDS_MASK                 0x00000700u    ///< Mask for Rx Buffer Data Field Size
#define DBC_CAN_RXESC_F1DS_MASK                 0x00000070u    ///< Mask for Rx FIFO 1 Data Field Size
#define DBC_CAN_RXESC_F0DS_MASK                 0x00000007u    ///< Mask for Rx FIFO 0 Data Field Size
/* Definitions of Bit-Positions */
#define DBC_CAN_RXESC_RBDS_POS                           8u    ///< Position for Rx Buffer Data Field Size
#define DBC_CAN_RXESC_F1DS_POS                           4u    ///< Position for Rx FIFO 1 Data Field Size
#define DBC_CAN_RXESC_F0DS_POS                           0u    ///< Position for Rx FIFO 0 Data Field Size


/* DBC_CAN_TXBC: Tx Buffer Configuration register (0x10C0) */
/* Definitions of Bit-Masks */
#define DBC_CAN_TXBC_TFQM_MASK                  0x40000000u    ///< Mask for Tx FIFO/Queue Mode
#define DBC_CAN_TXBC_TFQS_MASK                  0x3F000000u    ///< Mask for Tx FIFO/Queue Size
#define DBC_CAN_TXBC_NDTB_MASK                  0x003F0000u    ///< Mask for Number of Dedicated Transmit Buffers
#define DBC_CAN_TXBC_TBSA_MASK                  0x0000FFFFu    ///< Mask for Tx Buffers Start Address
/* Definitions of Bit-Positions */
#define DBC_CAN_TXBC_TFQM_POS                           30u    ///< Position for Tx FIFO/Queue Mode
#define DBC_CAN_TXBC_TFQS_POS                           24u    ///< Position for Tx FIFO/Queue Size
#define DBC_CAN_TXBC_NDTB_POS                           16u    ///< Position for Number of Dedicated Transmit Buffers
#define DBC_CAN_TXBC_TBSA_POS                            0u    ///< Position for Tx Buffers Start Address

/* DBC_CAN_TXESC: Tx Buffer Element Size Configuration register (0x10C8) */
/* Definitions of Bit-Masks */
#define DBC_CAN_TXBC_TBDS_MASK                  0x00000007u    ///< Mask for Tx Buffer Data Field Size
/* Definitions of Bit-Positions */
#define DBC_CAN_TXBC_TBDS_POS                            0u    ///< Position for Tx Buffer Data Field Size

/* DBC_CAN_TXEFC: Tx Event FIFO Configuration register (0x10F0) */
/* Definitions of Bit-Masks */
#define DBC_CAN_TXEFC_EFWM_MASK                 0x3F000000u    ///< Mask for Tx Event FIFO Watermark
#define DBC_CAN_TXEFC_EFS_MASK                  0x003F0000u    ///< Mask for Tx Event FIFO Size
#define DBC_CAN_TXEFC_EFSA_MASK                 0x0000FFFFu    ///< Mask for Tx Event FIFO Start Address
/* Definitions of Bit-Positions */
#define DBC_CAN_TXEFC_EFWM_POS                          24u    ///< Position for Tx Event FIFO Watermark
#define DBC_CAN_TXEFC_EFS_POS                           16u    ///< Position for Tx Event FIFO Size
#define DBC_CAN_TXEFC_TBSA_POS                           0u    ///< Position for Tx Event FIFO Start Address

/* DBC_CAN_SWCR: Selective Wake Control Register (0x2010) */
/* Definitions of Bit-Masks */
#define DBC_CAN_SWCR_CAN_DR_MASK                0x70000000u    ///< Mask for CAN bus data rate
/* Definitions of Bit-Positions */
#define DBC_CAN_SWCR_CAN_DR_POS                         28u    ///< Position for CAN bus data rate

/**
 * @brief Defines of valid values of DLC (Data Length Code) bitfield of the @c DBC_CanRxHdr_t / @c DBC_CanTxHdr_t structures
 */
enum DBC_CAN_DLC {
    DBC_CAN_DLC_0B  = 0u,
    DBC_CAN_DLC_1B  = 1u,
    DBC_CAN_DLC_2B  = 2u,
    DBC_CAN_DLC_3B  = 3u,
    DBC_CAN_DLC_4B  = 4u,
    DBC_CAN_DLC_5B  = 5u,
    DBC_CAN_DLC_6B  = 6u,
    DBC_CAN_DLC_7B  = 7u,
    DBC_CAN_DLC_8B  = 8u,
    DBC_CAN_DLC_12B = 9u,
    DBC_CAN_DLC_16B = 10u,
    DBC_CAN_DLC_20B = 11u,
    DBC_CAN_DLC_24B = 12u,
    DBC_CAN_DLC_32B = 13u,
    DBC_CAN_DLC_48B = 14u,
    DBC_CAN_DLC_64B = 15u
};

/**
 * @brief CAN message header structure for received messages
 */
typedef struct
{
    /* Word 0 */
    //! @brief CAN ID received
    TbitField ID : 29;
    //! @brief Remote Transmission Request flag
    TbitField RTR : 1;
    //! @brief Extended Identifier flag
    TbitField XTD : 1;
    //! @brief Error state indicator flag
    TbitField ESI : 1;
    /* Word 1 */
    //! @brief Receive time stamp
    TbitField RXTS : 16;
    //! @brief Data length code
    enum DBC_CAN_DLC DLC : 4;
    //! @brief Bit rate switch used flag
    TbitField BRS : 1;
    //! @brief CAN FD Format flag
    TbitField FDF : 1;
    //! @brief Reserved (0)
    TbitField : 2;
    //! @brief Filter index that this message matched
    TbitField FIDX : 7;
    //! @brief Accepted non-matching frame flag
    TbitField ANMF : 1;
} DBC_CanRxHdr_t;

#define DBC_CAN_HDR_SIZE     (uint16_t)sizeof(DBC_CanRxHdr_t)   ///< The number of bytes of CAN message header (received and transmitted message)
/* Definitions of Bit-Masks */
/* Word 0 */
#define DBC_CAN_RX_HDR_ESI_MASK                 0x80000000u    ///< Mask for Error state Indicator flag
#define DBC_CAN_RX_HDR_XTD_MASK                 0x40000000u    ///< Mask for Extended Identifier flag
#define DBC_CAN_RX_HDR_RTR_MASK                 0x20000000u    ///< Mask for Remote Transmission Request flag
#define DBC_CAN_RX_HDR_SID_MASK                 0x1FFC0000u    ///< Mask for CAN Standard ID received
#define DBC_CAN_RX_HDR_XID_MASK                 0x1FFFFFFFu    ///< Mask for CAN Extended ID received
/* Word 1 */
#define DBC_CAN_RX_HDR_ANMF_MASK                0x80000000u    ///< Mask for Accepted non-matching frame flag
#define DBC_CAN_RX_HDR_FIDX_MASK                0x7F000000u    ///< Mask for Filter index flag
#define DBC_CAN_RX_HDR_FDF_MASK                 0x00200000u    ///< Mask for CAN FD Format flag
#define DBC_CAN_RX_HDR_BRS_MASK                 0x00100000u    ///< Mask for Bit rate switch flag
#define DBC_CAN_RX_HDR_DLC_MASK                 0x000F0000u    ///< Mask for Data length code
#define DBC_CAN_RX_HDR_RXTS_MASK                0x0000FFFFu    ///< Mask for Receive time stamp
/* Definitions of Bit-Positions */
/* Word 0 */
#define DBC_CAN_RX_HDR_ESI_POS                          31u    ///< Position for Error state Indicator flag
#define DBC_CAN_RX_HDR_XTD_POS                          30u    ///< Position for Extended Identifier flag
#define DBC_CAN_RX_HDR_RTR_POS                          29u    ///< Position for Remote Transmission Request flag
#define DBC_CAN_RX_HDR_SID_POS                          18u    ///< Position for CAN Standard ID received
#define DBC_CAN_RX_HDR_XID_POS                           0u    ///< Position for CAN Extended ID received
/* Word 1 */
#define DBC_CAN_RX_HDR_ANMF_POS                         31u    ///< Position for Accepted non-matching frame flag
#define DBC_CAN_RX_HDR_FIDX_POS                         24u    ///< Position for Filter index flag
#define DBC_CAN_RX_HDR_FDF_POS                          21u    ///< Position for CAN FD Format flag
#define DBC_CAN_RX_HDR_BRS_POS                          20u    ///< Position for Bit rate switch flag
#define DBC_CAN_RX_HDR_DLC_POS                          16u    ///< Position for Data length code
#define DBC_CAN_RX_HDR_RXTS_POS                          0u    ///< Position for Receive time stamp


/**
 * @brief CAN message header structure for transmitted messages
 */
typedef PACKED struct {
    /* Word 0 */
    //! @brief CAN ID to send
    TbitField ID : 29;
    //! @brief Remote Transmission Request flag
    TbitField RTR : 1;
    //! @brief Extended Identifier flag
    TbitField XTD : 1;
    //! @brief Error state indicator flag
    TbitField ESI : 1;
    //! @brief Data length code
    /* Half-Word 1 */
    enum DBC_CAN_DLC DLC : 4;
    //! @brief Bit rate switch used flag
    TbitField BRS : 1;
    //! @brief CAN FD Format flag
    TbitField FDF : 1;
    //! @brief Reserved
    TbitField : 1;
    //! @brief Event FIFO Control flag, to store tx events or not
    TbitField EFC : 1;
    //! @brief Message Marker, used if EFC is set to 1
    TbitField MM : 8;
} DBC_CanTxHdr_t;

/* Definitions of Bit-Masks */
#define DBC_CAN_TX_HDR_MM_MASK                  0xFF000000u    ///< Mask for Message Marker
#define DBC_CAN_TX_HDR_EFC_MASK                 0x00800000u    ///< Mask for Event FIFO Control flag
/* Definitions of Bit-Positions */
#define DBC_CAN_TX_HDR_MM_POS                           24u    ///< Position for Message Marker
#define DBC_CAN_TX_HDR_EFC_POS                          23u    ///< Position for Event FIFO Control flag


/**
 * @brief Enumeration of CAN frame types (standard CAN or CAN FD). Can be used for configuration of @c XTD or @c FDF bit of @c DBC_CanTxHdr_t / DBC_CanRxHdr_t struct
 */
typedef enum {
    DBC_CAN_FRAME_STD,
    DBC_CAN_FRAME_FD
} DBC_CanFrame_t;

/**
 * @brief Data payload defines for the different RAM sections, used by the @c DBC_CanRamCfg_t struct
 */
enum DBC_CanRamElementDataSize {
    //! 8 bytes of data payload
    DBC_CAN_RAM_8_Byte_Data  = 0u,
    //! 12 bytes of data payload
    DBC_CAN_RAM_12_Byte_Data = 1u,
    //! 16 bytes of data payload
    DBC_CAN_RAM_16_Byte_Data = 2u,
    //! 20 bytes of data payload
    DBC_CAN_RAM_20_Byte_Data = 3u,
    //! 24 bytes of data payload
    DBC_CAN_RAM_24_Byte_Data = 4u,
    //! 32 bytes of data payload
    DBC_CAN_RAM_32_Byte_Data = 5u,
    //! 48 bytes of data payload
    DBC_CAN_RAM_48_Byte_Data = 6u,
    //! 64 bytes of data payload
    DBC_CAN_RAM_64_Byte_Data = 7u
};

/**
 * @brief Defines the number of RAM elements and the size of the elements (0x8000)
 */
typedef PACKED struct {
    /************************
     *    Filter Elements   *
     ************************/
    //! @brief Standard ID Number of Filter Elements: The number of 11-bit filters the user would like
    //! \n Valid range is: 0 to 128
    TbitField SidNumElements : 8;
    //! @brief Extended ID Number of Filter Elements: The number of 29-bit filters the user would like
    //! \n Valid range is: 0 to 64
    TbitField XidNumElements : 7;
    /************************
     *  RX FIFO Elements    *
     ************************/
    //! @brief RX FIFO 0 number of elements: The number of elements for the RX FIFO 0
    //! \n Valid range is: 0 to 64
    TbitField Rx0NumElements : 7;
    //! @brief RX FIFO 0 element size: The number of bytes for the RX 0 FIFO (data payload)
    enum DBC_CanRamElementDataSize Rx0ElementSize : 3;
    //! @brief RX FIFO 1 number of elements: The number of elements for the RX FIFO 1
    //!\n Valid range is: 0 to 64
    TbitField Rx1NumElements : 7;
    //! @brief RX FIFO 1 element size: The number of bytes for the RX 1 FIFO (data payload)
    enum DBC_CanRamElementDataSize Rx1ElementSize : 3;
    //! @brief RX Buffers number of elements: The number of elements for the RX Buffers (Not the FIFO)
    //! \n Valid range is: 0 to 64
    TbitField RxBufNumElements : 7;
    //! @brief RX Buffers element size: The number of bytes for the RX Buffers (data payload), not the FIFO
    enum DBC_CanRamElementDataSize RxBufElementSize : 3;
    /************************
     *  TX Buffer Elements  *
     ************************/
    //! @brief TX Event FIFO number of elements: The number of elements for the TX Event FIFO
    //! \n Valid range is: 0 to 32
    TbitField TxEventFifoNumElements : 6;
    //! @brief TX Buffers number of elements: The number of elements for the TX Buffers
    //! \n Valid range is: 0 to 32
    TbitField TxBufNumElements : 6;
    //! @brief TX Buffers element size: The number of bytes for the TX Buffers (data payload)
    enum DBC_CanRamElementDataSize TxBufElementSize : 3;
} DBC_CanRamCfg_t;

#define DBC_CAN_SID_FILTER_ELEMENT_SIZE                  4u    ///< The number of bytes for the SID Filter element
#define DBC_CAN_XID_FILTER_ELEMENT_SIZE                  8u    ///< The number of bytes for the XID Filter element
#define DBC_CAN_TXEF_ELEMENT_SIZE                        8u    ///< The number of bytes for the TX Event FIFO element

/*lint -restore -e657 -e658 -e9018*/

#ifdef __cplusplus
}
#endif

#endif // DBUSCAN_H
