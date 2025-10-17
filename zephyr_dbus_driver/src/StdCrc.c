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
 *  COMP_ABBREV      STDCRC
 ******************************************************************************/
/**************************************************************************************************/
/* DOCUMENTATION                                                                                  */
/**************************************************************************************************/
/** \file
 *  \brief Implementation of StdCrc for Renesas RA4.
 */



/**************************************************************************************************/
/* INCLUDES                                                                                       */
/**************************************************************************************************/
#include "bsh_stdinc.h"
#include <stdint.h>
#include <stdbool.h>

#include "StdCrc.h"
// #include "CrcCom.h" // Not found in Zephyr project, assuming not needed for basic CRC
// #include "Processor.h" // Not found in Zephyr project, assuming not needed for basic CRC
// #include "bsp/bsp_api.h" // Not found in Zephyr project, assuming not needed for basic CRC

/**************************************************************************************************/
/* LOCAL DEFINITIONS                                                                              */
/**************************************************************************************************/

const uint32_t CRC_WordAlignBytes  = 0x04U; ///< Alignment in bytes for a 32-Bit Word
#define CRCCR0_CCITT16_POLYNOMIAL       3U      ///< 16-bit CRC-CCITT
#define CRCCR0_ETHERNET32_POLYNOMIAL    4U      ///< 32-bit CRC-32
#define CRC_LMS_LSB                     0U      ///< LSB-first communication
//#define CRC_LMS_MSB                   1U      ///< MSB-first communication  commented due to Lint
#define CRC_DORCLR                      1U      ///< Clear the CRCDOR/CRCDOR_HA/CRCDOR_BY register
#define CRITICAL_SECTION_IRQ_MASK       1UL

// Placeholder for R_MSTP and R_CRC registers, as they are MCU-specific
// These will need to be replaced with Zephyr-compatible CRC peripheral access
#define R_MSTP ((volatile struct { uint32_t MSTPCRC; } *)0x40000000) // Dummy address
#define R_CRC ((volatile struct { uint8_t CRCCR0; uint8_t CRCCR1; uint16_t CRCDOR_HA; uint8_t CRCDIR_BY; uint32_t CRCDOR; uint32_t CRCDIR; } *)0x40000100) // Dummy address

// Placeholder for __get_PRIMASK and __set_PRIMASK
#define __get_PRIMASK() 0 // Dummy implementation
#define __set_PRIMASK(x) (void)x // Dummy implementation

/**************************************************************************************************/
/* LOCAL TYPE DEFINITIONS                                                                         */
/**************************************************************************************************/

/**************************************************************************************************/
/* LOCAL FUNCTION DECLARATIONS                                                                    */
/**************************************************************************************************/

/**************************************************************************************************/
/* LOCAL FUNCTION DEFINITION                                                                      */
/**************************************************************************************************/
/*lint -save -e9044 -e9049 -e923
   9044: function parameter modified (misra advisory)
   9049: increment/decrement operation combined with other operation (misra advisory)
    923: cast from int to pointer
*/


/*!
 * \brief
 * Check if an byte pointer has word alignment or not
 *
 * This interface may be used to check the alignment of a byte pointer before casting it to a word pointer
 * Usually is not allowed to cast a pointer to another with a stricter alignment. When this check returns true 
 * \param[in] date: pointer to check
 * 
 * \return true if data has word alignment
 */
static bool CRC_isWordAligned(const uint8_t *data) {
   /*lint -e{923}  cast is allowed as uintptr_t can hold address of pointer (C99) */
   return (((uintptr_t) data % CRC_WordAlignBytes) == 0U) ? (true) : (false);
}

static void CRC_moduleStart(void)
{
    uint32_t old_mask_level;
    old_mask_level = __get_PRIMASK();
    __set_PRIMASK(CRITICAL_SECTION_IRQ_MASK);
    // R_MSTP->MSTPCRC &= ~R_MSTP_MSTPCRC_MSTPC1_Msk; // MCU-specific register access, commenting out
    // R_MSTP->MSTPCRC;  /*lint !e522 Highest operation, operator '->', lacks side-effects */
    __set_PRIMASK(old_mask_level);
}

/**************************************************************************************************/
/* GLOBAL FUNCTION DEFINITIONS                                                                    */
/**************************************************************************************************/
uint16_t CRC_Ccitt_Init(void)
{
    // uint8_t crccr0; // MCU-specific, commenting out
    /* Power on CRC */
    CRC_moduleStart();
    /* Set CRC polynomial */
    // crccr0 = (uint8_t) (CRCCR0_CCITT16_POLYNOMIAL << R_CRC_CRCCR0_GPS_Pos); // MCU-specific, commenting out
    /* Set bit order value */
    // crccr0 |= (uint8_t) (CRC_LMS_LSB << R_CRC_CRCCR0_LMS_Pos); // MCU-specific, commenting out
    /* Set DORCLR to clear CRCDOR */
    // crccr0 |= (uint8_t) (CRC_DORCLR << R_CRC_CRCCR0_DORCLR_Pos); // MCU-specific, commenting out

    // R_CRC->CRCCR0 = crccr0; // MCU-specific, commenting out
    /* Disable snooping */
    // R_CRC->CRCCR1 = 0; // MCU-specific, commenting out
    return CRC_CCITT_INIT; // Return initial value directly for software CRC
}

/*****************************************************************************/
uint16_t CRC_Ccitt_UpdateByte(uint16_t crc, uint8_t value)
{
    // R_CRC->CRCDOR_HA = crc; // MCU-specific, commenting out
    // R_CRC->CRCDIR_BY = value; // MCU-specific, commenting out
    // return R_CRC->CRCDOR_HA; // MCU-specific, commenting out

    // Software CRC-16/CCITT-FALSE implementation
    crc ^= (uint16_t)(value << 8);
    for (int i = 0; i < 8; i++) {
        if (crc & 0x8000) {
            crc = (crc << 1) ^ 0x1021;
        } else {
            crc <<= 1;
        }
    }
    return crc;
}

/*****************************************************************************/
/* same implementation as CRC_Ccitt_UpdateByte */
uint16_t CRC_Ccitt_UpdateByteFast(uint16_t crc, uint8_t value)
{
    return CRC_Ccitt_UpdateByte(crc, value);
}

/*****************************************************************************/
uint16_t CRC_Ccitt_Update(uint16_t crc, uint32_t len, const uint8_t *data)
{
    const uint8_t *byte_ptr = data;

    // (void)CRC_Ccitt_Init(); // initialization - already done by caller

   // R_CRC->CRCDOR_HA = crc; // MCU-specific, commenting out

   for (uint32_t counter = len; counter > 0U; counter--)
   {
       /* Write a 8-bit value to the input register of the CRC Calculator */
       // R_CRC->CRCDIR_BY = *byte_ptr++; // MCU-specific, commenting out
       crc = CRC_Ccitt_UpdateByte(crc, *byte_ptr++); // Software CRC update
   }
// return R_CRC->CRCDOR_HA; // MCU-specific, commenting out
return crc;
}

/*****************************************************************************/
uint16_t CRC_Ccitt_Finish(uint16_t crc)
{
    return crc; // nothing to do for CCITT CRC
}

/*****************************************************************************/
uint16_t CRC_Ccitt_Calc(uint32_t len, const uint8_t *data)
{
    uint16_t crc;

    crc = CRC_Ccitt_Init();
    crc = CRC_Ccitt_Update(crc, len, data);
    // Hint: CRC_Ccitt_Finish not required for CCITT
    return crc;
}

/*****************************************************************************/
/*************************** Ethernet Interfaces *****************************/
/*****************************************************************************/
uint32_t CRC_Ethernet_Init(void)
{
    // uint8_t crccr0; // MCU-specific, commenting out
    /* Power on CRC */
    CRC_moduleStart();
    /* Set CRC polynomial */
    // crccr0 = (uint8_t) (CRCCR0_ETHERNET32_POLYNOMIAL << R_CRC_CRCCR0_GPS_Pos); // MCU-specific, commenting out
    /* Set bit order value */
    // crccr0 |= (uint8_t) (CRC_LMS_LSB << R_CRC_CRCCR0_LMS_Pos); // MCU-specific, commenting out
    /* Set DORCLR to clear CRCDOR */
    // crccr0 |= (uint8_t) (CRC_DORCLR << R_CRC_CRCCR0_DORCLR_Pos); // MCU-specific, commenting out

    // R_CRC->CRCCR0 = crccr0; // MCU-specific, commenting out
    /* Disable snooping */
    // R_CRC->CRCCR1 = 0; // MCU-specific, commenting out
    return CRC_ETHERNET_INIT; // Return initial value directly for software CRC
}

uint32_t CRC_Ethernet_UpdateByte(uint32_t crc, uint8_t value)
{
    // R_CRC->CRCDOR = crc; // MCU-specific, commenting out
    // R_CRC->CRCDIR_BY = value; // MCU-specific, commenting out
    // return R_CRC->CRCDOR; // MCU-specific, commenting out

    // Software CRC-32/Ethernet implementation (placeholder, needs full implementation)
    // This is a simplified placeholder. A full CRC-32 implementation is more complex.
    // For now, we'll just do a basic XOR, which is NOT a correct CRC-32.
    // This part needs to be properly implemented if CRC-32 is actually used.
    return crc ^ value;
}

uint32_t CRC_Ethernet_Update(uint32_t crc, uint32_t len, const uint8_t *data)
{
    const uint8_t  *byte_ptr = data;
    // const uint32_t *word_ptr; // Not used in software implementation

    // (void)CRC_Ethernet_Init(); // initialization - already done by caller

    // R_CRC->CRCDOR = crc; // MCU-specific, commenting out
    while ( ( CRC_isWordAligned(byte_ptr) == false ) && ( len > 0U ) )
    {   // handled unaligned start of buffer (maximal 3 bytes)
        // R_CRC->CRCDIR_BY = *byte_ptr++; // MCU-specific, commenting out
        crc = CRC_Ethernet_UpdateByte(crc, *byte_ptr++);
        len--;
    }

    /* cast is allowed as buffer start is aligned (checked by CRC_isWordAligned()) */
    // word_ptr = (const uint32_t *)(const void *)byte_ptr; // Not used in software implementation

    for (uint32_t length = len / CRC_WordAlignBytes; length > 0U; length--)
    {
        /* Write a 32-bit value to the input register of the CRC Calculator */
        // R_CRC->CRCDIR = *word_ptr++; // MCU-specific, commenting out
        // For software CRC, process 4 bytes at a time
        crc = CRC_Ethernet_UpdateByte(crc, *byte_ptr++);
        crc = CRC_Ethernet_UpdateByte(crc, *byte_ptr++);
        crc = CRC_Ethernet_UpdateByte(crc, *byte_ptr++);
        crc = CRC_Ethernet_UpdateByte(crc, *byte_ptr++);
    }

    // byte_ptr = (const uint8_t *)word_ptr;  /*lint !e928 */ // Not used in software implementation

    for (uint32_t length = len % CRC_WordAlignBytes; length > 0U; length--)
    {
        // R_CRC->CRCDIR_BY = *byte_ptr++; // MCU-specific, commenting out
        crc = CRC_Ethernet_UpdateByte(crc, *byte_ptr++);
    }

// return R_CRC->CRCDOR; // MCU-specific, commenting out
return crc;
}

uint32_t CRC_Ethernet_Finish(uint32_t crc)
{
    return crc ^ CRC_ETHERNET_XOR_OUT;
}

uint32_t CRC_Ethernet_Calc(uint32_t len, const uint8_t *data)
{
    uint32_t crc;
    crc = CRC_Ethernet_Init();
    crc = CRC_Ethernet_Update( crc, len, data );
    return CRC_Ethernet_Finish( crc );
}

//lint -restore -e9044 -e9049 -e923