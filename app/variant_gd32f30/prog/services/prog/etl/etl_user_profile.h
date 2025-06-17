/*******************************************************************************
 *   Copyright (c) 2023 BSH Hausgeraete GmbH,
 *   Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *   All rights reserved. This program and the accompanying materials
 *   are protected by international copyright laws.
 *   Please contact copyright holder for licensing information.
 *
 *********************************************************************************/

#ifndef SRV_ETL_USER_PROFILE_H
#define SRV_ETL_USER_PROFILE_H

#ifdef KEILARM
// Automatic endianness detection fails on keilArm.
// ETL_ENDIAN_NATIVE must be defined either as 0 for 'little endian' or 1 for 'big endian'
#define ETL_ENDIAN_NATIVE 0
#endif

// Put your definitions here (see https://www.etlcpp.com/macros.html)

#endif // SRV_ETL_USER_PROFILE_H
