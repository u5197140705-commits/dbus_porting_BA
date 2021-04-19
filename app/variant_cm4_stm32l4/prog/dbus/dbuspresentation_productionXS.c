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
#include "dbuspresentation_production.h"
#include "firmware_update/mem_drv/intflash_drv.h"
#include "firmware_update/fwu_version.h"
#include "utility.h"
#include "SegmentDef.h"

#define CRC_SIZE                    sizeof(CRC_crc_t)
#define STDV_VERSION_SIZE           sizeof(struct STDV_version)
#define DBPL_WRITE_ECU_BUFFER_SIZE  (STDV_VERSION_SIZE + CRC_SIZE)

#if defined (PSOC4)
    #define DBPL_WRITE_BUFFER_SIZE    MIN_WRITEBLOCK_SIZE // PSOC4 granularity is 128B/256B
    #define DBPL_UNFLASHED_BYTE       0x00U
#elif defined (EFR32BG21)
/* for EFR32 platform as base for BTM the DBPL_WRITE_BUFFER_SIZE must be set
 * with respect both to max. length of appliance data items and granularity of platform.
 */
    #define DBPL_WRITE_BUFFER_SIZE    80U
    #define DBPL_UNFLASHED_BYTE       0xFFU

#else
    /* set with respect both to max. length of production data and granularity of platforms excluding PSOC4,
       DBPL_WRITE_BUFFER_SIZE must not be set to less than 32 bytes!!! */
    #define DBPL_WRITE_BUFFER_SIZE    32U
    #define DBPL_UNFLASHED_BYTE       0xFFU
#endif

/* These dummy definitions are here to avoid problems with FWU1 build. */
#ifndef PROD_DATA_HWVERSION_ADDR
    #define PROD_DATA_HWVERSION_ADDR    (UL_INVALID)
#endif
#ifndef PROD_DATA_TRID_ADDR
    #define PROD_DATA_TRID_ADDR         (UL_INVALID)
#endif
#ifndef PROD_DATA_TIME_ADDR
    #define PROD_DATA_TIME_ADDR         (UL_INVALID)
#endif
#ifndef APPLIANCE_DATA_ADDR
    #define APPLIANCE_DATA_ADDR         (UL_INVALID)
#endif




static bool DBPL_bWriteFlash(uint32_t ulAddr, uint32_t ulSize, const uint8_t* pucData);
static uint8_t DBPL_CopyItem(const uint8_t* pucSource, uint8_t* pucDestination, uint8_t maxSize);
static uint32_t DBPL_CheckIsErased(uint32_t startingAddress, uint32_t size);

/* Appliance Data Item's attributes Array consists of parameter ID number, maximum size of item and start address of item.
 * A data alignment in flash is considered, therefore the union member 'alignedItem' is used for start address
 */
/* Switch off: Note 923: cast from unsigned int to pointer [MISRA 2012 Rule 11.1, required], [MISRA 2012 Rule 11.4, required], [MISRA 2012 Rule 11.6, required]
 * It was checked and it works well */
/*lint -save -e923 */
const struct DBPL_ApplianceDataItemAttr DBPL_ApplianceDataAttributes[DBPL_APPLIANCEDATA_MAX_ID_NUMBER] =
{
/* Array index is Appliance Data parameters ID, string item?, Maximum Appliance Data Item's Size Array, Appliance Data Item's Address */
[DBPL_APPLIANCEDATA_DEVICETYPE] = { false , DBPL_APPLIANCE_DATA_DEVICE_TYPE_SIZE, &(((struct DBPL_ApplianceData*)APPLIANCE_DATA_ADDR)->deviceType.alignedItem[0]) },
[DBPL_APPLIANCEDATA_DEVICETYPESTRING] = { true , DBPL_APPLIANCE_DATA_DEVICE_TYPE_STRING_SIZE, &(((struct DBPL_ApplianceData*)APPLIANCE_DATA_ADDR)->deviceTypeString.alignedItem[0]) },
[DBPL_APPLIANCEDATA_BRAND] = { true, DBPL_APPLIANCE_DATA_BRAND_SIZE, &(((struct DBPL_ApplianceData*)APPLIANCE_DATA_ADDR)->brand.alignedItem[0]) },
[DBPL_APPLIANCEDATA_BMRK] = { false, DBPL_APPLIANCE_DATA_BMRK_SIZE, &(((struct DBPL_ApplianceData*)APPLIANCE_DATA_ADDR)->BMrk.alignedItem[0]) },
[DBPL_APPLIANCEDATA_VIB] = { true , DBPL_APPLIANCE_DATA_VIB_SIZE, &(((struct DBPL_ApplianceData*)APPLIANCE_DATA_ADDR)->vib.alignedItem[0]) },
[DBPL_APPLIANCEDATA_KIALPHANUMERIC] = { true, DBPL_APPLIANCE_DATA_KIALPHANUMERIC_SIZE, &(((struct DBPL_ApplianceData*)APPLIANCE_DATA_ADDR)->kiAlphanumeric.alignedItem[0]) },
[DBPL_APPLIANCEDATA_FD] = { false, DBPL_APPLIANCE_DATA_FD_NUMBER_SIZE, &(((struct DBPL_ApplianceData*)APPLIANCE_DATA_ADDR)->fdNumber.alignedItem[0]) },
[DBPL_APPLIANCEDATA_SERIALNUMBER] = { true, DBPL_APPLIANCE_DATA_SERIAL_NUMBER_SIZE, &(((struct DBPL_ApplianceData*)APPLIANCE_DATA_ADDR)->serialNumber.alignedItem[0]) },
[DBPL_APPLIANCEDATA_MANUFACTURINGTIMESTAMP] = {true , DBPL_APPLIANCE_DATA_MANUFACTURING_TIMESTAMP_SIZE, &(((struct DBPL_ApplianceData*)APPLIANCE_DATA_ADDR)->manufacturingTimestamp.alignedItem[0]) },
[DBPL_APPLIANCEDATA_COUNTRYSETTINGS] = { false , DBPL_APPLIANCE_DATA_COUNTRYSETTINGS_SIZE, &(((struct DBPL_ApplianceData*)APPLIANCE_DATA_ADDR)->countrySettings.alignedItem[0]) },
};
/*lint -restore */

/* Switch off: Note 928: cast from pointer to pointer */
/*lint -save -e928 */

enum DBPL_ProductionStatus DBPL_tWriteEcuConfigHw(const struct STDV_version* hwVersion)
{
    enum DBPL_ProductionStatus eRet = DBPL_ProductionStatusFunctionError;
    uint8_t pucBuff[DBPL_WRITE_ECU_BUFFER_SIZE];
    const uint8_t* pucHwVersion = (const uint8_t*)hwVersion;
    CRC_crc_t HW_ID_crc = CRC_Init();

    for(uint8_t i = 0u; i < STDV_VERSION_SIZE; i++)
    {
        HW_ID_crc = CRC_UpdateByte(HW_ID_crc, *pucHwVersion);
        pucHwVersion++;
    }
    HW_ID_crc = CRC_Finish(HW_ID_crc);

    UTI_vMemCopy((SDEF_FAR const uint8_t*)&HW_ID_crc, pucBuff, (uint16_t)CRC_SIZE);    //lint !e934 Taking address of near auto variable, tested to work correctly
    UTI_vMemCopy((SDEF_FAR const uint8_t*)hwVersion, &pucBuff[CRC_SIZE], (uint16_t)STDV_VERSION_SIZE);    //lint !e934

    if(DBPL_CheckIsErased((uint32_t)PROD_DATA_HWVERSION_ADDR, DBPL_WRITE_ECU_BUFFER_SIZE))
    {
        if(DBPL_bWriteFlash(PROD_DATA_HWVERSION_ADDR, DBPL_WRITE_ECU_BUFFER_SIZE, pucBuff) == true)    //lint !e934
        {
            eRet = DBPL_ProductionStatusOK;
        }
    }
    return eRet;
}

enum DBPL_ProductionStatus DBPL_tWriteTracingId(const struct DBPL_TracingId* tracingId)
{
    enum DBPL_ProductionStatus eRet = DBPL_ProductionStatusFunctionError;

    if(DBPL_CheckIsErased((uint32_t)PROD_DATA_TRID_ADDR, sizeof(struct DBPL_TracingId)))
    {
        if(DBPL_bWriteFlash(PROD_DATA_TRID_ADDR, sizeof(struct DBPL_TracingId), (const uint8_t*)tracingId) == true)
        {
            eRet = DBPL_ProductionStatusOK;
        }
    }
    return eRet;
}

enum DBPL_ProductionStatus DBPL_tWriteProductionTime(const struct DBPL_ProductionTime* timeReceived, struct DBPL_ProductionTime* timeWritten)
{
    enum DBPL_ProductionStatus eRet = DBPL_ProductionStatusFunctionError;

    *timeWritten = *timeReceived;

    if(DBPL_CheckIsErased((uint32_t)PROD_DATA_TIME_ADDR, sizeof(struct DBPL_ProductionTime)))
    {
        if(DBPL_bWriteFlash(PROD_DATA_TIME_ADDR, sizeof(struct DBPL_ProductionTime), (const uint8_t*)timeReceived) == true)
        {
            eRet = DBPL_ProductionStatusOK;
        }
    }
    return eRet;
}

enum DBPL_ProductionStatus DBPL_tReadProductionTime(struct DBPL_ProductionTime* timeRead)
{
    UTI_vMemCopy((SDEF_FAR const uint8_t*)PROD_DATA_TIME_ADDR, (uint8_t*)timeRead, (uint16_t)sizeof(struct DBPL_ProductionTime));    //lint !e923 Cast is tested to work correctly
    return DBPL_ProductionStatusOK;
}

enum DBPL_ProductionStatus DBPL_tWriteTestState(enum DBPL_TestState stateReceived, enum DBPL_TestState* stateWritten, uint8_t* repairCnt)
{
    *stateWritten = stateReceived;
    *repairCnt = 0U;
    return DBPL_ProductionStatusOK;
}

enum DBPL_ProductionStatus DBPL_tReadTestStateRepainrCnt(enum DBPL_TestState* stateWritten, uint8_t* repairCnt)
{
    *stateWritten = DBPL_TestStateOK;
    *repairCnt = 0U;
    return DBPL_ProductionStatusOK;
}

//lint -restore -e928


enum DBPL_ApplianceDataStatus DBPL_tGetApplianceDataReadResult(enum DBPL_ApplianceDataItemId parameterID, uint8_t* pucMessageData, uint8_t* lengthOfDataReadOut)
{
    enum DBPL_ApplianceDataStatus retval = DBPL_APPLIANCEDATA_FUNCTION_ERROR;

    const uint8_t* pItemAddress;
    pItemAddress = DBPL_GetApplianceDataItemAddress(parameterID);
    /*A checking if parameterID is lower then DBPL_APPLIANCEDATA_MAX_ID_NUMBER is provided in higher level)*/
    if(parameterID == DBPL_APPLIANCEDATA_COUNTRYSETTINGS)
    {
    /* special case: 1-byte sized item may contain 0x0 and thus function DBPL_CopyItem cannot be used */
        *pucMessageData = *pItemAddress;
        *lengthOfDataReadOut = 1U;
        retval = DBPL_APPLIANCEDATA_STATUS_OK;
    }
    else
    {
        *lengthOfDataReadOut = DBPL_CopyItem(pItemAddress, pucMessageData, DBPL_GetApplianceDataMaxItemSize(parameterID));
        if(DBPL_ApplianceDataAttributes[parameterID].stringItem)
        {
            if (*lengthOfDataReadOut != 0U)
            {
                if(*lengthOfDataReadOut < DBPL_GetApplianceDataMaxItemSize(parameterID))
                {
                    retval = DBPL_APPLIANCEDATA_STATUS_OK;
                }
                else
                {
                    retval = DBPL_APPLIANCEDATA_FUNCTION_ERROR;
                }
            }
            else
            {
                retval = DBPL_APPLIANCEDATA_EMPTY_ITEM_ERROR;
            }
        }
        else
        {
            if(*lengthOfDataReadOut > 0U)
            {
                retval = DBPL_APPLIANCEDATA_STATUS_OK;
            }

        }
    }
    return retval;
}

enum DBPL_ApplianceDataStatus DBPL_tWriteApplianceData(enum DBPL_ApplianceDataItemId parameterID, uint8_t* pucMessageData, uint8_t rawDataLength)
{
    enum DBPL_ApplianceDataStatus retval = DBPL_APPLIANCEDATA_FUNCTION_ERROR;

    const uint8_t* pucItemAddress;
    uint8_t itemSize;
    uint8_t* pucBegining;
    pucBegining = pucMessageData;

    pucItemAddress = DBPL_GetApplianceDataItemAddress(parameterID);
    itemSize = DBPL_GetApplianceDataMaxItemSize(parameterID);
    if(DBPL_CheckIsErased((uint32_t)pucItemAddress, DRVIF_driver.writeBlockSize) != 0u) //lint !e923 Cast from pointer to unsigned int; tested, working good
    {
        if(DBPL_ApplianceDataAttributes[parameterID].stringItem)
        {
            if(rawDataLength <= itemSize)
            {
                /* adding zero character at the end of data to have string format */
                for( uint8_t i = 0 ; i < rawDataLength ; i++ )
                {
                    pucMessageData++;
                }
                *pucMessageData = 0U; /* adding '\0' character at the end */
                pucMessageData = pucBegining;
                rawDataLength++;
                if(DBPL_bWriteFlash((uint32_t)pucItemAddress, (uint32_t)rawDataLength, (const uint8_t*)pucMessageData) == true)  //lint !e923 !e926  Cast from pointer to unsigned int; cast from pointer to pointer, tested, working good
                {
                    retval = DBPL_APPLIANCEDATA_STATUS_OK;
                }
            }
            else
            {
                retval = DBPL_APPLIANCEDATA_LENGTH_ERROR;
            }
        }
        else
        {
            if(rawDataLength == itemSize)
            {
                if(DBPL_bWriteFlash((uint32_t)pucItemAddress, (uint32_t)rawDataLength, (const uint8_t*)pucMessageData) == true) //lint !e923 !e926  Cast from pointer to unsigned int; cast from pointer to pointer, tested, working good
                {
                    retval = DBPL_APPLIANCEDATA_STATUS_OK;
                }
            }
            else
            {
                retval = DBPL_APPLIANCEDATA_LENGTH_ERROR;
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

/* The function returns a start address of appliance data item identified by parameterID */
const uint8_t * DBPL_GetApplianceDataItemAddress(enum DBPL_ApplianceDataItemId parameterID)
{
    return DBPL_ApplianceDataAttributes[parameterID].startAddress;
}

/* DBPL_CopyItem function copies string without ending \0 character and it returns real size of item
 * the copy process ends before \0 and it should not overrun maximum size of item
 */
static uint8_t DBPL_CopyItem(const uint8_t* pucSource, uint8_t* pucDestination, uint8_t maxSize)
{
    uint8_t count = 0u;

    while((*pucSource != 0U) && (count < maxSize))
    {
        *pucDestination++ = *pucSource++; //lint !e9049 increment/decrement operation combined with other operation with side-effects, tested, works well, no side-effects
        count++;
    }
    return count;
}


enum DBPL_ApplianceDataItemId DBPL_GetParameterID(uint8_t value)
{
    enum DBPL_ApplianceDataItemId retval;
    if(value <= (uint8_t)DBPL_APPLIANCEDATA_COUNTRYSETTINGS)
    {
        /* all values between 0 and 9  are valid enumerations*/
        retval = (enum DBPL_ApplianceDataItemId) value; //lint !e9030 !e9034 Impermissible cast; cannot cast from 'essentially unsigned' to 'essentially enum<i>' Lint: Note 9034: Expression assigned to a narrower or different essential type [MISRA 2012 Rule 10.3, required]
    }
    else
    {
        retval = DBPL_APPLIANCEDATA_MAX_ID_NUMBER;
    }
    return retval;
}

static uint32_t DBPL_CheckIsErased(uint32_t startingAddress, uint32_t size)
{
    return DRVIF_checkErase(startingAddress, size);
}

static bool DBPL_bWriteFlash(uint32_t ulAddr, uint32_t ulSize, const uint8_t* pucData)
{
    bool bRet = false;
    uint32_t ulFlashedBytes;
    uint32_t ulWrSize;
    uint8_t buffer[DBPL_WRITE_BUFFER_SIZE];
    const uint32_t buffer_size = sizeof(buffer);
    const uint8_t* pucWrData;

#ifndef PSOC4
    /* In case of platforms with small granularity, data to write is aligned to the multiple of granularity */
    ulWrSize = ((ulSize % MIN_WRITEBLOCK_SIZE) == 0u) ? ulSize : (ulSize + MIN_WRITEBLOCK_SIZE - (ulSize % MIN_WRITEBLOCK_SIZE));
#else
    ulWrSize = DBPL_WRITE_BUFFER_SIZE;
#endif

    /* protection condition to avoid overflowing the buffer */
    if((buffer_size >= ulSize) && (buffer_size >= ulWrSize))    //lint !e774 Right-side condition of '&&' is always true, issue relevant only for PSoC4 platform, tested to work correctly
    {
        /* copy message data to write buffer */
        for(uint8_t i = 0u; i < ulSize; i++)
        {
            buffer[i] = pucData[i];
        }

        /* fill unused space in write buffer with void symbols */
        if(ulWrSize > ulSize)
        {
            for(uint32_t i = ulSize; i < ulWrSize; i++)
            {
                buffer[i] = DBPL_UNFLASHED_BYTE;
            }
        }

        pucWrData = buffer;    //lint !e934 Taking address of near auto variable, tested to work correctly

        DRVIF_initFlash();
        if(DRVIF_isBusy() == false)
        {
            while(ulWrSize > 0u)
            {
                bRet = false;
                ulFlashedBytes = DRVIF_write(ulAddr, ulWrSize, pucWrData);
                while(DRVIF_isBusy() == true) {}
                if(DRVIF_isError() != false){break;}
                if(ulFlashedBytes == 0u){break;}
                bRet = true;
                ulAddr    += ulFlashedBytes;
                pucWrData += ulFlashedBytes;
                if(ulFlashedBytes > ulWrSize){break;}
                ulWrSize -= ulFlashedBytes;
            }
        }
        DRVIF_deinitFlash();
    }
    return bRet;
}
