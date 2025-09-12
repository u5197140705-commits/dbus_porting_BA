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
    \brief Public definitions and declarations for types concerning information about ecu.
*/


#ifndef COMMON_PROG_DBUS_DBUSPRESENTATION_ECU_TYPES_H_
#define COMMON_PROG_DBUS_DBUSPRESENTATION_ECU_TYPES_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "LibTypes.h"
#include "std_version.h"

/** \brief Identifiers of objects requested by EcuConfigReadRequest start with one, so zero is not a valid number.*/
#define DBPL_UPDATE_ECU_CONFIG_UNSUPPORTED_OBJ_ID_NUMBER  0U

/**
 * \brief Status of EcuConfigReadRequest  and EcuUniqueIdReadRequest given in response.
 * \details Discrete values DBPL_EcuStatusOK, DBPL_EcuStatusUpdateModeActive, DBPL_EcuStatusError, DBPL_EcuConfigUnsupported.
 */
enum DBPL_EcuStatus{
    DBPL_EcuStatusOK = 0U,
    DBPL_EcuStatusUpdateModeActive = 1U,
    DBPL_EcuStatusError = 32U,
    DBPL_EcuStatusUnsupported = 33U
};

/**
 * \brief Represents identification object of one instance of software on controller in EcuConfigReadResponse.
 * \details Contains version of both sw and hw. Version of hw must be identical for ALL objects on one ECU.
 */
struct DBPL_EcuIdentificationObject{
    struct STDV_version sw_version;
    struct STDV_version hw_version;
};

/**
 * \brief Represents identification object of one submodule of software on controller in SwSubmoduleReadResponse.
 * \details Contains version of ECU sw and submodule sw.
 */
struct DBPL_SubmoduleIdentificationObject{
    struct STDV_version sw_version;
    struct STDV_version sw_submodule_version;
};

/**
 * \brief Complete contents of EcuConfigReadResponse.
 * \details Contains status, number of object(objIdNumber), overall count of objects(objCount), and the object itself.
 */
struct DBPL_EcuConfigReadResult{
    enum DBPL_EcuStatus status;
    uint8_t objIdNumber;
    uint8_t objCount;
    struct DBPL_EcuIdentificationObject identificationObject;
}; /*lint -e9045 "non-hidden definition of type..." not hidden by intention */

/**
 * \brief Complete contents of SwSubmoduleReadResponse.
 * \details Contains status, number of object(objIdNumber), overall count of objects(objCount), and the object itself.
 */
struct DBPL_SwSubmoduleReadResult{
    enum DBPL_EcuStatus status;
    uint8_t objIdNumber;
    uint8_t objCount;
    struct DBPL_SubmoduleIdentificationObject identificationObject;
};

/**
 * \brief Complete contents of EcuUniqueIdReadResponse.
 * \details Contains status, length of uniqueId and zero-terminated uniqueId.
 */
struct DBPL_EcuUniqueIdReadResult{
    enum DBPL_EcuStatus status;
    uint8_t uniqueIdLen;
    uint8_t* const uniqueIdPtr;
};

#ifdef __cplusplus
}
#endif

#endif /* COMMON_PROG_DBUS_DBUSPRESENTATION_ECU_TYPES_H_ */
