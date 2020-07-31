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

#ifndef PSOC4
    /* set with respect both to max. length of production data and granularity of platforms excluding PSOC4,
       DBPL_WRITE_BUFFER_SIZE must not be set to less than 32 bytes!!! */
    #define DBPL_WRITE_BUFFER_SIZE    32U
    #define DBPL_UNFLASHED_BYTE       0xFFU
#else
    #define DBPL_WRITE_BUFFER_SIZE    MIN_WRITEBLOCK_SIZE // PSOC4 granularity is 128B/256B
    #define DBPL_UNFLASHED_BYTE       0x00U
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

static bool DBPL_bWriteFlash(uint32_t ulAddr, uint32_t ulSize, const uint8_t* pucData);

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

    if(DBPL_bWriteFlash(PROD_DATA_HWVERSION_ADDR, DBPL_WRITE_ECU_BUFFER_SIZE, pucBuff) == true)    //lint !e934
    {
        eRet = DBPL_ProductionStatusOK;
    }
    return eRet;
}

enum DBPL_ProductionStatus DBPL_tWriteTracingId(const struct DBPL_TracingId* tracingId)
{
    enum DBPL_ProductionStatus eRet = DBPL_ProductionStatusFunctionError;

    if(DBPL_bWriteFlash(PROD_DATA_TRID_ADDR, sizeof(struct DBPL_TracingId), (const uint8_t*)tracingId) == true)
    {
        eRet = DBPL_ProductionStatusOK;
    }
    return eRet;
}

enum DBPL_ProductionStatus DBPL_tWriteProductionTime(const struct DBPL_ProductionTime* timeReceived, struct DBPL_ProductionTime* timeWritten)
{
    enum DBPL_ProductionStatus eRet = DBPL_ProductionStatusFunctionError;

    *timeWritten = *timeReceived;
    if(DBPL_bWriteFlash(PROD_DATA_TIME_ADDR, sizeof(struct DBPL_ProductionTime), (const uint8_t*)timeReceived) == true)
    {
        eRet = DBPL_ProductionStatusOK;
    }
    return eRet;
}

enum DBPL_ProductionStatus DBPL_tReadProductionTime(struct DBPL_ProductionTime* timeRead)
{
    UTI_vMemCopy((SDEF_FAR const uint8_t*)PROD_DATA_TIME_ADDR, (uint8_t*)timeRead, (uint16_t)sizeof(struct DBPL_ProductionTime));    //lint !e923 Cast is tested to work correctly
    return DBPL_ProductionStatusOK;
}

enum DBPL_ProductionStatus DBPL_tWriteTestState(enum DBPL_TestState stateReceived, enum DBPL_TestState* stateWritten, uchar* repairCnt)
{
    *stateWritten = stateReceived;
    *repairCnt = 0U;
    return DBPL_ProductionStatusOK;
}

enum DBPL_ProductionStatus DBPL_tReadTestStateRepainrCnt(enum DBPL_TestState* stateWritten, uchar* repairCnt)
{
    *stateWritten = DBPL_TestStateOK;
    *repairCnt = 0U;
    return DBPL_ProductionStatusOK;
}

//lint -restore -e928

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
