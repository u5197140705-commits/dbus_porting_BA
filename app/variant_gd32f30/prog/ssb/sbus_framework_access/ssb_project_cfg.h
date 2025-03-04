/*******************************************************************************
 *  Copyright (c) 2022 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          Smart Sensor Bus
 *  COMP_ABBREV      SSB
 ******************************************************************************/

#ifndef SSB_PROJECT_CFG_H
#define SSB_PROJECT_CFG_H

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file     ssb_project_cfg.h
 *
 *  \ingroup  sbus_framework_access
 *
 *  \brief    Definition switches for configuring the SSB-project at a central place
 *
 */

// #define SSBCFG_MAIN__<MCU_Hardware>__<Sensor_Config>
//                        SSBCFG_MCU_HW   SSBCFG_SBUS

// Begin: ====== Choose only one of the following MCU-HW-platforms, =============
//        ====== containing used MCU-type, serial pins (I2C / SPI)  =============
//        ====== and sensor configuration                           =============

// #define SSBCFG_MAIN__STM32G071_I2C_HW0__IMUT_ST_ST_ST_ST_PUMU0
        ///< - Old DBus reference board with I2C connected and
        ///<   4 IMU+T-sensors
        ///< - MCU-HW for tests, with I2C, sensor config. of 1st sensor is used in PUMU0,
        ///<   only for tests of 4 Clients and sensors here)
        ///< - Has been placed into operation

// #define SSBCFG_MAIN__STM32G071_I2C_HW0__IMUT_ST_n_n_n_PUMU0
        ///< - Old DBus reference board with I2C connected and
        ///<   1 IMU+T-sensor
        ///< - MCU-HW for tests, with I2C, sensor config. is used in PUMU0)
        ///< - Has been placed into operation

// #define SSBCFG_MAIN__STM32G071_SPI_HW0__IMUT_ST_n_n_n_PUMU0
        ///< - Old DBus reference board with SPI connected and
        ///<   1 IMU+T-sensor
        ///< - MCU-HW for tests, with SPI, sensor config. is used in PUMU0)
        ///< - Still to be placed into operation

#define SSBCFG_MAIN__GD32F303_I2C_HW0__IMUT_ST_n_n_n_PUMU0
        ///< - GD32F303 evaluation board with I2C connected and
        ///<   1 IMU+T-sensor
        ///< - MCU-HW as used in BEA0, sensor config. used in PUMU0
        ///< - Has been placed into operation

// #define SSBCFG_MAIN__GD32F303_I2C_HW0__HUMID_BEA0
        ///< - GD32F303 evaluation board with I2C connected and
        ///<   humidity sensor
        ///< - All as used in BEA0
        ///< - Still to be placed into operation

// End:   ====== Choose the MCU-HW-platform =====================================

#if defined SSBCFG_MAIN__STM32G071_I2C_HW0__IMUT_ST_ST_ST_ST_PUMU0
    #define SSBCFG_I2C_USED
    #define SSBCFG_STM32G071_I2C_HW0
    #define SSBCFG_IMUT_ST_ST_ST_ST_PUMU0

#elif defined SSBCFG_MAIN__STM32G071_I2C_HW0__IMUT_ST_n_n_n_PUMU0
    #define SSBCFG_I2C_USED
    #define SSBCFG_STM32G071_I2C_HW0
    #define SSBCFG_IMUT_ST_n_n_n_PUMU0

#elif defined SSBCFG_MAIN__STM32G071_SPI_HW0__IMUT_ST_n_n_n_PUMU0
    #define SSBCFG_SPI_USED
    #define SSBCFG_STM32G071_SPI_HW0
    #define SSBCFG_IMUT_ST_n_n_n_PUMU0

#elif defined SSBCFG_MAIN__GD32F303_I2C_HW0__IMUT_ST_n_n_n_PUMU0
    #define SSBCFG_I2C_USED
    #define SSBCFG_GD32F303_I2C_HW0
    #define SSBCFG_IMUT_ST_n_n_n_PUMU0

#elif defined SSBCFG_MAIN__GD32F303_I2C_HW0__HUMID_BEA0
    #define SSBCFG_I2C_USED
    #define SSBCFG_GD32F303_I2C_HW0
    #define SSBCFG_HUMID_BEA0

#else
    #error "SSB-Error 1: Switch SSBCFG_MAIN__... missed in ssb_project_cfg.h"
#endif

#endif // From: #ifndef SSB_PROJECT_CFG_H

