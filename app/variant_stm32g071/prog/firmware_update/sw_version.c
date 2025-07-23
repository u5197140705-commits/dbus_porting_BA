/*******************************************************************************
 *  Copyright (c) 2025 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          application SW ID module
 ******************************************************************************/


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file sw_version.c
 *
 *  \brief    Implementation of SW module versions.
 *
 *  \details  Template file for SW module versions.
 */


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "version/fwu_version.h"
#include "build_version.h"
#include "std_lib/std_symbol.h"
#include "firmware_update/BootManager/BootManagerModule.h"

/******************************************************************************/
/* PRIVATE DATA DEFINITIONS                                                   */
/******************************************************************************/
  
// number of versions in bootmanager submodule version table
#if defined(FEATURE_FUNCTIONAL_SAFETY_ENABLED)
  #define FWU_SUBMODULE_CNTS         2u
#else
  #define FWU_SUBMODULE_CNTS         1u
#endif

/** \brief    SW ID structure
 *
 *  \details  SW version structure defined by project
 */

/* 
If fixed position of SW_version within application is needed, create
corresponding segment in linkerscript file and uncomment following lines.
*/
// SDEF_SetSegmentConst(APP_SWVERSION)
SYMBOL_USED const struct FWU_sw_version_s SW_version =
{
    .version =
    { /** \todo please specify id-string for your application */
        .ID       = STDV_ParseIdentifier(UID),            // ID    defined by build process
        .major    = STDV_ParseVersion(VERSION_MAJOR),     // major defined by build process
        .minor    = STDV_ParseVersion(VERSION_MINOR),     // minor defined by build process
        .revision = STDV_ParseVersion(VERSION_REVISION),  // revision defined by build process
        .build    = STDV_ParseVersionBuild(VERSION_BUILD) // build defined by build process
    }
};
// SDEF_SetSegmentConst_Default()

#if defined(APP_VARIANT)
/** \brief  SW component version table, this table could be extended by application */
SYMBOL_USED const struct FWU_SubmoduleVersion_s FWU_versionsInApp =
{
    .submoduleCount = FWU_SUBMODULE_CNTS,
    //The MCU Framework version is stored in the Application
    {
      {
        .version = {
          .ID       = {'M','c','u','F','W',' ',' ',' '},           // ID       "McuFW   "
          .major    = STDV_ParseVersion(MCU_FW_VERSION_MAJOR),     // major    defined by build process
          .minor    = STDV_ParseVersion(MCU_FW_VERSION_MINOR),     // minor    defined by build process
          .revision = STDV_ParseVersion(MCU_FW_VERSION_REVISION),  // revision currently not used
          .build    = STDV_ParseVersionBuild(0u)                   // build    currently not used
        }
      }
      
#if defined(FEATURE_FUNCTIONAL_SAFETY_ENABLED)
      //fs component version in application
      ,{
        .version = {
          .ID       = {'F','S','_','A','P','P',' ',' '},           // ID       "FS_APP  "
          .major    = STDV_ParseVersion(FS_COMP_VERSION_MAJOR),    // major    defined by build process
          .minor    = STDV_ParseVersion(FS_COMP_VERSION_MINOR),    // minor    defined by build process
          .revision = STDV_ParseVersion(FS_COMP_VERSION_REVISION), // revision defined by build process
          .build    = STDV_ParseVersionBuild(0u)                   // build    currently not used
        }
      }
#endif
    }
};
#endif /* defined(APP_VARIANT) */
