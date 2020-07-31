/*******************************************************************************
 *  Copyright (c) 2017 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          PP PED Framework
 *  COMP_ABBREV      TM
 ******************************************************************************/


/**************************************************************************************************/
/* DOCUMENTATION                                                                                  */
/**************************************************************************************************/
/** \file
    Published definitions and declarations for project specific part of TestSupport Module.\n
*/


#ifndef TESTSUPPORTXS_H
#define TESTSUPPORTXS_H

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include <stdint.h>


/**************************************************************************************************/
/* DEFINITIONS                                                                                    */
/**************************************************************************************************/
//lint -esym(9058,T_TM_TpTypes)   "tag.. unused outside of typedefs" reason: used in case of unit test


// project specific settings:
// - set TM_MAX_ARRAY_SIZE_INTX to the maximal number of array elements THAT ARE PASSED AS AN ARGUMENT
// - set TM_MAX_BYTES_FOR_STRUCT_ARRAY to the maximal size of a struct or array of structs in bytes THAT IS PASSED AS AN ARGUMENT
#define TM_MAX_ARRAY_SIZE_INT8          40U
#define TM_MAX_ARRAY_SIZE_INT16         20U
#define TM_MAX_ARRAY_SIZE_INT32         10U
#define TM_MAX_ARRAY_SIZE_INT64         5U

#define TM_MAX_BYTES_FOR_STRUCT_ARRAY   200U
#define TM_MAX_INDEX_POINTER_ARRAY      10U

typedef struct TM_Wrapper_prm_tag {

    union TM_Wrapper_prm_res_tag {

        int8_t          TM_Wrapper_int8[TM_MAX_ARRAY_SIZE_INT8];
        int16_t         TM_Wrapper_int16[TM_MAX_ARRAY_SIZE_INT16];
        int32_t         TM_Wrapper_int32[TM_MAX_ARRAY_SIZE_INT32];
        #if !defined(INT64_NOT_SUPPORTED)
        int64_t         TM_Wrapper_int64[TM_MAX_ARRAY_SIZE_INT64];
        #endif
        uint8_t         TM_Wrapper_uint8[TM_MAX_ARRAY_SIZE_INT8];
        uint16_t        TM_Wrapper_uint16[TM_MAX_ARRAY_SIZE_INT16];
        uint32_t        TM_Wrapper_uint32[TM_MAX_ARRAY_SIZE_INT32];
        #if !defined(INT64_NOT_SUPPORTED)
        uint64_t        TM_Wrapper_uint64[TM_MAX_ARRAY_SIZE_INT64];
        #endif
        //

        /*float         TSCD_Wrapper_float;
        double          TSCD_Wrapper_double;
        long double     TSCD_Wrapper_dfloat;*/

        uint8_t         TM_Wrapper_bytearray[TM_MAX_BYTES_FOR_STRUCT_ARRAY];

#if defined(RL78G14)
        void __huge *   TM_Wrapper_parray[TM_MAX_INDEX_POINTER_ARRAY];
        void __huge **  TM_Wrapper_ptparray[TM_MAX_INDEX_POINTER_ARRAY];
#else
        void*   TM_Wrapper_parray[TM_MAX_INDEX_POINTER_ARRAY];
        void**  TM_Wrapper_ptparray[TM_MAX_INDEX_POINTER_ARRAY];
#endif

    }TM_Wrapper_res_content;

    #if defined(RL78G14)
    void __huge *   TM_Wrapper_p;
    void __huge **  TM_Wrapper_pp;
    void __huge *** TM_Wrapper_ppp;
    #else
    void*       TM_Wrapper_p;
    void**      TM_Wrapper_pp;
    void***     TM_Wrapper_ppp;
    #endif

}TM_Wrapper_prm;

extern TM_Wrapper_prm TM_Wrapper_ut_RET, TM_Wrapper_ut_prm0, TM_Wrapper_ut_prm1, TM_Wrapper_ut_prm2, TM_Wrapper_ut_prm3, TM_Wrapper_ut_prm4, TM_Wrapper_ut_prm5, TM_Wrapper_ut_prm6, TM_Wrapper_ut_prm7;

typedef enum T_TM_TpTypes {
    TM_STACK,
    TM_WATCH,
    TM_FLAG,
    TM_INC,
    TM_COUNT_OF_TP_HANDLERS
} T_TM_TpTypes;

/**************************************************************************************************/
/* DATA DECLARATION                                                                               */
/**************************************************************************************************/


/**************************************************************************************************/
/* FUNCTION DECLARATION                                                                           */
/**************************************************************************************************/
extern void TM_resetAllWrapperStructs(void);





#ifdef __cplusplus
}
#endif

#endif
