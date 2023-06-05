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
 * \brief Application layer configuration for writing production data
 */

#include "bsh_stdinc.h"
#include "dbuspresentation_production.h"
#include "dbuspresentation.h"
#include "firmware_update/version/fwu_version.h"
#include "utility.h"
#include "stdcrc/StdCrc.h"
#include "firmware_update/otp_mem/fwu_otp_memory.h"


#if defined (DBUS2_APPLIANCE_MSG_IN_APP)
static uint8_t DBPL_CopyItem(const uint8_t* const pucSource, uint8_t* const pucDestination, uint8_t maxItemSize, bool isStringItem, bool* pEmptyItemSign);
#endif /* end of DBUS2_APPLIANCE_MSG_IN_APP */
/* Appliance Data Item's attributes Array consists of parameter ID number, maximum size of item and start address of item.
 * A data alignment in flash is considered, therefore the union member 'alignedItem' is used for start address
 */
/* Switch off: Note 923: cast from unsigned int to pointer [MISRA 2012 Rule 11.1, required], [MISRA 2012 Rule 11.4, required], [MISRA 2012 Rule 11.6, required]
 * It was checked and it works well */
/*lint -save -e923 */
#if defined (DBUS2_APPLIANCE_MSG_IN_APP)
static const struct DBPL_ApplianceDataItemAttr DBPL_ApplianceDataAttributes[(uint32_t)DBPL_APPLIANCEDATA_COUNT] =
{
/* Array index is Appliance Data parameters ID, string item?, Maximum Appliance Data Item's Size Array, Appliance Data Item's Address */
[DBPL_APPLIANCEDATA_DEVICETYPE] = { false , DBPL_APPLIANCE_DATA_DEVICE_TYPE_SIZE, offsetof(struct DBPL_ApplianceData, deviceType.alignedItem[0]) },
[DBPL_APPLIANCEDATA_DEVICETYPESTRING] = { true , DBPL_APPLIANCE_DATA_DEVICE_TYPE_STRING_SIZE, offsetof(struct DBPL_ApplianceData, deviceTypeString.alignedItem[0]) },
[DBPL_APPLIANCEDATA_BRAND] = { true, DBPL_APPLIANCE_DATA_BRAND_SIZE, offsetof(struct DBPL_ApplianceData, brand.alignedItem[0]) },
[DBPL_APPLIANCEDATA_BMRK] = { false, DBPL_APPLIANCE_DATA_BMRK_SIZE, offsetof(struct DBPL_ApplianceData, BMrk.alignedItem[0]) },
[DBPL_APPLIANCEDATA_VIB] = { true , DBPL_APPLIANCE_DATA_VIB_SIZE, offsetof(struct DBPL_ApplianceData, vib.alignedItem[0]) },
[DBPL_APPLIANCEDATA_KIALPHANUMERIC] = { true, DBPL_APPLIANCE_DATA_KIALPHANUMERIC_SIZE, offsetof(struct DBPL_ApplianceData, kiAlphanumeric.alignedItem[0]) },
[DBPL_APPLIANCEDATA_FD] = { false, DBPL_APPLIANCE_DATA_FD_NUMBER_SIZE, offsetof(struct DBPL_ApplianceData, fdNumber.alignedItem[0]) },
[DBPL_APPLIANCEDATA_SERIALNUMBER] = { true, DBPL_APPLIANCE_DATA_SERIAL_NUMBER_SIZE, offsetof(struct DBPL_ApplianceData, serialNumber.alignedItem[0]) },
[DBPL_APPLIANCEDATA_MANUFACTURINGTIMESTAMP] = {true , DBPL_APPLIANCE_DATA_MANUFACTURING_TIMESTAMP_SIZE, offsetof(struct DBPL_ApplianceData, manufacturingTimestamp.alignedItem[0]) },
[DBPL_APPLIANCEDATA_COUNTRYSETTINGS] = { false , DBPL_APPLIANCE_DATA_COUNTRYSETTINGS_SIZE, offsetof(struct DBPL_ApplianceData, countrySettings.alignedItem[0]) },
[DBPL_APPLIANCEDATA_BLE_TX_POWER] = { false , DBPL_APPLIANCEDATA_BLE_TX_POWER_SIZE, offsetof(struct DBPL_ApplianceData, bleTxPower.alignedItem[0]) },
};
#endif /* end of DBUS2_APPLIANCE_MSG_IN_APP */
/*lint -restore */

/* Switch off: Note 928: cast from pointer to pointer */
/* Switch off: Note 934: Taking address of near auto variable [MISRA 2012 Rule 1.3, required], not a problem on known MCUs  */
/*lint -save -e928 -e934*/
#if ((defined DBUS2_UPDATE_HSI) || (defined DBUS2_PROD_MSG_IN_APP))
enum DBPL_ProductionStatus DBPL_tWriteEcuConfigHw(const struct STDV_version* const hwVersion)
{
    struct FWU_hw_version_s fwuHwVersion;

    fwuHwVersion.CRC = CRC_Calc(sizeof(struct STDV_version), (const uint8_t*)hwVersion);
    fwuHwVersion.version = *hwVersion;
    
    if(OTP_writeHardwareVersion(&fwuHwVersion))
    {
        return DBPL_ProductionStatusOK;
    }
    else
    {
        return DBPL_ProductionStatusFunctionError;
    }
}

enum DBPL_ProductionStatus DBPL_tWriteTracingId(const struct DBPL_TracingId* const tracingId)
{
    if(OTP_writeTracingID(tracingId))
    {
        return DBPL_ProductionStatusOK;
    }
    else
    {
        return DBPL_ProductionStatusFunctionError;
    }
}

enum DBPL_ProductionStatus DBPL_tWriteProductionTime(const struct DBPL_ProductionTime* const timeReceived, struct DBPL_ProductionTime* const timeWritten)
{
    enum DBPL_ProductionStatus returnValue = DBPL_ProductionStatusFunctionError;

    if(OTP_writeProductionTime(timeReceived))
    {
        returnValue = DBPL_ProductionStatusOK;
    }
    (void)DBPL_tReadProductionTime(timeWritten);
    return returnValue;
}
#endif /* ((defined DBUS2_UPDATE_HSI) || (defined DBUS2_PROD_MSG_IN_APP)) */

enum DBPL_ProductionStatus DBPL_tReadProductionTime(struct DBPL_ProductionTime* const timeRead)
{
    const struct DBPL_ProductionTime* prodTime = OTP_getProductionTime();
    
    if(prodTime != NULL)
    {
        *timeRead = *prodTime;
        return DBPL_ProductionStatusOK;
    }
    else
    {
        return DBPL_ProductionStatusFunctionError;
    }
}

enum DBPL_ProductionStatus DBPL_tWriteTestState(enum DBPL_TestState stateReceived, enum DBPL_TestState* const stateWritten, uint8_t* const repairCnt)
{
    *stateWritten = stateReceived;
    *repairCnt = 0U;
    return DBPL_ProductionStatusOK;
}

enum DBPL_ProductionStatus DBPL_tReadTestStateRepairCnt(enum DBPL_TestState* const stateRead, uint8_t* const repairCnt)
{
    *stateRead = DBPL_TestStateOK;
    *repairCnt = 0U;
    return DBPL_ProductionStatusOK;
}

/*lint -e{818} In programmer second param could be const, but not in app!*/
void DBPL_tGetUniqueIdReadResult(uint8_t maxReadableUniqueIdLen, struct DBPL_EcuUniqueIdReadResult* const result)
{
#ifdef VARIANT_PROGRAMMER
    (void)maxReadableUniqueIdLen;
    (void)*result;
#else
    const struct DBPL_TracingId *pucTrid = OTP_getTracingID();
    result->uniqueIdLen = (uint8_t)sizeof(struct DBPL_TracingId);

    if(NULL == pucTrid)
    {
        /*TRID is not in OTP memory*/
        result->status = DBPL_EcuStatusError;
        result->uniqueIdLen = 0u;
    }
    else if(result->uniqueIdLen > maxReadableUniqueIdLen)
    {
        /*Does not fit into transmit buffer of Dbus*/
        result->status = DBPL_EcuStatusUnsupported;
        result->uniqueIdLen = 0u;
    }
    else
    {
        result->status = DBPL_EcuStatusOK;
        UTI_vMemCopy((const uint8_t*)pucTrid, result->uniqueIdPtr, (uint16_t)result->uniqueIdLen);
    }
#endif
}

/*lint -restore -e928 -e934*/
#if defined(DBUS2_APPLIANCE_MSG_IN_APP)
enum DBPL_ApplianceDataStatus DBPL_tGetApplianceDataReadResult(enum DBPL_ApplianceDataItemId parameterID, uint8_t* const pucMessageData, uint8_t* const lengthOfDataReadOut)
{
    enum DBPL_ApplianceDataStatus retval = DBPL_ApplianceDataFunctionError;
    const uint8_t* pItemAddress;
    uint8_t maxItemSize;
    bool isStringItem;
    bool isEmptyItem = true;
    /*A checking if parameterID is lower then DBPL_APPLIANCEDATA_COUNT is provided in higher level)*/
    pItemAddress = DBPL_GetApplianceDataItemAddress(parameterID);
    maxItemSize  = DBPL_GetApplianceDataMaxItemSize(parameterID);
    isStringItem = DBPL_ApplianceDataAttributes[parameterID].stringItem;
    *lengthOfDataReadOut = DBPL_CopyItem(pItemAddress, pucMessageData, maxItemSize, isStringItem, &isEmptyItem); //lint !e934 Taking address of near auto variable. Tested. The variable will be referenced, only as long as it exists on stack

    if (isEmptyItem == true)
    {
        retval = DBPL_ApplianceDataEmptyItemError;
    }
    else
    {
        if((*lengthOfDataReadOut == maxItemSize) && !isStringItem)
        {
            retval = DBPL_ApplianceDataStatusOK;
        }
        else if((*lengthOfDataReadOut <= maxItemSize) && isStringItem)
        {
            retval = DBPL_ApplianceDataStatusOK;
        }
        else
        {
            retval = DBPL_ApplianceDataFunctionError;
        }
    }
    return retval;
}

enum DBPL_ApplianceDataStatus DBPL_tWriteApplianceData(enum DBPL_ApplianceDataItemId parameterID, const uint8_t* const pucMessageData, uint8_t rawDataLength)
{
    uint8_t localMessageDataBuffer[DBPL_MAX_APPLIANCE_DATA_SIZE] = {0};
    enum DBPL_ApplianceDataStatus retval = DBPL_ApplianceDataFunctionError;

    const uint8_t* pucItemAddress;
    uint8_t maxItemSize;

    pucItemAddress = DBPL_GetApplianceDataItemAddress(parameterID);
    maxItemSize = DBPL_GetApplianceDataMaxItemSize(parameterID);

    for(uint8_t i = 0; i < rawDataLength; i++)
    {
        localMessageDataBuffer[i] = pucMessageData[i];
    }

    if(OTP_isEmpty(pucItemAddress, maxItemSize) != 0u) //lint !e923 Cast from pointer to unsigned int; tested, working good
    {
        if(DBPL_ApplianceDataAttributes[parameterID].stringItem == true)
        {
            if(rawDataLength <= maxItemSize)
            {
                /* adding zero character at the end of data to have string format */
                localMessageDataBuffer[rawDataLength] = 0U; /* adding '\0' character at the end */
                rawDataLength++;
                if(OTP_writeFlash(pucItemAddress, (uint32_t)rawDataLength, (const uint8_t*)localMessageDataBuffer) == true) //lint !e923 !e926 !e934 Cast from pointer to unsigned int; cast from pointer to pointer; taking address of near auto variable, tested, working good. The variable will be referenced, only as long as it exists on stack
                {
                    retval = DBPL_ApplianceDataStatusOK;
                }
            }
            else
            {
                retval = DBPL_ApplianceDataLengthError;
            }
        }
        else
        {
            if(rawDataLength == maxItemSize)
            {
                if(OTP_writeFlash(pucItemAddress, (uint32_t)rawDataLength, (const uint8_t*)localMessageDataBuffer) == true) //lint !e923 !e926 !e934 Cast from pointer to unsigned int; cast from pointer to pointer; taking address of near auto variable, tested, working good. The variable will be referenced, only as long as it exists on stack
                {
                    retval = DBPL_ApplianceDataStatusOK;
                }
            }
            else
            {
                retval = DBPL_ApplianceDataLengthError;
            }
        }
    }
    return retval;
}

/* The function returns a maximum size of appliance data item (in bytes) identified by parameterID */
uint8_t DBPL_GetApplianceDataMaxItemSize(enum DBPL_ApplianceDataItemId parameterID)
{
    return DBPL_ApplianceDataAttributes[parameterID].maxSize;
}

size_t DBPL_GetApplianceDataItemOffset(enum DBPL_ApplianceDataItemId parameterID)
{
    return DBPL_ApplianceDataAttributes[parameterID].itemOffset;
}

/* The function returns a start address of appliance data item identified by parameterID */
const uint8_t* DBPL_GetApplianceDataItemAddress(enum DBPL_ApplianceDataItemId parameterID)
{
    const struct DBPL_ApplianceData* applianceData = OTP_getApplianceData();
    return (const uint8_t*)applianceData + DBPL_GetApplianceDataItemOffset(parameterID);
}

/* DBPL_CopyItem function copies string without ending \0 character and it returns real size of item
 * the copy process ends before \0 and it should not overrun maximum size of item.
 * The function is also checking if item is empty or not and set appropriate sign variable
 */
static uint8_t DBPL_CopyItem(const uint8_t* const pucSource, uint8_t* const pucDestination, uint8_t maxItemSize, bool isStringItem, bool* pEmptyItemSign)
{
    uint8_t counter = 0u;
    *pEmptyItemSign = false;
    /* If first byte of appliance item is empty, we assume the whole appliance item is empty. */
    if(pucSource[counter] == OTP_UNFLASHED_BYTE)
    {
        *pEmptyItemSign = true;
    }
    /* The destination buffer is not set in case of empty string appliance items,
     * in other cases destination buffer will be filled */
    if((*pEmptyItemSign == false) || (isStringItem == false))
    {
        while(counter < maxItemSize)
        {
            pucDestination[counter] = pucSource[counter];
            counter++;
            if((isStringItem == true) && (pucSource[counter] == 0U))
            {
                break;
            }
        }
    }
    return counter;
}


enum DBPL_ApplianceDataItemId DBPL_GetParameterID(uint8_t value)
{
    enum DBPL_ApplianceDataItemId retval;
    if(value < (uint8_t)DBPL_APPLIANCEDATA_COUNT)
    {
        /* the value is valid enumeration DBPL_ApplianceDataItemId */
        retval = (enum DBPL_ApplianceDataItemId) value; /*lint !e9030 !e9034 Impermissible cast needed here and works.*/
    }
    else
    {
        retval = DBPL_APPLIANCEDATA_COUNT;
    }
    return retval;
}
#endif /* end of DBUS2_APPLIANCE_MSG_IN_APP */


/*lint -save -restore -e662 */
