/*******************************************************************************
*   Copyright (c) 2017 BSH Hausgeraete GmbH,
*   Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
*
*   All rights reserved. This program and the accompanying materials
*   are protected by international copyright laws.
*   Please contact copyright holder for licensing information.
*
********************************************************************************
*   PROJECT          IC Dbus
*   COMP_ABBREV      DBPL
*******************************************************************************/

/**************************************************************************************************/
/* DOCUMENTATION                                                                                  */
/**************************************************************************************************/
/** \file
    \brief Public definitions and declarations for remote update.
*/

#ifndef DBUSPRESENTATION_UPDATE_H__
#define DBUSPRESENTATION_UPDATE_H__

#ifdef __cplusplus
extern "C" {
#endif

/*Forwand declaration of needed struct.*/
struct DBPL_EcuConfigReadResult;

/** \brief    List of baud rates allowed in dbus.*/
enum DBPL_BaudRate{
    DBPL_Baud9600 = 96U,
    DBPL_Baud19200 = 192U,
    DBPL_Baud38400 = 384U,
    DBPL_Baud57600 = 576U,
    DBPL_Baud115200 = 1152U,
    DBPL_Baud125000 = 1250U,
    DBPL_Baud230400 = 2304U,
    DBPL_Baud250000 = 2500U,
#ifdef DBM_DBUSCAN
    DBPL_Baud500000 = 5000U,
    DBPL_Baud1000000 = 10000U
#endif
};

/** \brief    List of bit times for baud rates allowed in dbus.*/
enum DBPL_BitTimingUs{
    DBPL_BitTimingUs9600 = 104U,
    DBPL_BitTimingUs19200 = 52U,
    DBPL_BitTimingUs38400 = 26U,
    DBPL_BitTimingUs57600 = 17U,
    DBPL_BitTimingUs115200 = 9U,
    DBPL_BitTimingUs125000 = 8U,
    DBPL_BitTimingUs230400 = 5U,
    DBPL_BitTimingUs250000 = 4U,
    DBPL_BitTimingUs500000 = 2U,
    DBPL_BitTimingUs1000000 = 1U
};

/** \brief Value for default baud rate. Defined in XS-File.*/
extern const enum DBPL_BaudRate DBPL_uDefaultBaudRate;

/**\brief Function in XS-File. See ,if it is possible to switch to update mode.*/
bool DBPL_bIsUpdateModePossible(void);

/** \brief Function in XS-File. Switch to the update mode.*/
void DBPL_vSetUpdateMode(void);

/** \brief Function in XS-File. Switch back to master app after (un-)successful programming.*/
void DBPL_vLeaveUpdateMode(void);

/** \brief Function in XS-File. Get constant defining, how much time is needed to switch to programmer.*/
uint16_t DBPL_ulGetUpdateTransitionDelay(void);

/**\brief Function in XS-File. See, which baudrates are possible.*/
bool DBPL_bIsRequestedBaudValid(uint16_t baud);

/**\brief Function in XS-File. Returns time needed for baudrate change, units of 10ms.*/
uint16_t DBPL_ulGetBaudrateTransitionDelay(void);

/** \brief Function in XS-File. Changes baudrate and timings to value previously requested with DBPL_bIsRequestedBaudValid.*/
void DBPL_vConfigureBaudrate(uint16_t baud);

/** \brief Function in XS-File. Returns bit time in US for given baud rate. This time is NOT capped.*/
uint16_t DBPL_uGetUsBitTimeForBaudRate(uint16_t baud);

/** \brief Function in XS-File. Returns configuration for ECU Config Read Response.*/
struct DBPL_EcuConfigReadResult DBPL_tGetEcuConfigReadResult(uint8_t objIdNumber);

/** \brief Function in XS-File. Returns configuration for ECU Sw Submodule Read Response.*/
struct DBPL_SwSubmoduleReadResult DBPL_tGetSwSubmoduleReadResult(const uint8_t * const pucSwID, uint8_t objIdNumber);

#ifdef __cplusplus
}
#endif

#endif

