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
/** \file
 * \brief Application layer configuration for remote update
 */
#include "bsh_stdinc.h"
#include "dbuspresentation_ecu_types.h"
#include "dbuspresentation_update.h"
#include "dbusdll.h"
#include "dbusmapping.h"
#if defined(MCAL_MSUP_INCLUDED)
#include "mcal/msup.h" //Needed for reset execution
#else
#include "hsup.h" //Needed for reset execution
#endif

#if defined(DBUS2_UPDATE)||defined(DBUS2_UPDATE_HSI)
    #if defined(REMOTE_FIRMWARE_UPDATE)
        #include "firmware_update/BootManager/BootManagerSharedData.h"
        #include "firmware_update/mal/moduleAdministration.h"
        #include "firmware_update/BootManager/BootManagerModule.h"
    #endif
#endif

#if defined(DBUS2_PROD_MSG_IN_APP) || defined(REMOTE_FIRMWARE_UPDATE)
    #include "firmware_update/version/fwu_version.h"
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
#elif (DBUS_DEFAULT_BAUDRATE==5000)
const enum DBPL_BaudRate DBPL_uDefaultBaudRate=DBPL_Baud500000;
#elif (DBUS_DEFAULT_BAUDRATE==10000)
const enum DBPL_BaudRate DBPL_uDefaultBaudRate=DBPL_Baud1000000;
#endif

#if defined(DBUS2_UPDATE)||defined(DBUS2_UPDATE_HSI)

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

#ifdef DBM_DBUSCAN
static const enum DBPL_BaudRate baud_lookup_table[]={DBPL_Baud9600,DBPL_Baud19200,DBPL_Baud38400,DBPL_Baud57600,DBPL_Baud125000,DBPL_Baud250000,DBPL_Baud500000,DBPL_Baud1000000};
#else
static const enum DBPL_BaudRate baud_lookup_table[]={DBPL_Baud9600,DBPL_Baud19200,DBPL_Baud38400,DBPL_Baud57600,DBPL_Baud115200,DBPL_Baud125000};
/*lint -esym(9003,bit_time_lookup_table ) readability not better when in block scope */
static const enum DBPL_BitTimingUs bit_time_lookup_table[]=
{
    DBPL_BitTimingUs9600,   DBPL_BitTimingUs19200,
    DBPL_BitTimingUs38400,  DBPL_BitTimingUs57600,
    DBPL_BitTimingUs115200, DBPL_BitTimingUs125000
};
#endif // DBM_DBUSCAN

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
    #if defined(MCAL_MSUP_INCLUDED)
        MSUP_generateReset();
    #else
        HSUP_vGenerateReset();
    #endif
#endif
#endif
}

void DBPL_vLeaveUpdateMode(void)
{
#if defined(REMOTE_FIRMWARE_UPDATE)
    BMDAT_setBaudRate(0u);
    BMDAT_setBootModule(MAL_PRODUCT_APP1);
#endif
    #if defined(MCAL_MSUP_INCLUDED)
        MSUP_generateReset();
    #else
        HSUP_vGenerateReset();
    #endif
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
    {   /*lint -e{661,662} no out of bound access here! */
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
        DBM_PERIPH_vSetBaudRate(DLL_ucGetStandardUartConfigIndex(), baud);
    }
}

#ifndef DBM_DBUSCAN
uint16_t DBPL_uGetUsBitTimeForBaudRate(uint16_t baud)
{
    uint8_t numOfElements=((uint8_t)sizeof(bit_time_lookup_table)/(uint8_t)sizeof(enum DBPL_BitTimingUs));
    for(uint8_t i=0;i<numOfElements;i++)
    {   /*lint -e{661,662} no out of bound access here! */
        if(baud==(uint16_t)baud_lookup_table[i])
        {
            /*lint -e{661,662} no out of bound access here! */
            return (uint16_t)bit_time_lookup_table[i];
        }
    }
    return 0U;
}
#endif // DBM_DBUSCAN

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

#endif/*DBUS2_UPDATE or DBUS2_UPDATE_HSI*/


#if (defined(DBUS2_UPDATE)||defined(DBUS2_UPDATE_HSI))&&defined(REMOTE_FIRMWARE_UPDATE)
struct DBPL_EcuConfigReadResult DBPL_tGetEcuConfigReadResult(uint8_t objIdNumber)
{   /* "partially initialized struct MisraC2012 9.3 required" other 2 elements initialized later */
    struct DBPL_EcuConfigReadResult obj = {.identificationObject = {{{0}}}}; /*lint !e785 , C99 initialization, all zero! */
    const uint8_t EcuConfigCount   = ECU_ID_COUNT;
    const uint8_t ProgrammerObjId  = PROGRAMMER_ID_NUMBER;
    const uint8_t ApplicationObjId = APP_ID_NUMBER;
#ifdef VARIANT_PROGRAMMER
    /* do not change settings for programmer */
    obj.status = DBPL_EcuStatusUpdateModeActive; /*Programmer is running.*/
#else
    obj.status = DBPL_EcuStatusOK; /*Expected to be ok.*/
#endif
    const struct FWU_hw_version_s  *hw_version = NULL;
    const struct MAL_ModuleHeader_s *module_header = NULL;

    obj.objIdNumber = objIdNumber;
    obj.objCount = EcuConfigCount;

    if((objIdNumber > DBPL_UPDATE_ECU_CONFIG_UNSUPPORTED_OBJ_ID_NUMBER) && (objIdNumber <= EcuConfigCount))
    {
        if (objIdNumber == ApplicationObjId)/*lint !e774 boolean condition always evaluates to 'true/false' */
        {
            module_header = BMMOD_GetMemModuleHeader(MAL_PRODUCT_APP1);
        }
        else if (objIdNumber == ProgrammerObjId)/*lint !e774 boolean condition always evaluates to 'true/false' */
        {
            module_header = BMMOD_GetMemModuleHeader(MAL_PROGRAMMER_ID);
        }
        else
        {
            /* no action required */
        }

        hw_version = FWU_getHardwareVersion();
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

struct DBPL_SwSubmoduleReadResult DBPL_tGetSwSubmoduleReadResult(const uint8_t * const pucSwID, uint8_t objIdNumber)
{
    struct DBPL_SwSubmoduleReadResult obj = 
    {
        .status = DBPL_EcuStatusOK,
        .objIdNumber   = objIdNumber,
        .objCount      = 0x00u,        
        .identificationObject =
        {
         .sw_version =
            {
                .ID       = {0,1,2,3,4,5,6,7},
                .major    = {0x12, 0x34},
                .minor    = {0x98, 0x76},
                .revision = {0xfe, 0xdc},
                .build    = {0xab, 0xcd, 0xef, 0x01}
            },
         .sw_submodule_version = 
            {
                .ID       = {0,1,2,3,4,5,6,7},
                .major    = {0x12, 0x34},
                .minor    = {0x98, 0x76},
                .revision = {0xfe, 0xdc},
                .build    = {0xab, 0xcd, 0xef, 0x01}
            }
        }
    };
    
#ifdef VARIANT_PROGRAMMER
    if (NULL == pucSwID) {}
    (void)objIdNumber;
    obj.status = DBPL_EcuStatusUpdateModeActive; /*Programmer is running.*/
    return obj;   //return dummy data
#else
    const struct STDV_version bmVersion =
    {
        .ID = {0},
        .major = {0},
        .minor = {0},
        .revision = {0},
        .build = {0}
    };
    
    uint8_t SwSubmoduleCount = 0;
    enum MAL_ModuleEnum moduleID = MAL_NO_MODULE;
    const struct STDV_version *swVersion = NULL;
    const struct FWU_sw_version_s *subModuleVersion = NULL;
    const struct MAL_ModuleHeader_s *appModuleHeader = BMMOD_GetMemModuleHeader(MAL_PRODUCT_APP1);
    
    //check module variant(BootManager, programmer, application)
    /*lint -e{927,826}  cast from 'const uint8_t *const' (aka 'const unsigned char *const') to 'const struct STDV_idArray *' */
    if(STDV_isIdEqual(&bmVersion, (const struct STDV_idArray *)pucSwID) == true)  //is BootManager
    {
        moduleID = MAL_PARTITION_TABLE;
        
        swVersion = &bmVersion;
    }
    else if(STDV_isIdEqual(&appModuleHeader->sw_version.version, (const struct STDV_idArray *)pucSwID) == true) // is application
    {
        moduleID = MAL_PRODUCT_APP1;

        swVersion = &appModuleHeader->sw_version.version;
    }
    else
    {

    }

    // fetch submodule version
    if(moduleID != MAL_NO_MODULE)
    {
        SwSubmoduleCount = FWU_getSwSubmodCount(moduleID);
        subModuleVersion = FWU_getSwSubmodVersion(moduleID, objIdNumber);
    }

    // copy version data
    if(subModuleVersion != NULL)
    {
        obj.identificationObject.sw_submodule_version = subModuleVersion->version;

        obj.identificationObject.sw_version = *swVersion;

        obj.objCount = SwSubmoduleCount;
    }
    else
    {
        obj.status = DBPL_EcuStatusError;
    }
    return obj;
#endif
}

#elif defined (DBUS2_PROD_MSG_IN_APP)
/* FWU1 with fwu production messages enabled */
struct DBPL_EcuConfigReadResult DBPL_tGetEcuConfigReadResult(uint8_t objIdNumber)
{
    struct DBPL_EcuConfigReadResult obj = {.identificationObject = {{{0}}}}; /*lint !e785 , C99 initialization, all zero! */
    const uint8_t EcuConfigCount = 1U;
    const struct FWU_hw_version_s  *hw_version;
    const struct FWU_sw_version_s  *sw_version;

    obj.status = DBPL_EcuStatusOK; /*Expected to be ok.*/
    obj.objIdNumber = objIdNumber;
    obj.objCount = EcuConfigCount;

    /* set pointer to address of hw and sw version structure location */
    hw_version = FWU_getHardwareVersion();
    sw_version = FWU_getSoftwareVersion();

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

/* typical FWU1 application with fwu production messages */
struct DBPL_SwSubmoduleReadResult DBPL_tGetSwSubmoduleReadResult(const uint8_t * const pucSwID, uint8_t objIdNumber)
{   
    struct DBPL_SwSubmoduleReadResult obj = 
    {
        .status = DBPL_EcuStatusOK,
        .objIdNumber   = objIdNumber,
        .objCount      = 0x00u,        
        .identificationObject =
        {
         .sw_version =
            {
                .ID       = {0,1,2,3,4,5,6,7},
                .major    = {0x12, 0x34},
                .minor    = {0x98, 0x76},
                .revision = {0xfe, 0xdc},
                .build    = {0xab, 0xcd, 0xef, 0x01}
            },
         .sw_submodule_version = 
            {
                .ID       = {0,1,2,3,4,5,6,7},
                .major    = {0x12, 0x34},
                .minor    = {0x98, 0x76},
                .revision = {0xfe, 0xdc},
                .build    = {0xab, 0xcd, 0xef, 0x01}
            }
        }
    };
    
    uint8_t SwSubmoduleCount = 0;
    enum MAL_ModuleEnum moduleID = MAL_NO_MODULE;
    const struct STDV_version *swVersion = NULL;
    const struct FWU_sw_version_s *subModuleVersion = NULL;
    const struct FWU_sw_version_s  *app_sw_version = FWU_getSoftwareVersion();;
    
    /*lint -e{927,826}  cast from 'const uint8_t *const' (aka 'const unsigned char *const') to 'const struct STDV_idArray *' */
    if(app_sw_version != NULL)
    {
        if(STDV_isIdEqual(&app_sw_version->version, (const struct STDV_idArray *)pucSwID) == true)
        {
            moduleID = MAL_PRODUCT_APP1;
            swVersion = &app_sw_version->version;
        }
    }

    // fetch submodule version
    if(moduleID != MAL_NO_MODULE)
    {
        SwSubmoduleCount = FWU_getSwSubmodCount(moduleID);
        subModuleVersion = FWU_getSwSubmodVersion(moduleID, objIdNumber);
    }

    // copy version data
    if(subModuleVersion != NULL)
    {
        obj.identificationObject.sw_submodule_version = subModuleVersion->version;

        obj.identificationObject.sw_version = *swVersion;

        obj.objCount = SwSubmoduleCount;
    }
    else
    {
        obj.status = DBPL_EcuStatusError;
    }
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
        /* Axivion Next Line MisraC2012-9.3: "Provide explicit initializer for each part of initialized entity" */
        .identificationObject = {{{0}}}
    };

    return dummy;
}

/* typical FWU1 application without fwu production messages */
struct DBPL_SwSubmoduleReadResult DBPL_tGetSwSubmoduleReadResult(const uint8_t * const pucSwID, uint8_t objIdNumber)
{   
    // adapt parameters below according your requirements
    
    /*lint -e818 Pointer parameter ... could be declared: pucSwID was not used */
    (void)*pucSwID; 
    
    struct DBPL_SwSubmoduleReadResult dummy = 
    {
        .status = DBPL_EcuStatusOK,
        .objIdNumber   = objIdNumber,
        .objCount      = 0x00u,
        /* Axivion Next Line MisraC2012-9.3: "Provide explicit initializer for each part of initialized entity" */
        .identificationObject = {{{0}}}
    };

    return dummy;    
}

#endif
