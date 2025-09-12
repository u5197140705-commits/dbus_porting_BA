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
    \brief Public definitions and declarations for XS-Interface concerning production data
*/

#ifndef DBUS_DBUSPRESENTATION_PRODUCTION_H_
#define DBUS_DBUSPRESENTATION_PRODUCTION_H_
#ifdef __cplusplus
extern "C" {
#endif

#include "dbuspresentation_production_types.h"
#include "dbuspresentation_ecu_types.h"

/** \brief Function in XS-File to write Hw part of ecu config.*/
enum DBPL_ProductionStatus DBPL_tWriteEcuConfigHw(const struct STDV_version* const hwVersion);

/** \brief Function in XS-File to write tracing id.*/
enum DBPL_ProductionStatus DBPL_tWriteTracingId(const struct DBPL_TracingId* const tracingId);

/** \brief Function in XS-File to write production time.*/
enum DBPL_ProductionStatus DBPL_tWriteProductionTime(const struct DBPL_ProductionTime* const timeReceived, struct DBPL_ProductionTime* const timeWritten);

/** \brief Function in XS-File to read production time.*/
enum DBPL_ProductionStatus DBPL_tReadProductionTime(struct DBPL_ProductionTime* const timeRead);

/** \brief Function in XS-File to write test state.*/
enum DBPL_ProductionStatus DBPL_tWriteTestState(enum DBPL_TestState stateReceived, enum DBPL_TestState* const stateWritten, uint8_t* const repairCnt);

/** \brief Function in XS-File to read test state and repair count.*/
enum DBPL_ProductionStatus DBPL_tReadTestStateRepairCnt(enum DBPL_TestState* const stateRead, uint8_t* const repairCnt);

/** \brief Function in XS-File. Resets project specific values written during Factory Test.*/
void DBPL_vDoFactoryReset(void);

#if defined (DBUS2_APPLIANCE_MSG_IN_APP)
/** \brief Function in XS-File to read appliance data item referenced by parameterID.*/
enum DBPL_ApplianceDataStatus DBPL_tGetApplianceDataReadResult(enum DBPL_ApplianceDataItemId parameterID, uint8_t* const pucMessageData, uint8_t* const lengthOfDataReadOut);

/** \brief Function in XS-File to write appliance data item referenced by parameterID.*/
enum DBPL_ApplianceDataStatus DBPL_tWriteApplianceData(enum DBPL_ApplianceDataItemId parameterID, const uint8_t* const pucMessageData, uint8_t rawDataLength);

/** \brief Function returns maximal number of bytes - the declared size of item structure identified by parameterID */
uint8_t DBPL_GetApplianceDataMaxItemSize(enum DBPL_ApplianceDataItemId parameterID);

/** \brief Function returns address of appliance data item identified by parameterID */
const uint8_t* DBPL_GetApplianceDataItemAddress(enum DBPL_ApplianceDataItemId parameterID);

/** \brief Function returns enumeration type for appliance data item identified by parameterID */
enum DBPL_ApplianceDataItemId DBPL_GetParameterID(uint8_t value);
#endif /* end of DBUS2_APPLIANCE_MSG_IN_APP */

#ifdef __cplusplus
}
#endif


#endif /* DBUS_DBUSPRESENTATION_PRODUCTION_H_ */
