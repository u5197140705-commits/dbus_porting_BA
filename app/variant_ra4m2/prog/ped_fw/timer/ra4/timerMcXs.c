/*******************************************************************************
 *  Copyright (c) 2022 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          Generic SW / Renesas RA4
 *  COMP_ABBREV      TIM
 ******************************************************************************/


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
    \brief RA4 specific configuration template for timer library.

*/

/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include "libtypes.h"
#include "processor.h"
#include "timerlib.h"
#include "fsp/r_gpt.h"
#include "fsp/r_agt.h"
#include "hal_data.h"
#include "std_lib/std_symbol.h"
#if defined(MCAL_MPCM_INCLUDED)
#include "mcal/mpcm.h"
#endif

/*lint -save -e923 */
static void TIM_createTimersCascade(void)
{
    R_MSTP->MSTPCRC_b.MSTPC14 = 0;                             // cancel the ELC module-stop state
    R_ELC->ELSR[0].HA         = (uint16_t)ELC_EVENT_AGT0_INT;  // AGT0 underflow interrupt linked as input to GPT7
    R_ELC->ELCR_b.ELCON       = 1;                             // enable ELC function
}

BOOL _TIM_bGetInitConfig(void)
{
    BOOL retVal = FALSE;

    if(R_GPT_Open(&g_timerGPT7_ctrl, &g_timerGPT7_cfg) == FSP_SUCCESS)
    {
        if(R_GPT_Enable (&g_timerGPT7_ctrl) == FSP_SUCCESS)
        {
            (void)R_GPT_Start (&g_timerGPT7_ctrl);
            TIM_createTimersCascade();
#if defined(MCAL_MPCM_INCLUDED)
            timerAGT0_cfg.period_counts = MPCM_getClockFreq(MPCM_CLOCK_PCLKB) / 1000000UL;
#endif
            if(R_AGT_Open (&timerAGT0_ctrl, &timerAGT0_cfg) == FSP_SUCCESS)
            {
                if(R_AGT_Start (&timerAGT0_ctrl) == FSP_SUCCESS)
                {
                    retVal = TRUE;
                }
            }
        }
    }
    return retVal;
}


uint16 TIM_uiGetCircleMicroSeconds(void)
{
    /* Change number in R_GPTx according to used timer unit in g_timerGPTx_cfg in hal_data.c */
	const R_GPT0_Type * p_reg = R_GPT7; // Base register for the timer's unit
    
    /* Return counter value */
    return (uint16)p_reg->GTCNT;
}
/*lint -restore */
