/*******************************************************************************
 *  Copyright (c) 2023 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          MCU Framework
 *  COMP_ABBREV      TIM
 ******************************************************************************/


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
    \brief GD32F30x specific configuration template for timer library.
*
*   \details    The basic timer module (Timer5) reference 
                is a 16-bit counter that can be used as an unsigned counter.
*/


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include "Libtypes.h"
#include "timerlib.h"
#include "processor.h"
#ifdef MCAL_MPCM_INCLUDED
#include "mcal/mpcm.h"
#endif
/******************************************************************************/
/* DEFINITIONS                                                                */
/******************************************************************************/
#define APB1_PSC_4   2U     // number 4 represent CK_AHB/2
#define APB1_PSC_5   4U     // number 5 represent CK_AHB/4
#define APB1_PSC_6   8U     // number 6 represent CK_AHB/8
#define APB1_PSC_7   16U    // number 7 represent CK_AHB/16



static uint16_t rcu_apb1_psc_get(void);

/* How to set CORE_CLOCK and PCLOCK1, PCLOCK2 -> in the build process see common_config.mak */

/* Switch off: info 845: the right/left operand to << always evaluates to 0 */
/* Switch off: info 835: zero given as right argument to + in a constant expression */
/* Switch off: info 923: explicit cast from 'uint32_t' (aka 'unsigned int') to 'volatile uint32_t *' (aka 'volatile unsigned int *') */
/*lint -save -e845 -e835 -e923*/

// Used TIMER 5


static uint16_t rcu_apb1_psc_get(void)
{
    return (uint16_t)(RCU_CFG0 & RCU_CFG0_APB1PSC);
}


BOOL _TIM_bGetInitConfig(void)
{
    uint16_t apb1_prescaler;
    uint16_t timer_psc = 1;
    timer_parameter_struct timerConfig;
#ifdef MCAL_MPCM_INCLUDED
    uint16_t core_clock = (uint16_t)(MPCM_getClockFreq(MPCM_CLOCK_CORECLK)/1000000U);
#else
    uint16_t core_clock = CORE_CLOCK;
#endif

    apb1_prescaler = rcu_apb1_psc_get();   // read value APB1PSC register

    if (apb1_prescaler == RCU_APB1_CKAHB_DIV1) // TIMER1,2,3,4,5,6,11,12,13 if(APB1prescale = 1) x 1
    {
        timer_psc = core_clock;
    }
    else if(apb1_prescaler == RCU_APB1_CKAHB_DIV2) // else x 2
    {
        timer_psc = ((core_clock /  APB1_PSC_4) * 2u);
    }
    else if(apb1_prescaler == RCU_APB1_CKAHB_DIV4)
    {
        timer_psc = ((core_clock /  APB1_PSC_5) * 2u);
    }
    else if(apb1_prescaler == RCU_APB1_CKAHB_DIV8)
    {
        timer_psc = ((core_clock /  APB1_PSC_6) * 2u);
    }
    else if(apb1_prescaler == RCU_APB1_CKAHB_DIV16)
    {
        timer_psc = ((core_clock /  APB1_PSC_7) * 2u);
    }
    else
    {
        // out of range
    }

    /* TIMER5 configuration */
    timerConfig.prescaler         = timer_psc - 1U;  //The TIMER_CK clock is divided by (PSC+1) to generate the counter clock.
    timerConfig.alignedmode       = TIMER_COUNTER_EDGE;
    timerConfig.counterdirection  = TIMER_COUNTER_UP;
    timerConfig.clockdivision     = TIMER_CKDIV_DIV1;
    timerConfig.period            = 65535U;
    timerConfig.repetitioncounter = 0U;

    /* Enable clock for timer unit. */
    RCU_APB1EN |= (uint32_t)RCU_APB1EN_TIMER5EN;
    /* Initialization of TIMER 5 */
    timer_init(TIMER5, &timerConfig);
    /* Enable TIMER 5*/
    TIMER_CTL0(TIMER5) |= (uint32_t)TIMER_CTL0_CEN;

    return TRUE;
}

// Axivion Next Line MisraC2012-8.3: Return type at definition differs from type at declaration
uint16_t TIM_uiGetCircleMicroSeconds(void)
{
    return (uint16_t) (TIMER_CNT(TIMER5));
}

#ifdef MCAL_MPCM_INCLUDED
// Axivion Next Line MisraC2012-8.13: Parameter can be declared as pointer/reference to const. [obj]
void TIM_MpcmEventCallback(void *obj, uint32_t flags, const struct MCAL_EventResponse *eventResponse)
{
    (void) obj;
    (void) flags;

    if(MCAL_isEvent(eventResponse, MCAL_EVENT_CLOCK_CHANGE_PRE))
    {
        /* disable timer until pmode is changed */
        TIMER_CTL0(TIMER5) &= ~(uint32_t)TIMER_CTL0_CEN;
    }
    else if(MCAL_isEvent(eventResponse, MCAL_EVENT_CLOCK_CHANGE_POST))
    {
        /* configure and re-enable timer */
        (void)_TIM_bGetInitConfig();
    }
    else
    {
        /* Nothing to do */
    }
}
#endif

