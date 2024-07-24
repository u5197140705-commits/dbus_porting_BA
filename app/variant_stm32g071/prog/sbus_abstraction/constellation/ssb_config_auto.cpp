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
 *  COMP_ABBREV      SSB_CFG
 ******************************************************************************/

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file     ssb_config_auto.cpp
 *
 *  \ingroup  sbus_abstraction/constellation
 *
 *  \brief    Class containing the configurations done by the SSB-Configurator
 *
 *  \details
 */

/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/

#include "ssb_config_auto.h"

using namespace ::SSBAL::SSBCO;

#ifdef  SSB_CFG_AS_HUB_REGS_ONLY_WITH_1_HUB_ONLY
const uint8_t
SsbConfigurations_c::SsbConfigurationBytes[SSB_NUMBER_OF_CONFIGURATION_BYTES] =
{
    SSBAL_CFG_HUB_0,
    (uint8_t)0x00U, (uint8_t)0x09U, // Number of following bytes: high-byte, low-byte
    (uint8_t)0x33U, (uint8_t)0x11U, // Logical config. idx for Hub: high-byte, low-byte
    (uint8_t)0xAAU,                 // I2C-address
    (uint8_t)0x7EU, (uint8_t)0x4BU, // Hub register address, Hub register data byte
    (uint8_t)0x7FU, (uint8_t)0x01U, // Hub register address, Hub register data byte
    (uint8_t)0x7FU, (uint8_t)0x00U, // Hub register address, Hub register data byte

    SSBAL_CFG_END
};
#else
SsbConfigurations_c::SsbConfigurationBytes[SSB_NUMBER_OF_CONFIGURATION_BYTES] =
{
    SSBAL_CFG_HUB_0,
    (uint8_t)0x00U, (uint8_t)0x09U, // Number of following bytes: high-byte, low-byte
    (uint8_t)0x33U, (uint8_t)0x11U, // Logical config. idx for Hub: high-byte, low-byte
    (uint8_t)0xAAU,                 // I2C-address
    (uint8_t)0x60U, (uint8_t)0x70U, // Hub register address, Hub register data byte
    (uint8_t)0x61U, (uint8_t)0x71U, // Hub register address, Hub register data byte
    (uint8_t)0x62U, (uint8_t)0x72U, // Hub register address, Hub register data byte

    SSBAL_CFG_CLIENT_0,
    (uint8_t)0x00U, (uint8_t)0x04U, // Number of following bytes: high-byte, low-byte
    (uint8_t)0x80U, (uint8_t)0x90U, // Hub register address, Hub register data byte
    (uint8_t)0x81U, (uint8_t)0x91U, // Hub register address, Hub register data byte

    SSBAL_CFG_DEVICE_0,
    (uint8_t)0x00U, (uint8_t)0x0FU, // Number of following bytes: high-byte, low-byte
    (uint8_t)0x00U, (uint8_t)0x02U, // Number of write bytes: high-byte, low-byte
    (uint8_t)0x00U, (uint8_t)0x03U, // Number of read  bytes: high-byte, low-byte
    (uint8_t)0x01U, (uint8_t)0x02U, // Write bytes
    (uint8_t)0x00U, (uint8_t)0x05U, // Number of write bytes: high-byte, low-byte
    (uint8_t)0x00U, (uint8_t)0x00U, // Number of read  bytes: high-byte, low-byte
    (uint8_t)0x11U, (uint8_t)0x12U, (uint8_t)0x13U, (uint8_t)0x14U, (uint8_t)0x15U, // Write bytes

    SSBAL_CFG_CLIENT_1,
    (uint8_t)0x00U, (uint8_t)0x02U, // Number of following bytes: high-byte, low-byte
    (uint8_t)0xA0U, (uint8_t)0xB0U, // Hub register address, Hub register data byte

    SSBAL_CFG_HUB_2,
    (uint8_t)0x00U, (uint8_t)0x05U, // Number of following bytes: high-byte, low-byte
    (uint8_t)0x33U, (uint8_t)0x12U, // Logical config. idx for Hub: high-byte, low-byte
    (uint8_t)0xAAU,                 // I2C-address
    (uint8_t)0x60U, (uint8_t)0x70U, // Hub register address, Hub register data byte

    SSBAL_CFG_CLIENT_1,
    (uint8_t)0x00U, (uint8_t)0x04U, // Number of following bytes: high-byte, low-byte
    (uint8_t)0x80U, (uint8_t)0x90U, // Hub register address, Hub register data byte
    (uint8_t)0x81U, (uint8_t)0x91U, // Hub register address, Hub register data byte

    SSBAL_CFG_END
};
#endif

/*lint -e40 @@ */

