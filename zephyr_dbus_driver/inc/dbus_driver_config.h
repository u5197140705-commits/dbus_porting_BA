#ifndef ZEPHYR_DBUS_DRIVER_CONFIG_H__
#define ZEPHYR_DBUS_DRIVER_CONFIG_H__

#include <stdint.h>
#include <stdbool.h>
#include <zephyr/kernel.h>

// Define TbitField for bit-field structures
typedef uint32_t TbitField;

// Placeholder for MCAL types and macros
enum MCAL_Error {
    MCAL_OK = 0,
    MCAL_ERROR = 1
};

typedef void (*MCAL_CallbackFunction_t)(void *obj, uint32_t flags, const void *eventResponse);

typedef struct {
    MCAL_CallbackFunction_t cbFunction;
    void *obj;
} MCAL_Callback_t;

struct MDIO_Channel {
    uint32_t dummy; // Placeholder
};



struct MSPI_Handle {
    uint32_t dummy; // Placeholder
    uint32_t status; // Placeholder for MCAL_STATUS_READY/BUSY
    void *mdmaRxHandle; // Placeholder
    MCAL_Callback_t mdmaRxCallback; // Placeholder
    void *mdmaTxHandle; // Placeholder
    MCAL_Callback_t mdmaTxCallback; // Placeholder
    void *channel; // Placeholder for MSPI_Channel
};

struct MDMA_Handle {
    uint32_t dummy; // Placeholder
};

struct MSUP_TimeoutHandle {
    uint32_t dummy; // Placeholder
};

struct MTDIV_Result {
    uint32_t quotient;
    uint32_t remainder;
};

struct MCAL_EventResponse {
    uint32_t dummy; // Placeholder
};

#define MCAL_STATUS_RESET 0x00000000u // Placeholder
#define MCAL_STATUS_READY 0x00000001u // Placeholder
#define MCAL_STATUS_BUSY  0x00000002u // Placeholder
#define MCAL_OK           0x00000000u // Placeholder
#define MCAL_EVENT_TRANSFER_COMPLETE 0x00000001u // Placeholder

#define MSPI_TIMEOUT_VALUE 10000u // Placeholder, in microseconds

#define TASK_INITIALISED 0 // Placeholder for task return value

#define WORD_SIZE 4u
// #define BYTE_SIZE 8u // Removed to avoid redefinition with dbus_app_layer.h
#define WORD_SIZEOF(len) ((len + WORD_SIZE - 1u) / WORD_SIZE)

// DBC_EEP_t structure definition from common/prog/devices/dbuscan/dbuscan.h
typedef struct {
    union {
        uint32_t word;
        struct {
            TbitField BVD_TO_NWKRQ : 1;
            TbitField MCAN_EN      : 1;
            TbitField DBUS_EN      : 1;
            TbitField CAN_BIAS     : 1;
            TbitField FD_DR        : 1;
            TbitField CAN_DR       : 3;
            TbitField DBUS2CAN     : 1;
            TbitField GP_MEM       : 7;
            TbitField SUBNODE_ID   : 4;
            TbitField NODE_ID      : 4;
            TbitField ADV_PWR_MGMT : 1;
            TbitField BVD_THLD     : 1;
            TbitField BVD_WAIT_EN  : 1;
            TbitField CLKIN        : 2;
            TbitField DBR          : 3;
        };
    };
} DBC_EEP_t;

// DBC_Cfg_t is a typedef of DBC_EEP_t
typedef DBC_EEP_t DBC_Cfg_t;

// UTI_RES and UTI_SET macros from common/prog/ped_fw/utility/utility.h
#define UTI_SET(VAR1,VAR2)  ((VAR1)|=(VAR2))
#define UTI_RES(VAR1,VAR2)  ((VAR1)&=~(VAR2))

// Placeholder for DBC_Error enum
enum DBC_Error
{
    DBC_OK = 0,
    DBC_ERROR
};

// Macro for checking return value of a function.
#define DBC_RETURN_ON_ERROR(function) do {enum DBC_Error retVal = (function); if(retVal != DBC_OK) {return retVal;}} while(false);

// Placeholder for DBC_Reset enum
enum DBC_Reset
{
    DBC_RST_DBUS_SOFT = 0x00000100u,     ///< Causes reset of the DBus Rx/Tx buffers and interrupts of the DBusCAN chip.
    DBC_RST_DBUS_HARD = 0x00000200u, ///< Causes reset of the whole DBus part of the DBusCAN chip (DBus Rx/Tx buffers, interrupts and registers).
    DBC_RST_FULL = 0x00000000u                                   ///< Causes reset of the whole DBusCAN chip.
};

// Placeholder for DBC_RegAddr enum
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
    DBC_DBUS_TOCV_ADDR   = 0x402Cu,    /* TimeOut Counter Value register           Address offset: 0x402C */
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
    DBC_DBUS_NF0_ADDR    = 0x4200u,       /* DBus Node Filter 0              Address offset: 0x4200 */
    DBC_DBUS_NF1_ADDR    = 0x4200u + 4u,  /* DBus Node Filter 1              Address offset: 0x4204 */
    DBC_DBUS_NF2_ADDR    = 0x4200u + 8u,  /* DBus Node Filter 2              Address offset: 0x4208 */
    DBC_DBUS_NF3_ADDR    = 0x4200u + 12u, /* DBus Node Filter 3              Address offset: 0x420C */
    DBC_DBUS_NF4_ADDR    = 0x4200u + 16u, /* DBus Node Filter 4              Address offset: 0x4210 */
    DBC_DBUS_NF5_ADDR    = 0x4200u + 20u, /* DBus Node Filter 5              Address offset: 0x4214 */
    DBC_DBUS_NF6_ADDR    = 0x4200u + 24u, /* DBus Node Filter 6              Address offset: 0x4218 */
    DBC_DBUS_NF7_ADDR    = 0x4200u + 28u, /* DBus Node Filter 7              Address offset: 0x421C */
    DBC_DBUS_NF8_ADDR    = 0x4200u + 32u, /* DBus Node Filter 8              Address offset: 0x4220 */
    DBC_DBUS_NF9_ADDR    = 0x4200u + 36u, /* DBus Node Filter 9              Address offset: 0x4224 */
    DBC_DBUS_NF10_ADDR   = 0x4200u + 40u, /* DBus Node Filter 10             Address offset: 0x4228 */
    DBC_DBUS_NF11_ADDR   = 0x4200u + 44u, /* DBus Node Filter 11             Address offset: 0x422C */
    DBC_DBUS_NF12_ADDR   = 0x4200u + 48u, /* DBus Node Filter 12             Address offset: 0x4230 */
    DBC_DBUS_NF13_ADDR   = 0x4200u + 52u, /* DBus Node Filter 13             Address offset: 0x4234 */
    DBC_DBUS_NF14_ADDR   = 0x4200u + 56u, /* DBus Node Filter 14             Address offset: 0x4238 */
    DBC_DBUS_TXSF_ADDR   = 0x4300u,     /* DBus Transmit Status FIFO       Address offset: 0x4300 */
    DBC_DBUS_RX_TX_FIFO_ADDR = 0x4400u   /* DBus Receive/Transmit FIFO      Address offset: 0x4400 */
};


#define DBC_DBUS_NF_BASE       (0x4200u)  ///< Base address of Node Filter registers
#define DBC_DBUS_TXSF_BASE     (0x4300u)  ///< Base address of DBus Tx Status FIFO buffer
#define DBC_DBUS_TXF_BASE      (0x4400u)  ///< Base address of DBus Tx FIFO buffer
#define DBC_DBUS_RXF_BASE      (0x4400u)  ///< Base address of DBus Rx FIFO buffer
#define DBC_RAM_BASE           (0x8000u)  ///< Base address of RAM memory

/* Size definitions */
#define DBC_RAM_SIZE             (2048u)  ///< The number of bytes of RAM memory inside the chip
#define DBC_DBUS_FIFO_MAX_MSG_CNT   (8u)  ///< DBus Tx/Rx FIFO can hold up to 8 DBus frames
#define DBC_SPI_HDR_SIZE            (4u)  ///< Each SPI frame starts with 4-byte SPI header (1 command byte + 2-byte address + 1 data length byte)
#define DBC_SPI_CRC_SIZE            (0u)  ///< The number of bytes for CRC in SPI frame if CRC is disabled
#define DBC_SPI_BUFFER_SIZE         (64u) ///< Placeholder for SPI buffer size
#define DBC_SPI_MAX_DATA_LEN        (60u) ///< Placeholder for max SPI data length (64 - HDR - CRC)

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
#define DBC_EEP_BITS_REG_DBUS_MASK              (~(DBC_EEP_BITS_REG_IPEC_MASK | DBC_EEP_BITS_REG_CAN_MASK) & 0xFFFFFFFFu)                                   ///< Mask for bits (pulled) from DBUS registers
#define DBC_EEP_BITS_REG_DBUS_DPC_MASK          (DBC_EEP_ADV_PWR_MGMT_MASK | DBC_EEP_BVD_WAIT_EN_MASK | DBC_EEP_BVD_THLD_MASK | DBC_EEP_BVD_TO_NWKRQ_MASK) ///< Mask for bits (pulled) from the DBUS_DPC register
#define DBC_EEP_BITS_REG_DBUS_SIDFC_MASK        (DBC_EEP_SUBNODE_ID_MASK | DBC_EEP_NODE_ID_MASK)                                            ///< Mask for bits (pulled) from the DBUS_SIDFC register
#define DBC_EEP_BITS_REG_DBUS_DBR_MASK          (DBC_EEP_DBR_MASK | DBC_EEP_CLKIN_MASK)                                                     ///< Mask for bits (pulled) from the DBUS_DBR register
/* Definitions of Bit-Positions */
#define DBC_EEP_GP_MEM_POS                               9u      ///< Position for General purpose memory bits
#define DBC_EEP_SUBNODE_ID_POS                          16u      ///< Position for DBus Subnode ID
#define DBC_EEP_NODE_ID_POS                             20u      ///< Position for DBus Node ID
#define DBC_EEP_ADV_PWR_MGMT_POS                        24u      ///< Position for Advanced Power Management frames enable
#define DBC_EEP_BVD_THLD_POS                            25u      ///< Position for Bus Voltage Detector Threshold Voltage
#define DBC_EEP_BVD_WAIT_EN_POS                         26u      ///< Position for Bus Voltage Detector Wait Enable
#define DBC_EEP_CLKIN_POS                               27u      ///< Position for Clock Input
#define DBC_EEP_DBR_POS                                 29u      ///< Position for DBus Baud Rate
#define DBC_EEP_CAN_DR_POS                               5u      ///< Position for CAN Bus Data Rate
#define DBC_EEP_FD_DR_POS                                4u      ///< Position for CAN FD bus data rate ratio vs CAN data rate
#define DBC_EEP_CAN_BIAS_POS                             3u      ///< Position for CAN Sleep Mode Bias Voltage
#define DBC_EEP_DBUS_EN_POS                              2u      ///< Position for DBus IP enable bit
#define DBC_EEP_MCAN_EN_POS                              1u      ///< Position for CAN IP enable bit
#define DBC_EEP_BVD_TO_NWKRQ_POS                         0u      ///< Position for BVD to nWKRQ pin enable bit
#define DBC_EEP_DBUS2CAN_POS                             8u      ///< Position for DBus TXD/RXD to CAN transceiver enable bit


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
#define DBC_DBUS_IR_WUP_POS                             30u      ///< Position for Wake Up Pulse Interrupt Enable
#define DBC_DBUS_IR_ARA_POS                             29u      ///< Position for Access to a reserved address status bit
#define DBC_DBUS_IR_DBUSSLNT_POS                        28u      ///< Position for DBus has been silent for tSILENCE amount of time
#define DBC_DBUS_IR_BVD_POS                             25u      ///< Position for BVD Interrupt
#define DBC_DBUS_IR_BVD_FAIL_POS                        24u      ///< Position for BVD Fail from a TX Wake Pulse
#define DBC_DBUS_IR_BEU_POS                             21u      ///< Position for Bit Error Uncorrected
#define DBC_DBUS_IR_BEC_POS                             20u      ///< Position for Bit Error Corrected
#define DBC_DBUS_IR_TOO_POS                             18u      ///< Position for Timeout Occurred
#define DBC_DBUS_IR_TSW_POS                             16u      ///< Position for Timestamp Wraparound
#define DBC_DBUS_IR_TEFL_POS                            15u      ///< Position for Tx Status FIFO Element Lost
#define DBC_DBUS_IR_TEFF_POS                            14u      ///< Position for Tx Status FIFO Full Event Enable
#define DBC_DBUS_IR_TEFN_POS                            12u      ///< Position for Tx Status FIFO New Entry Event Enable
#define DBC_DBUS_IR_TFE_POS                             11u      ///< Position for Tx FIFO Empty Event Enable
#define DBC_DBUS_IR_TFL_POS                             10u      ///< Position for Tx FIFO Message Lost Event Enable
#define DBC_DBUS_IR_TC_POS                               9u      ///< Position for Transmission Completed Event Enable
#define DBC_DBUS_IR_RF0L_POS                             3u      ///< Position for Rx FIFO Message Lost Event Enable
#define DBC_DBUS_IR_RF0F_POS                             2u      ///< Position for Rx FIFO Full Event Enable
#define DBC_DBUS_IR_RBLFE_POS                            1u      ///< Position for Rx Bootloader Framing Error Event Enable
#define DBC_DBUS_IR_RF0N_POS                             0u      ///< Position for Rx FIFO New Message Event Enable

/* DBUS->IE (0x4054) */
/* Definitions of Bit-Masks */
#define DBC_DBUS_IE_ALL_BIT_MASK                0xF035DE0Fu      ///< Mask for (enabling) all interrupts of DBUS_IE register
#define DBC_DBUS_IE_WK_EVENT_EN_MASK            0x80000000u      ///< Mask for Wake Event Enable
#define DBC_DBUS_IE_WUP_EN_MASK                 0x40000000u      ///< Mask for Wake Up Pulse Interrupt Enable
#define DBC_DBUS_IE_ARA_EN_MASK                 0x20000000u      ///< Mask for Access to Reserved Address Event Enable
#define DBC_DBUS_IE_DBUSSLNT_EN_MASK            0x10000000u      ///< Mask for DBus Silent Flag Enable
#define DBC_DBUS_IE_BVD_EN_MASK                 0x02000000u      ///< Mask for BVD Interrupt Enable
#define DBC_DBUS_IE_BVD_FAIL_EN_MASK            0x01000000u      ///< Mask for BVD Fail from a TX Wake Pulse Event Enable
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
#define DBC_DBUS_IE_RF0N_EN_POS                          0u      ///< Position for Rx FIFO FIFO New Message Event Enable

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
#define DBC_DBUS_TXFQS_TFDA_POS                          8u      ///< Position for Tx FIFO Data Available (number of bytes in Tx FIFO)
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

#define DBC_DBUS_CRC_SIZE                 (2u)                          ///< The number of bytes of DBus message CRC
#define DBC_DBUS_TXF_HDR_SIZE             (6u)                          ///< The number of bytes of overhead data in DBus Transmit FIFO buffer (@c DBC_DBUS_TXF_t)
#define DBC_DBUS_TXF_MSG_ID_HIGH           DBC_DBUS_TXF_HDR_SIZE        ///< Byte offset of DBus message ID high byte in Tx FIFO
#define DBC_DBUS_TXF_MSG_ID_LOW           (DBC_DBUS_TXF_HDR_SIZE + 1u)  ///< Byte offset of DBus message ID low byte in Tx FIFO
#define DBC_DBUS_TXF_DATA_OFFSET          (DBC_DBUS_TXF_HDR_SIZE + 2u)  ///< Byte offset of DBus message data in Tx FIFO
#define DBC_DBUS_TXF_CRC_IN_BUFFER        (1u)                          ///< CRC is put into Tx FIFO together with DBus frame data
#define DBC_DBUS_TXF_CRC_HW_CALCULATED    (0u)                          ///< CRC over DBus frame is calculated/added by the DBusCAN chip

#define DBC_DBUS_BL_TXF_HDR_SIZE          (4u)  ///< The number of bytes of overhead data in DBus BootLoader Transmit FIFO buffer (@c DBC_DBUS_BL_TXF_t)
#define DBC_DBUS_BL_TXF_DATA_OFFSET       DBC_DBUS_BL_TXF_HDR_SIZE  ///< Offset of data in DBus BootLoader Transmit FIFO buffer (@c DBC_DBUS_BL_TXF_t)

// Placeholder for DBC_DBUS_RXF_t and DBC_DBUS_TXSF_t structures
// These are needed for DBC_DBUS_RXF_HDR_SIZE and DBC_DBUS_TXSF_SIZE
typedef struct {
    uint32_t dummy[4]; // Placeholder for actual RXF structure
} DBC_DBUS_RXF_t;

typedef struct {
    uint32_t dummy[2]; // Placeholder for actual TXSF structure
} DBC_DBUS_TXSF_t;

#define DBC_DBUS_RXF_HDR_SIZE             ((uint16_t)sizeof(DBC_DBUS_RXF_t))  ///< The number of bytes of overhead data in DBus Receive FIFO buffer (@c DBC_DBUS_RXF_t)
#define DBC_DBUS_TXSF_SIZE    ((uint16_t)sizeof(DBC_DBUS_TXSF_t))    ///< The number of bytes in DBus Transmit Status FIFO buffer

#define DBC_CAN_SID_FILTER_ELEMENT_SIZE                  4u    ///< The number of bytes for the SID Filter element
#define DBC_CAN_XID_FILTER_ELEMENT_SIZE                  8u    ///< The number of bytes for the XID Filter element
#define DBC_CAN_TXEF_ELEMENT_SIZE                        8u    ///< The number of bytes for the TX Event FIFO element

// Additional definitions from dbuscan.h that might be missing or needed
#define DBC_IPEC_EP_CC_VAL                 0x17CAu
#define DBC_IPEC_EP_CC_POS                 16u
#define DBC_SPI_HDR_BYTE_CMD               0u
#define DBC_SPI_HDR_BYTE_ADDR_HIGH         1u
#define DBC_SPI_HDR_BYTE_ADDR_LOW          2u
#define DBC_SPI_HDR_BYTE_DATA_LEN          3u

#define DBC_SPI_HDR_BYTE_CMD_READ               0x00u
#define DBC_SPI_HDR_BYTE_CMD_WRITE              0x80u
#define DBC_SPI_HDR_BYTE_CMD_READ_BURST         0x20u
#define DBC_SPI_HDR_BYTE_CMD_WRITE_BURST        0xA0u

#define DBC_DBUS_BAUD_125K                      0x00u
#define DBC_DBUS_BAUD_250K                      0x01u
#define DBC_DBUS_BAUD_500K                      0x02u
#define DBC_DBUS_BAUD_1M                        0x03u
#define DBC_DBUS_BAUD_2M                        0x04u
#define DBC_DBUS_BAUD_4M                        0x05u
#define DBC_DBUS_BAUD_8M                        0x06u

#define DBC_DBUS_CLKIN_8M                       0x00u
#define DBC_DBUS_CLKIN_16M                      0x01u
#define DBC_DBUS_CLKIN_20M                      0x02u
#define DBC_DBUS_CLKIN_40M                      0x03u

// Enum for EEPROM write state
enum DBCDRV_EepWriteState {
    DBCDRV_EEP_WRITE_IDLE = 0,
    DBCDRV_EEP_WRITE_PENDING,
    DBCDRV_EEP_WRITE_IN_PROGRESS,
    DBCDRV_EEP_WRITE_DONE,
    DBCDRV_EEP_WRITE_ERROR,
    DBCDRV_EEP_WRITE_START,
    DBCDRV_EEP_WRITE_ONGOING,
    DBCDRV_EEP_WRITE_END
};

// Enum for DBC_command
enum DBC_command {
    DBC_CMD_READ = 0x40u,    // Read  - Low data byte first
    DBC_CMD_WRITE = 0x60u,   // Write - Low data byte first
    DBC_CMD_READ_BURST = 0x20, // Assuming these remain the same, verify if needed
    DBC_CMD_WRITE_BURST = 0xA0 // Assuming these remain the same, verify if needed
};

// Enum for DBC_PowerMode
enum DBC_PowerMode {
    DBC_POWER_MODE_NORMAL = 0,
    DBC_POWER_MODE_STANDBY
};

// Union for DBC_SpiBuf
union DBC_SpiBuf {
    uint8_t array[64]; // DBC_SPI_BUFFER_SIZE is 64
    struct {
        uint8_t spiHdr[4]; // DBC_SPI_HDR_SIZE is 4
        uint32_t data0;
        uint32_t crc;
    } d0;
    struct {
        uint8_t spiHdr[4]; // DBC_SPI_HDR_SIZE is 4
        uint32_t data0;
        uint32_t data1;
        uint32_t crc;
    } d1;
    struct {
        uint8_t spiHdr[4]; // DBC_SPI_HDR_SIZE is 4
        uint32_t data0;
        uint32_t data1;
        uint32_t data2;
        uint32_t crc;
    } d2;
};

// Function Prototypes
DBC_Cfg_t DBCDRV_getConfig(void);
enum MCAL_Error MDIO_init(const struct MDIO_Channel *channel, const void *config);
const struct MDIO_Channel *MEXTI_getPin(uint32_t channel);
bool MDIO_read(const struct MDIO_Channel *channel);
void MDIO_toggle(const struct MDIO_Channel *channel);
void MDIO_write(const struct MDIO_Channel *channel, bool value);
void MCAL_initCallback(MCAL_Callback_t *cb, MCAL_CallbackFunction_t func, void *obj);
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

void DBCDRV_setSpiFrameHdr(enum DBC_RegAddr addr, uint16_t len, enum DBC_command cmd, uint8_t *writeBuf);
enum DBC_Error DBCDRV_readReg32(enum DBC_RegAddr addr, uint32_t *data);
enum DBC_Error DBCDRV_writeReg32(enum DBC_RegAddr addr, uint32_t data);
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

#endif // ZEPHYR_DBUS_DRIVER_CONFIG_H__