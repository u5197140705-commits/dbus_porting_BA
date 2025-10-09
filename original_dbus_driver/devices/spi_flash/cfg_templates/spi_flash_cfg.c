/*******************************************************************************
 *  Copyright (c) 2017 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          Generic SW
 ******************************************************************************/


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *  \ingroup  serial_flash
 *
 *  \brief    Implementation of serial SPI flash configuration
 *
 *  \details  This interfaces the serial flash device with the SPI peripheral
 *            of the MCU.
 */


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include <stdint.h>
#include <stdbool.h>
#include "devices/spi_flash/spi_flash.h"
#include "hspi.h"
#include "hdio.h"

/******************************************************************************/
/* PRIVATE FUNCTION DECLARATIONS                                              */
/******************************************************************************/
static uint8_t ABSSF_exchangeByte(uint8_t data);
static bool    ABSSF_isTxOnGoing(void);

/******************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                */
/******************************************************************************/

void ABSSF_initInterface(void)
{
    ABSSF_unselectChip();
    HSPI2_vInit(0);
    HSPI2_vSetCom(0);
}


void ABSSF_readData(uint32_t size, uint8_t* buffer)
{
    for(uint32_t i = 0u; i < size; i++)
    {   // receive data by writing dummy data
        buffer[i]=ABSSF_exchangeByte(0u);
    }
}


void ABSSF_writeData(uint32_t size, const uint8_t* buffer)
{
    for(uint32_t i = 0u; i < size; i++)
    {   // send data, ignore received data
        (void)ABSSF_exchangeByte(buffer[i]);
    }
}


void ABSSF_selectChip(void)
{
    HDIO_vSetPortC0(0);
}


void ABSSF_unselectChip(void)
{
    HDIO_vSetPortC0(1);
}


/******************************************************************************/
/* PRIVATE FUNCTION DEFINITIONS                                               */
/******************************************************************************/

/** \brief    Exchange a single byte via SPI interface.
 *
 *  \details  An SPI interface sends an receives at the same time. For receiving
 *            dummy data need to be sent.
 *
 *  \param    data   data to transmit
 *  \return   the received data
 */
static uint8_t ABSSF_exchangeByte(uint8_t data)
{
    (void)HSPI2_bSetTxData(data);
    while(ABSSF_isTxOnGoing()){}
    return (uint8_t)HSPI2_uiGetRxData();
}


/** returns true as long an SPI transmission is ongoing */
static bool ABSSF_isTxOnGoing(void)
{
  #ifdef EFR32BG21
    return ((USART2->STATUS & _USART_STATUS_TXC_MASK)!=0UL) ? false : true;
  #else
  #error "Implement ABSSF_isTxOnGoing"
  #endif
    
}
