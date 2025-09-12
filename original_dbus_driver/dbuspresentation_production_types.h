/*******************************************************************************
*   Copyright (c) 2017 BSH Hausgeraete GmbH,
*   Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
*
*   All rights reserved. This program and the accompanying materials
*   are protected by international copyright laws.
*   Please contact copyright holder for licensing information.
*
********************************************************************************
*   PROJECT          IC DBUS
*   COMP_ABBREV      DBPL
*******************************************************************************/
/**************************************************************************************************/
/* DOCUMENTATION                                                                                  */
/**************************************************************************************************/
/** \file
    \brief Public definitions and declarations for types concerning information about production data.
*/
#ifndef DBUSPRESENTATION_PRODUCTION_TYPES_H_
#define DBUSPRESENTATION_PRODUCTION_TYPES_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "LibTypes.h"

/** \brief Responses to all production service messages go to this address.*/
#define DBPL_PRODUCTION_RESPONSE_ADDRESS 0xC0U
/** \brief Material Number in Tracing ID.*/
#define DBPL_TRID_MATERIAL_NUMBER_SIZE   10U
/** \brief Supplier ID in Tracing ID.*/
#define DBPL_TRID_SUPPLIER_ID_SIZE       10U
/** \brief Counter Size in Tracing ID.*/
#define DBPL_TRID_COUNTER_SIZE           9U
/** \brief Exact length of production date.*/
#define DBPL_DATE_OF_PRODUCTION_SIZE     6U
/** \brief Exact length of production time.*/
#define DBPL_CLOCK_OF_PRODUCTION_SIZE    4U
/** \brief Smallest invalid RepairCnt*/
#define DBPL_INVALID_REPAIR_CNT          8U

/* \brief  Appliance Data Item maximum size definitions according specification page  */
#define DBPL_APPLIANCE_DATA_DEVICE_TYPE_SIZE                  1U    // 1 unsigned character
#define DBPL_APPLIANCE_DATA_DEVICE_TYPE_STRING_SIZE          33U    // 32 characters + zero ending character
#define DBPL_APPLIANCE_DATA_BRAND_SIZE                       33U    // 32 characters + zero ending character
#define DBPL_APPLIANCE_DATA_BMRK_SIZE                         3U    // 3 ASCII characters
#define DBPL_APPLIANCE_DATA_VIB_SIZE                         33U    // 32 characters + zero ending character
#define DBPL_APPLIANCE_DATA_KIALPHANUMERIC_SIZE               3U    // 2 ASCII characters + zero ending character
#define DBPL_APPLIANCE_DATA_FD_NUMBER_SIZE                    4U    // 4 ASCII characters
#define DBPL_APPLIANCE_DATA_SERIAL_NUMBER_SIZE               65U    // 64 characters + zero ending character
#define DBPL_APPLIANCE_DATA_MANUFACTURING_TIMESTAMP_SIZE     13U    // YYYYMMDDhhmm +  zero ending character
#define DBPL_APPLIANCE_DATA_COUNTRYSETTINGS_SIZE              1U    // 1 unsigned character
#define DBPL_APPLIANCEDATA_BLE_TX_POWER_SIZE                  2U    // 2 character, signed value

#define DBPL_MAX_APPLIANCE_DATA_SIZE                         65U    // maximum value of all appliance data sizes


/* macro calculates aligned size of structure item defined by pure_size, it uses derivative specific value MIN_WRITEBLOCK_SIZE to add bytes for aligning */
/*lint -save -e9026 Function-like macro... [MISRA 2012 Directive 4.9, advisory] */
#define ALIGNED_SIZE(pure_size)    ((((pure_size) % (MIN_WRITEBLOCK_SIZE)) == 0u) ? (pure_size) : ((pure_size) + (MIN_WRITEBLOCK_SIZE) - ((pure_size) % (MIN_WRITEBLOCK_SIZE))))
/*lint -restore -e9026 */
/**
 * \brief Status of Production Commands
 * \details Discrete values DBPL_ProductionStatusOK, DBPL_ProductionStatusParamxError, DBPL_ProductionStatusLengthError, DBPL_ProductionStatusUnknownMsgError, DBPL_ProductionStatusFunctionError
 */
enum DBPL_ProductionStatus{
    DBPL_ProductionStatusOK = 0xA0U,
    DBPL_ProductionStatusParam1Error = 0xB1U,
    DBPL_ProductionStatusParam2Error = 0xB2U,
    DBPL_ProductionStatusParam3Error = 0xB3U,
    DBPL_ProductionStatusLengthError = 0xC0U,
    DBPL_ProductionStatusUnknownMsgError = 0xC1U,
    DBPL_ProductionStatusFunctionError = 0xD0U
};

struct DBPL_TracingId{
    uint8_t materialNumber[DBPL_TRID_MATERIAL_NUMBER_SIZE];
    uint8_t supplierId[DBPL_TRID_SUPPLIER_ID_SIZE];
    uint8_t counter[DBPL_TRID_COUNTER_SIZE];
};

/**
 * \brief Production Time
 * \details Contains day and time of the day.
 */
struct DBPL_ProductionTime{
    uint8_t date[DBPL_DATE_OF_PRODUCTION_SIZE];
    /*Axivion Next Line MisraC2012-21.2 : yes, clock is reserved name, but it is not in global name space */
    uint8_t clock[DBPL_CLOCK_OF_PRODUCTION_SIZE];
};  /*lint -esym(9045,*DBPL_ProductionTime) "non-hidden definition of type 'struct DBPL_ProductionTime'" */
/**
 * \brief Status of Appliance data Commands
 * \details Discrete values
 *      DBPL_ApplianceDataStatusOK
 *      DBPL_ApplianceDataLengthError
 *      DBPL_ApplianceDataFunctionError
 *      DBPL_ApplianceDataEmptyItemError
 *      DBPL_ApplianceDataUnknownParamterIdError
 */
enum DBPL_ApplianceDataStatus{
    DBPL_ApplianceDataStatusOK = 0xA0U,
    DBPL_ApplianceDataLengthError = 0xC0U,
    DBPL_ApplianceDataFunctionError = 0xD0U,
    DBPL_ApplianceDataEmptyItemError = 0xD1U,
    DBPL_ApplianceDataUnknownParamterIdError = 0xE0U
};
/**
 * \brief Appliance Data
 * \details Appliance Data structure contains items
 *          deviceType
 *          deviceTypeString
 *          brand
 *          BMrk
 *          vib
 *          kiAlphanumeric
 *          fdNumber
 *          serialNumber
 *          manufacturingTimestamp
 *          countrySettings
 *          bleTxPower
 */
/*lint -save -e506 -e778 -e9018*/
/* Warning 506: Constant value Boolean
 * Info 778: Constant expression evaluates to 0 in operation '%'
 * A desired calculation of aligned size in macro was tested and it is working well 
 * Note 9018: union declared [MISRA 2012 Rule 19.2, advisory] */
struct DBPL_ApplianceData{
    union
    {
        uint8_t pureItem; //!< uint8 - [32;191]
        uint8_t alignedItem[ALIGNED_SIZE(DBPL_APPLIANCE_DATA_DEVICE_TYPE_SIZE)];
    }deviceType;
    union
    {
        uint8_t pureItem[DBPL_APPLIANCE_DATA_DEVICE_TYPE_STRING_SIZE]; //!< string - UTF-8 encoded, \0 terminated, 1-32 characters
        uint8_t alignedItem[ALIGNED_SIZE(DBPL_APPLIANCE_DATA_DEVICE_TYPE_STRING_SIZE)];

    }deviceTypeString;
    union
    {
        uint8_t pureItem[DBPL_APPLIANCE_DATA_BRAND_SIZE]; //!< string - UTF-8 encoded, \0 terminated, 1-32 characters
        uint8_t alignedItem[ALIGNED_SIZE(DBPL_APPLIANCE_DATA_BRAND_SIZE)];
    }brand;
    union
    {
        uint8_t pureItem[DBPL_APPLIANCE_DATA_BMRK_SIZE]; //!< Panel brand, 3 ASCII characters
        uint8_t alignedItem[ALIGNED_SIZE(DBPL_APPLIANCE_DATA_BMRK_SIZE)];
    }BMrk;
    union
    {
        uint8_t pureItem[DBPL_APPLIANCE_DATA_VIB_SIZE]; //!< string- UTF-8 encoded, \0 terminated, 1-32 characters
        uint8_t alignedItem[ALIGNED_SIZE(DBPL_APPLIANCE_DATA_VIB_SIZE)];
    }vib;
    union
    {
        uint8_t pureItem[DBPL_APPLIANCE_DATA_KIALPHANUMERIC_SIZE]; //!< customer service index in string - 2 ASCII characters
        uint8_t alignedItem[ALIGNED_SIZE(DBPL_APPLIANCE_DATA_KIALPHANUMERIC_SIZE)];
    }kiAlphanumeric;
    union
    {
        uint8_t pureItem[DBPL_APPLIANCE_DATA_FD_NUMBER_SIZE]; //!< 4 ASCII characters
        uint8_t alignedItem[ALIGNED_SIZE(DBPL_APPLIANCE_DATA_FD_NUMBER_SIZE)];
    }fdNumber;
    union
    {
        uint8_t pureItem[DBPL_APPLIANCE_DATA_SERIAL_NUMBER_SIZE]; //!< string - UTF-8 encoded, \0 terminated, 0-64 characters
        uint8_t alignedItem[ALIGNED_SIZE(DBPL_APPLIANCE_DATA_SERIAL_NUMBER_SIZE)];
    }serialNumber;
    union
    {
        uint8_t pureItem[DBPL_APPLIANCE_DATA_MANUFACTURING_TIMESTAMP_SIZE]; //<! string UTF-8 - encoded, \0 terminated, format YYYYMMDDhhmm
        uint8_t alignedItem[ALIGNED_SIZE(DBPL_APPLIANCE_DATA_MANUFACTURING_TIMESTAMP_SIZE)];
    }manufacturingTimestamp;
    union
    {
        uint8_t pureItem; //!< Value Country or region: 0 WW (Worldwide), 1 US (USA and North America), 2 EU/AP (Europe, Asia Pacific without China), 3  CN (China)
        uint8_t alignedItem[ALIGNED_SIZE(DBPL_APPLIANCE_DATA_COUNTRYSETTINGS_SIZE)];
    }countrySettings;
    union
    {
        uint8_t pureItem; //!< Value BLE Tx Power level, int16 (Two Bytes), range: -30 to 200, default: 80
        uint8_t alignedItem[ALIGNED_SIZE(DBPL_APPLIANCEDATA_BLE_TX_POWER_SIZE)];
    }bleTxPower;
};
/*lint -restore */

/**
 * \brief   Appliance Data parameters ID
 * \details Appliance Data parameters ID is used for identify parameter by number
*/
enum DBPL_ApplianceDataItemId{
    DBPL_APPLIANCEDATA_DEVICETYPE,
    DBPL_APPLIANCEDATA_DEVICETYPESTRING,
    DBPL_APPLIANCEDATA_BRAND,
    DBPL_APPLIANCEDATA_BMRK,
    DBPL_APPLIANCEDATA_VIB,
    DBPL_APPLIANCEDATA_KIALPHANUMERIC,
    DBPL_APPLIANCEDATA_FD,
    DBPL_APPLIANCEDATA_SERIALNUMBER,
    DBPL_APPLIANCEDATA_MANUFACTURINGTIMESTAMP,
    DBPL_APPLIANCEDATA_COUNTRYSETTINGS,
    DBPL_APPLIANCEDATA_BLE_TX_POWER,
    DBPL_APPLIANCEDATA_COUNT
};

struct DBPL_ApplianceDataItemAttr{
    bool stringItem;            ///< A kind of appliance data item, string (true), other type (false)
    uint8_t maxSize;            ///< Maximum size of appliance data item
    size_t itemOffset;          ///< Offset of appliance data item from start of structure
};

enum DBPL_TestState{
    DBPL_TestStateFailed = 0U,
    DBPL_TestStateOK = 1U,
    DBPL_TestStateCnt = 2U
};

#ifdef __cplusplus
}
#endif


#endif /* DBUSPRESENTATION_PRODUCTION_TYYPES_H_ */
