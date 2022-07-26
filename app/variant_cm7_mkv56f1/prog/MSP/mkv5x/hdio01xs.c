/*******************************************************************************
*   Copyright (c) 2016 BSH Hausgeraete GmbH,
*   Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
*
*   All rights reserved. This program and the accompanying materials
*   are protected by international copyright laws.
*   Please contact copyright holder for licensing information.
*
********************************************************************************
*   PROJECT          PED-Framework
*   COMP_ABBREV      HDIO
*   AUTHOR           VOLCKO Tomas
*   CREATED          08.10.2018  15:06:00
*******************************************************************************/


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file hdio01xs.c
*
*   \brief    Configuration file for hdio.
*
*/

/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include "hdio.h"


/******************************************************************************/
/* PRIVATE DEFINITIONS                                                        */
/******************************************************************************/

/* Change the array definitions according to application needs. */
const THDIO_Config_t HDIO_atConfig[] =
{
    {
        /* Port A */
        {
        HDIO_PIN10_ACTIVE|HDIO_PIN11_ACTIVE|HDIO_PIN12_ACTIVE|HDIO_PIN13_ACTIVE,
        HDIO_PIN10_LOW|HDIO_PIN11_LOW|HDIO_PIN12_HIGH,
        HDIO_PIN10_OUTPUT|HDIO_PIN11_OUTPUT|HDIO_PIN12_OUTPUT|HDIO_PIN12_OUTPUT|HDIO_PIN13_INPUT,
        HDIO_PIN10_PULLDIS|HDIO_PIN11_PULLDIS|HDIO_PIN12_PULLEN|HDIO_PIN13_PULLEN,
        HDIO_PIN12_PULLUP|HDIO_PIN13_PULLUP
        },
        /* Port B */
        {0},
        /* Port C */
        {0},
        /* Port D */
        {0},
        /* Port E */
        {0},
    }
};
/******************************************************************************/
/* PRIVATE FUNCTION DECLARATIONS                                              */
/******************************************************************************/


/******************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                */
/******************************************************************************/

