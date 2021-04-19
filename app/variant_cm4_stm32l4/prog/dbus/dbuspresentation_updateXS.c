/*******************************************************************************
*   Copyright (c) 2017 BSH Hausgeraete GmbH,
*   Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
*
*   All rights reserved. This program and the accompanying materials
*   are protected by international copyright laws.
*   Please contact copyright holder for licensing information.
*
********************************************************************************
*   PROJECT          Generic SW
*   COMP_ABBREV      DBPL
*******************************************************************************/
#include "bsh_stdinc.h"
#include "dbuspresentation_update.h"
#include "dbusdll.h"
#include "dbusmapping.h"
#include "huart.h"
#include "hsup.h"


#if defined(DBUS2_UPDATE)||defined(DBUS2_UPDATE_HSI)
#if defined(REMOTE_FIRMWARE_UPDATE)
#include "firmware_update/BootManager/BootManagerSharedData.h"
#include "firmware_update/mal/moduleAdministration.h"
#include "firmware_update/BootManager/BootManagerModule.h"
#endif
#include "timerlib.h"
#endif
#if defined(DBUS2_PROD_MSG_IN_APP)
#include "firmware_update/fwu_version.h"
#include "firmware_update/sw_version.h"
#include "SegmentDef.h"
#endif

#if (DBUS_DEFAULT_BAUDRATE==96)
const enum DBPL_BaudRate DBPL_uDefaultBaudRate=DBPL_Baud9600;
#elif (DBUS_DEFAULT_BAUDRATE==192)
const enum DBPL_BaudRate DBPL_uDefaultBaudRate=DBPL_Baud19200;
#elif (DBUS_DEFAULT_BAUDRATE==384)
const enum DBPL_BaudRate DBPL_uDefaultBaudRate=DBPL_Baud38400;
#elif (DBUS_DEFAULT_BAUDRATE==576)
const enum DBPL_BaudRate DBPL_uDefaultBaudRate=DBPL_Baud57600;
#elif (DBUS_DEFAULT_BAUDRATE==1152)
const enum DBPL_BaudRate DBPL_uDefaultBaudRate=DBPL_Baud115200;
#elif (DBUS_DEFAULT_BAUDRATE==1250)
const enum DBPL_BaudRate DBPL_uDefaultBaudRate=DBPL_Baud125000;
#elif (DBUS_DEFAULT_BAUDRATE==2304)
const enum DBPL_BaudRate DBPL_uDefaultBaudRate=DBPL_Baud230400;
#elif (DBUS_DEFAULT_BAUDRATE==2500)
const enum DBPL_BaudRate DBPL_uDefaultBaudRate=DBPL_Baud250000;
#endif

#if defined(DBUS2_UPDATE)||defined(DBUS2_UPDATE_HSI)

SDEF_SetSegmentRW(TIMER16_10MS) // put it into segment TIMER8_10MS
static volatile Ttimer16 DBPL_tBaudRateTimer;

SDEF_SetSegmentRW(TIMER8_10MS) // put it into segment TIMER8_10MS
static volatile Ttimer8 DBPL_tResetTriggerTimer;
SDEF_SetSegmentRW_Default()

#ifdef VARIANT_PROGRAMMER
    #define UPDATE_TRANSITION_DELAY_10MS   0U
#else
    #define UPDATE_TRANSITION_DELAY_10MS   50U // In case of programmer zero, else time needed to boot another software
#endif

#define BAUDRATE_TRANSITION_DELAY_10MS 1U

#ifdef REMOTE_FIRMWARE_UPDATE
#define IS_UPDATE_POSSIBLE             1U
#else
#define IS_UPDATE_POSSIBLE             0U
#endif

static const enum DBPL_BaudRate baud_lookup_table[]={DBPL_Baud9600,DBPL_Baud19200,DBPL_Baud38400,DBPL_Baud57600,DBPL_Baud115200,DBPL_Baud125000};
/*lint -esym(9003,bit_time_lookup_table ) readability not better when in block scope */
static const enum DBPL_BitTimingUs bit_time_lookup_table[]=
{
    DBPL_BitTimingUs9600,   DBPL_BitTimingUs19200,
    DBPL_BitTimingUs38400,  DBPL_BitTimingUs57600,
    DBPL_BitTimingUs115200, DBPL_BitTimingUs125000
};

bool DBPL_bIsUpdateModePossible(void)
{
#if IS_UPDATE_POSSIBLE>0U
    return true;
#else
    return false;
#endif
}

void DBPL_vSetUpdateMode(void)
{
#if defined(REMOTE_FIRMWARE_UPDATE)
    BMDAT_setBootModule(MAL_PROGRAMMER_ID);
#ifndef VARIANT_PROGRAMMER
    HSUP_vGenerateReset();
#endif
#endif
}

void DBPL_vLeaveUpdateMode(void)
{
#if defined(REMOTE_FIRMWARE_UPDATE)
    BMDAT_setBaudRate(0u);
    BMDAT_setBootModule(MAL_PRODUCT_APP1);
#endif
    HSUP_vGenerateReset();
}

uint16_t DBPL_ulGetUpdateTransitionDelay(void)
{
    return (uint16_t)UPDATE_TRANSITION_DELAY_10MS;
}

bool DBPL_bIsRequestedBaudValid(uint16_t baud)
{
    uint8_t i;
    bool ret=false;
    uint8_t numOfElements=((uint8_t)sizeof(baud_lookup_table)/(uint8_t)sizeof(enum DBPL_BaudRate));
    for(i=0;i<numOfElements;i++)
    {   /*lint -e{662} no out of bound access here! */
        if(baud==(uint16_t)baud_lookup_table[i])
        {
            ret=true;
            break;
        }
    }
    return ret;
}

uint16_t DBPL_ulGetBaudrateTransitionDelay(void)
{
    return (uint16_t)BAUDRATE_TRANSITION_DELAY_10MS;
}

void DBPL_vConfigureBaudrate(uint16_t baud)
{
    if(DBPL_bIsRequestedBaudValid(baud))
    {

#if defined(REMOTE_FIRMWARE_UPDATE)
        BMDAT_setBaudRate(baud);
#endif
        DBM_UART_vSetBaudRate(DLL_ucGetStandardUartConfigIndex(),baud);
    }
}

uint16_t DBPL_uGetUsBitTimeForBaudRate(uint16_t baud)
{
    uint8_t numOfElements=((uint8_t)sizeof(bit_time_lookup_table)/(uint8_t)sizeof(enum DBPL_BitTimingUs));
    for(uint8_t i=0;i<numOfElements;i++)
    {
        if(baud==(uint16_t)baud_lookup_table[i])
        {
            return (uint16_t)bit_time_lookup_table[i];
        }
    }
    return 0U;
}

void DBPL_vSetBaudRateTimer(uint16_t baudRateTime)
{
    DBPL_tBaudRateTimer=baudRateTime;
}

uint16_t DBPL_uiGetBaudRateTimer(void)
{
    return (uint16_t)DBPL_tBaudRateTimer;
}

bool DBPL_bHasBaudRateTimerElapsed(void)
{
    return (DBPL_tBaudRateTimer==0U)?true:false;
}

void DBPL_vSetResetTriggerTimer(uint8_t resetTriggerTime)
{
    DBPL_tResetTriggerTimer=resetTriggerTime;
}

bool DBPL_bHasResetTriggerTimerElapsed(void)
{
    return (DBPL_tResetTriggerTimer==0U)?true:false;
}

#else /*DBUS2_UPDATE or DBUS2_UPDATE_HSI*/

bool DBPL_bIsUpdateModePossible(void)
{
    return false;
}

void DBPL_vSetUpdateMode(void)
{

}

void DBPL_vLeaveUpdateMode(void)
{

}

uint16_t DBPL_ulGetUpdateTransitionDelay(void)
{
    return (uint16_t)0U;
}

bool DBPL_bIsRequestedBaudValid(uint16_t baud)
{
    (void)baud;
    return false;
}

uint16_t DBPL_ulGetBaudrateTransitionDelay(void)
{
    return (uint16_t)0U;
}

void DBPL_vConfigureBaudrate(uint16_t baud)
{
    (void)baud;
}

uint16_t DBPL_uGetUsBitTimeForBaudRate(uint16_t baud)
{
    (void)baud;
    return 0U;
}

void DBPL_vSetBaudRateTimer(uint16_t baudRateTime)
{
    (void)baudRateTime;
}

uint16_t DBPL_uiGetBaudRateTimer(void)
{
    return 0U;
}


bool DBPL_bHasBaudRateTimerElapsed(void)
{
    return false;
}

void DBPL_vSetResetTriggerTimer(uint8_t resetTriggerTime)
{
    (void)resetTriggerTime;
}

bool DBPL_bHasResetTriggerTimerElapsed(void)
{
    return false;
}
#endif/*DBUS2_UPDATE or DBUS2_UPDATE_HSI*/



#if (defined(DBUS2_UPDATE)||defined(DBUS2_UPDATE_HSI))&&defined(REMOTE_FIRMWARE_UPDATE)
struct DBPL_EcuConfigReadResult DBPL_tGetEcuConfigReadResult(uint8_t objIdNumber)
{   //lint -e{785} "partially initialized struct MisraC2012 9.3 required" other 2 elements initialized later
    struct DBPL_EcuConfigReadResult obj = {.identificationObject = {{{0}}}}; // C99 initialisation, all zero!
#ifdef VARIANT_PROGRAMMER
    /* do not change settings for programmer */
    const uint8_t EcuConfigCount   = 1u; /* Programmer NOT updatable */
    const uint8_t ProgrammerObjId  = 1u;
    const uint8_t ApplicationObjId = 2u;
    obj.status = DBPL_EcuStatusUpdateModeActive; /*Programmer is running.*/
#else
    const uint8_t EcuConfigCount   = 1u; /*Standard: Programmer NOT updatable AND one user application.*/
    const uint8_t ProgrammerObjId  = 2u;
    const uint8_t ApplicationObjId = 1u;
    obj.status = DBPL_EcuStatusOK; /*Expected to be ok.*/
#endif
    const struct FWU_hw_version_s SDEF_FAR  *hw_version = NULL;
    const struct MAL_ModuleHeader_s SDEF_FAR *module_header = NULL;

    obj.objIdNumber = objIdNumber;
    obj.objCount = EcuConfigCount;

    if((objIdNumber > DBPL_UPDATE_ECU_CONFIG_UNSUPPORTED_OBJ_ID_NUMBER) && (objIdNumber <= EcuConfigCount))
    {
        if (objIdNumber == ApplicationObjId)
        {
            module_header = BMMOD_GetMemModuleHeader(MAL_PRODUCT_APP1);
        }
        else if (objIdNumber == ProgrammerObjId)
        {
            module_header = BMMOD_GetMemModuleHeader(MAL_PROGRAMMER_ID);
        }
        else
        {
            /* no action required */
        }

        hw_version = MAL_getHwVersion();
    }
    else if(objIdNumber == DBPL_UPDATE_ECU_CONFIG_UNSUPPORTED_OBJ_ID_NUMBER)
    {
        obj.status = DBPL_EcuStatusUnsupported;
    }
    else
    {
        /* no action required */
    }

    if(obj.status != DBPL_EcuStatusUnsupported)
    {
        if (module_header != NULL)
        {   /* Copy SW ID and version string */
            obj.identificationObject.sw_version = module_header->sw_version.version;
        }
        else
        {
            obj.status = DBPL_EcuStatusError;
        }

        if (hw_version != NULL)
        {
            /* Copy HW version string */
            obj.identificationObject.hw_version = hw_version->version;
        }
        else
        {
            obj.status = DBPL_EcuStatusError;
        }
    }
    
    return obj;
}

#elif defined (DBUS2_PROD_MSG_IN_APP)
/* FWU1 with fwu production messages enabled */
struct DBPL_EcuConfigReadResult DBPL_tGetEcuConfigReadResult(uint8_t objIdNumber)
{
    struct DBPL_EcuConfigReadResult obj = {.identificationObject = {{{0}}}}; // C99 initialization, all zero!
    const uint8_t EcuConfigCount = 1U;
    const struct FWU_hw_version_s SDEF_FAR  *hw_version = NULL;
    const struct FWU_sw_version_s SDEF_FAR  *sw_version = NULL;

    obj.status = DBPL_EcuStatusOK; /*Expected to be ok.*/
    obj.objIdNumber = objIdNumber;
    obj.objCount = EcuConfigCount;
    /* set pointer to address of hwversion structure location */
    hw_version = (SDEF_FAR const struct FWU_hw_version_s*)PROD_DATA_HWVERSION_ADDR;
    sw_version = (SDEF_FAR const struct FWU_sw_version_s*)&SW_version;
    
    if (hw_version != NULL)
    {
        /* Copy HW version string */
        obj.identificationObject.hw_version = hw_version->version;
    }
    else
    {
        obj.status = DBPL_EcuStatusError;
    }
    /* copy of SW ID and version from location to sw_version */
    obj.identificationObject.sw_version = sw_version->version;
    return obj;

}
#else
/* typical FWU1 application without fwu production messages */
struct DBPL_EcuConfigReadResult DBPL_tGetEcuConfigReadResult(uint8_t objIdNumber)
{
    struct DBPL_EcuConfigReadResult dummy={
        .status = DBPL_EcuStatusOK,
        .objIdNumber = objIdNumber,
        .objCount = 0U,
        .identificationObject = {{{0U}}}
    };

    return dummy;
}

#endif
