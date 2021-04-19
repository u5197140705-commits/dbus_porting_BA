/*******************************************************************************
 *  Copyright (c) 2017 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          Generic SW
 *  COMP_ABBREV      TIM
 ******************************************************************************/


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/**
    \brief Configuration template for timer segments.
*/

#if defined(GCCARM)
    #define LINKER_SEGMENT(segment)     *(.segment)
#elif defined(KEILARM) || defined(ARMCLANG)
    #define LINKER_SEGMENT(segment)     ER_##segment +0 {*(segment)}
#else
    #error unsupported compiler
#endif

#ifndef VARIANT_PROGRAMMER
LINKER_SEGMENT(START8_1)
LINKER_SEGMENT(TIMER8_10MS)
LINKER_SEGMENT(START8_2)
LINKER_SEGMENT(TIMER8_20MS)
LINKER_SEGMENT(START8_3)
LINKER_SEGMENT(TIMER8_100MS)
LINKER_SEGMENT(START8_4)
LINKER_SEGMENT(TIMER8_200MS)
LINKER_SEGMENT(START8_5)
LINKER_SEGMENT(TIMER8_1S)
LINKER_SEGMENT(START8_6)
LINKER_SEGMENT(TIMER8_2S)
LINKER_SEGMENT(START8_7)
LINKER_SEGMENT(TIMER8_10S)
LINKER_SEGMENT(START8_8)
LINKER_SEGMENT(TIMER8_20S)
LINKER_SEGMENT(START8_9)
LINKER_SEGMENT(TIMER8_1M)
LINKER_SEGMENT(START8_10)
LINKER_SEGMENT(TIMER8_2M)
LINKER_SEGMENT(END8)
LINKER_SEGMENT(START16_1)
LINKER_SEGMENT(TIMER16_10MS)
LINKER_SEGMENT(START16_2)
LINKER_SEGMENT(TIMER16_20MS)
LINKER_SEGMENT(START16_3)
LINKER_SEGMENT(TIMER16_100MS)
LINKER_SEGMENT(START16_4)
LINKER_SEGMENT(TIMER16_200MS)
LINKER_SEGMENT(START16_5)
LINKER_SEGMENT(TIMER16_1S)
LINKER_SEGMENT(START16_6)
LINKER_SEGMENT(TIMER16_2S)
LINKER_SEGMENT(START16_7)
LINKER_SEGMENT(TIMER16_10S)
LINKER_SEGMENT(START16_8)
LINKER_SEGMENT(TIMER16_20S)
LINKER_SEGMENT(START16_9)
LINKER_SEGMENT(TIMER16_1M)
LINKER_SEGMENT(START16_10)
LINKER_SEGMENT(TIMER16_2M)
LINKER_SEGMENT(START16_11)
LINKER_SEGMENT(TIMER16_10M)
LINKER_SEGMENT(START16_12)
LINKER_SEGMENT(TIMER16_20M)
LINKER_SEGMENT(END16)
#else
LINKER_SEGMENT(START8_1)
LINKER_SEGMENT(TIMER8_10MS)
LINKER_SEGMENT(START8_2)
LINKER_SEGMENT(TIMER8_2S)
LINKER_SEGMENT(END8)
LINKER_SEGMENT(START16_1)
LINKER_SEGMENT(TIMER16_10MS)
LINKER_SEGMENT(END16)
#endif