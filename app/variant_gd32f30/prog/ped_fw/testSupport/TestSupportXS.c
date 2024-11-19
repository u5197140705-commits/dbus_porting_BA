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


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *  \brief      Configuration for test support library.
 */



/**************************************************************************************************/
/* INCLUDES                                                                                       */
/**************************************************************************************************/
#include "bsh_stdinc.h"
#include <stdint.h>
#include "TestSupport.h"
#include "TestSupportXS.h"

/**************************************************************************************************/
/* LOCAL DEFINITIONS                                                                              */
/**************************************************************************************************/
#ifndef TM_TP_ARRAY_SIZE    // may be defined in build process
#define TM_TP_ARRAY_SIZE       1u  ///< Size of test point array (must be 1 at least)
#endif

#ifndef TM_TP_STACK_SIZE    // may be defined in build process
#define TM_TP_STACK_SIZE       20u ///< Size of stack array (must be 1 at least)
#endif

#ifndef TP_WATCH_ARRAY_SIZE // may be defined in build process
#define TP_WATCH_ARRAY_SIZE    10u ///< Size of watch point array (must be 1 at least)
#endif



/**************************************************************************************************/
/* LOCAL VARIABLES                                                                                */
/**************************************************************************************************/


/**************************************************************************************************/
/* LOCAL FUNCTIONS DEFINITION                                                                     */
/**************************************************************************************************/


/**************************************************************************************************/
/* GLOBAL DATA DEFINITION                                                                         */
/**************************************************************************************************/
T_TM_TestPoints   TM_atTestPoints[TM_TP_ARRAY_SIZE];   // test point array
uint16_t  TM_atTestPointStack[TM_TP_STACK_SIZE];         // stack array
uint16_t  TM_atWatchPoints[TP_WATCH_ARRAY_SIZE];         // watch point array
uint16_t  TM_atWatchPointDef[TP_WATCH_ARRAY_SIZE];       // watch point definition array

const uint8_t TM_ucCountOfTpHandlers = (uint8_t)TM_COUNT_OF_TP_HANDLERS; // number of test point handlers
const uint16_t TM_uiTpArraySize      = TM_TP_ARRAY_SIZE;
const uint16_t TM_uiTpStackSize      = TM_TP_STACK_SIZE;
const uint16_t TM_uiTpWatchArraySize = TP_WATCH_ARRAY_SIZE;

/** Array of test point handlers */
const TTestPointerHandler TM_atTestPointHandler[] ={
  TM_vSetStackPoint,
  TM_vSetWatchPoint,
  TM_vSetFagPoint,
  TM_vIncTestPoint
};

const T_TM_Anchor TM_TEST_SupportFuncPtr[] =
{
    //lint -e(929)
    (T_TM_Anchor) TM_resetAllWrapperStructs
};

//lint -e{9025, 9026,708} // 3 pointer indirection levels needed to provide 2 pointer indirection levels to the user (Misra 18.5)
TM_Wrapper_prm TM_Wrapper_ut_RET = {

        .TM_Wrapper_res_content = {{0}},

        .TM_Wrapper_p = &(TM_Wrapper_ut_RET.TM_Wrapper_res_content),
        .TM_Wrapper_pp = &(TM_Wrapper_ut_RET.TM_Wrapper_p),
        .TM_Wrapper_ppp = &(TM_Wrapper_ut_RET.TM_Wrapper_pp)

};


//lint -e(9023,9024, 9026,708) // function-like macro and (multiple) token pasting needed for ease of maintenance (Misra 18.5, Misra 20.10);  3 pointer indirection levels needed to provide 2 pointer indirection levels to the user (requested by Misra 18.5), union initialization
#define TM_Wrapper_Init(no)      TM_Wrapper_prm TM_Wrapper_ut_prm##no = {                               \
                                                                                                        \
                                    .TM_Wrapper_res_content = {{0}},                                    \
                                                                                                        \
                                    .TM_Wrapper_p = &(TM_Wrapper_ut_prm##no.TM_Wrapper_res_content),    \
                                    .TM_Wrapper_pp = &(TM_Wrapper_ut_prm##no.TM_Wrapper_p),             \
                                    .TM_Wrapper_ppp = &(TM_Wrapper_ut_prm##no.TM_Wrapper_pp)            \
                                                                                                        \
                                };

// init all pointers
//lint -e{9025,708} // 3 pointer indirection levels needed to provide 2 pointer indirection levels for the user (requested by Misra 18.5)
TM_Wrapper_Init(0)
//lint -e{9025,708} // 3 pointer indirection levels needed to provide 2 pointer indirection levels for the user (requested by Misra 18.5)
TM_Wrapper_Init(1)
//lint -e{9025,708} // 3 pointer indirection levels needed to provide 2 pointer indirection levels for the user (requested by Misra 18.5)
TM_Wrapper_Init(2)
//lint -e{9025,708} // 3 pointer indirection levels needed to provide 2 pointer indirection levels for the user (requested by Misra 18.5)
TM_Wrapper_Init(3)
//lint -e{9025,708} // 3 pointer indirection levels needed to provide 2 pointer indirection levels for the user (requested by Misra 18.5)
TM_Wrapper_Init(4)
//lint -e{9025,708} // 3 pointer indirection levels needed to provide 2 pointer indirection levels for the user (requested by Misra 18.5)
TM_Wrapper_Init(5)
//lint -e{9025,708} // 3 pointer indirection levels needed to provide 2 pointer indirection levels for the user (requested by Misra 18.5)
TM_Wrapper_Init(6)
//lint -e{9025,708} // 3 pointer indirection levels needed to provide 2 pointer indirection levels for the user (requested by Misra 18.5)
TM_Wrapper_Init(7)

/**************************************************************************************************/
/* GLOBAL FUNCTIONS DEFINITION                                                                    */
/**************************************************************************************************/
// Reset the passed Wrapper-Struct
void TM_resetWrapperStruct(TM_Wrapper_prm *resetter)
{


    uint8_t i=0;

    for(; i < TM_MAX_BYTES_FOR_STRUCT_ARRAY; i++ )
    {
            resetter->TM_Wrapper_res_content.TM_Wrapper_bytearray[i] = 0;
    }

    resetter->TM_Wrapper_p          = &(resetter->TM_Wrapper_res_content);
    resetter->TM_Wrapper_pp         = &(resetter->TM_Wrapper_p);
    //lint -e{9025} // 3 pointer indirection levels needed to provide 2 pointer indirection levels for the user (requested by Misra 18.5)
    resetter->TM_Wrapper_ppp        = &(resetter->TM_Wrapper_pp);

}

// Reset all Wrapper-Structs
void TM_resetAllWrapperStructs(void)
{

    TM_resetWrapperStruct(&TM_Wrapper_ut_RET);
    TM_resetWrapperStruct(&TM_Wrapper_ut_prm0);
    TM_resetWrapperStruct(&TM_Wrapper_ut_prm1);
    TM_resetWrapperStruct(&TM_Wrapper_ut_prm2);
    TM_resetWrapperStruct(&TM_Wrapper_ut_prm3);
    TM_resetWrapperStruct(&TM_Wrapper_ut_prm4);
    TM_resetWrapperStruct(&TM_Wrapper_ut_prm5);
    TM_resetWrapperStruct(&TM_Wrapper_ut_prm6);
    TM_resetWrapperStruct(&TM_Wrapper_ut_prm7);

}

// Set a flag test point in case of module test
void TM_vSetFagPoint(T_TM_TestPoints tTestPoint)
{
   TM_atTestPoints[(T_TM_TestPoints)tTestPoint] = (T_TM_TestPoints)tTestPoint;
}

// Increment an incrment point in case of module test
void TM_vIncTestPoint(T_TM_TestPoints tTestPoint)
{
   TM_atTestPoints[(T_TM_TestPoints)tTestPoint]++;
}
