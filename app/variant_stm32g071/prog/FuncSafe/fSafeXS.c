/*******************************************************************************
 *  Copyright (c) 2017 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          Functional Safety
 ******************************************************************************/

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file fsafeXS.c
 *
 *  \ingroup  FunctionalSafety
 *
 *  \brief    Application specific implementation and configuration
 *            for library functional safety.
 *
 */

/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include "LibTypes.h"
#include "FuncSafe/fSafe.h"
#include "FuncSafe/fSafeXS.h"
#include "FuncSafe/fspriv.h"
#include "FuncSafe/fsramcon.h"
#include "SegmentDef.h"
#include "stdcrc/StdCrc.h"
#include "std_lib/std_symbol.h"
#include "basic/basic.h"
#if !defined(TH_FSCOM) && !defined(VARIANT_BOOTMANAGER)
#include "watchdogtimer/watchdogtimer.h"
#endif

/******************************************************************************/
/* PRIVATE DEFINITIONS                                                        */
/******************************************************************************/
/* Switch off:
   Info 714: external symbol 'xx' was defined but not referenced
   Info 765: external symbol 'xx' could be made static [MISRA 2012 Rule 8.7, advisory]
 */
/*lint -save -e714 -e765 -e923*/

#ifdef __NOT_CC__
    #define STACK_BASE  ((uint32_t)0u)
    #define STACK_LIMIT ((uint32_t)0u)
#elif defined(__GNUC__) && !defined(__ARMCC_VERSION)
    /* Stack definition for GCC compiler*/
    extern uint32_t __StackTop;

    #define STACK_LIMIT (&__StackTop)
    #define STACK_BASE ((uint32_t)(STACK_LIMIT) - (uint32_t)(STACKSIZE) + (uint32_t)(STACK_MAGIC_SIZE))
#elif __CC_ARM || defined(__ARMCC_VERSION)
    /* Stack definition for Keil compiler*/
    extern uint32_t Image$$ARM_LIB_STACK$$ZI$$Base;
    extern uint32_t Image$$ARM_LIB_STACK$$ZI$$Limit;

    #define STACK_BASE (&Image$$ARM_LIB_STACK$$ZI$$Base)
    #define STACK_LIMIT (&Image$$ARM_LIB_STACK$$ZI$$Limit)
#endif // #elif __CC_ARM

#define STACKMAGIC (0xFAFEuL)

#ifdef FSF_CRC_32_BIT
    #ifdef __Cypress__            // for Cypress use SW CRC
        #define FSF_SW_CRC_32_BIT // SW CRC calculation is used
    #endif
#endif

/** ROM Sections
 *  No. of words that are processed per one cycle 
 */
#define ROM_CRC_STEP (32)


/******************************************************************************/
/* PRIVATE TYPE DEFINITIONS                                                   */
/******************************************************************************/


/******************************************************************************/
/* PRIVATE DATA DEFINITIONS                                                   */
/******************************************************************************/
/* Note 9003: could define global variable 'FSF_ucTaskState' within function
 * _FSF_bReadExtWatchdog [MISRA 2012 Rule 8.9, advisory], no not possible!
 */

/*  FSCOM reserved work area */

/** The FSCOM system state
 *  Computed next watchdog trigger
 *  Channel states. Structured and size: [FSF_FL_NR_OF_FLAGS][FSCOM_FLAG_FIELD]
 *  Pieces of base cycles elapsed. Structured and size: [FSF_NR_OF_CHANNELS]
 *  The check cycle timer count units of 100 milliseconds
 *  Last error and channel
 */
/*lint -esym(9003,FSF_ucTaskState) */
uint8_t  FSF_ucTaskState = 0u;
uint16_t FSF_ui16LastWDTrigger = 0u;
uint8_t  FSF_ucCH_states[(uint8_t)FSF_NR_OF_FLAGS][FSCOM_FLAG_FIELD];
T_FSCOM_CheckCycle FSF_sCH_CheckCycleTime[(uint8_t)FSF_NR_OF_CHANNELS];
#ifdef FSF_CYCLE_TIM_SIZE_32BIT
    uint16_t FSF_ui16CycLastTime   = 0u;
    uint32_t FSF_ulCheckCycleTimer = 0u;
#else
    uint8_t  FSF_ucCheckCycleTimer = 0u;
#endif
uint16_t FSF_ui16Current = 0u;
uint16_t FSF_ui16delta_t = 0u;

#ifdef OPERATIONAL_RAM_CHECK
SDEF_SetSegmentRW(RAMCONT_EXCLUDED_DATA) // put it into segment RAMCONT_EXCLUDED_DATA
    SYMBOL_USED uint8_t FSF_arMarchDataBuffer[MARCHBUFLENGTH]; //!< RAM Data Exchange Buffer
SDEF_SetSegmentRW_Default()
#endif

#ifndef TH_FSCOM
    #ifdef FUNCTIONAL_SAFETY_ROM_CHECK_ACTIVE
        /* ROM settings */
        FSTH_CONST T_ROM_Segment FSF_atRomSegments_0[] = 
        {
            /* BootLoader ( BootManager or romLoader ) */
            {(uint8_t*) FS_BL_START_ADR, (uint8_t*) FS_BL_END_ADR},
            /* APP */
            {(uint8_t*) FS_APP_START_ADR, (uint8_t*) FS_APP_END_ADR},
        };  //!< Main memory (internal ROM)

        /*lint -save -e929 cast from pointer to pointer */
        /*lint -save -e552 external variable not accessed, though is accessed indirectly by patRom in fsRom.c */
        FSTH_CONST T_ROM_Module FSF_atRomModules[] = 
        {
            //#warning "Adjust ROM bounds according to your MCU derivate in fSafe.mak"
            {   &(FSF_atRomSegments_0[0]),
                1u,   // only 1 segment
                (uint8_t)ROM_CRC_STEP, (T_ROM_Read)_FSF_pucReadROMData,
                (T_ROM_Ready)_FSF_bIsROMReady,  //!< still empty
                (uint32_t*)FS_BL_CRC_ADR
            },
            {   &(FSF_atRomSegments_0[1]),
                1u,   // only 1 segment
                (uint8_t)ROM_CRC_STEP, (T_ROM_Read)_FSF_pucReadROMData,
                (T_ROM_Ready)_FSF_bIsROMReady,  //!< still empty
                (uint32_t*)FS_APP_CRC_ADR
            },
        };
        /*lint -restore -e929 -e552 */

        FSTH_CONST uint8_t FSF_ucRomModuleCount =
            (uint8_t)(sizeof(FSF_atRomModules) / sizeof(T_ROM_Module));
    #else
        FSTH_CONST T_ROM_Module FSF_atRomModules[] = 
        {
            (T_ROM_Segment *)NULL, (uint8_t)0,          (uint8_t)0,
            (T_ROM_Read)NULL,      (T_ROM_Ready)NULL, (uint16_t*)0
        };
        FSTH_CONST uint8_t FSF_ucRomModuleCount = 0u;
    #endif
#endif

/** The FSCOM Default Channel configuration
 *  This channel is provided in order to save space. Some channels will have only
 *  one task function scheduled regularly. Hence, such channels may use the default
 *  channels so that no space is wasted.
 */
FSTH_CONST T_FSCOM_Channel FSF_sDefaultChannel = // No check cycle supervision
{
    { 0u, 0u },                    // No cycle multiplier
    false,                         // Is scheduled regularly?
    _FSF_bDefaultPreconditionTRUE, // No pre-condition
    _FSF_vDefaultOneTimeCheck      // No one-time check
};

/** The Default RAM Channel checker. This allows to test the functional safety
 *  generic moodule independently of a specific type of controller
 */

/** The RAM Channel configuration */
FSTH_CONST T_FSCOM_Channel FSF_sRAMChannel = 
{
#ifdef OPERATIONAL_RAM_CHECK       // Yes, check cycle supervision
    { RAM_CYCLE_CHECK_TIME, 0u},   // The cycle multiplier
    true,                          // Is scheduled regularly?
#else
    { 0u, 0u },                    // No cycle multiplier
    false,                         // Is scheduled regularly?
#endif
    _FSF_bDefaultPreconditionTRUE, // No pre-condition

#if defined(REMOTE_FIRMWARE_UPDATE) || defined(TH_FSCOM)
    _FSF_vDefaultOneTimeCheck,     // one-time check moved to BootManager
#else
    FSF_vRam_OneTimeCheck          // The RAM one-time check
#endif
};

/** The ROM Channel configuration */
FSTH_CONST T_FSCOM_Channel FSF_sROMChannel = // Has check cycle supervision
{
    { ROM_CYCLE_CHECK_TIME, 0u },  // One-time cycle multiplier
    true,                          // Is scheduled regularly?
    _FSF_bDefaultPreconditionTRUE, // No pre-condition
#if defined(TH_FSCOM)
    _FSF_vDefaultOneTimeCheck,     // one-time check moved to BootManager
#else
    FSF_vStartUpRomCrcCheck        // No one-time check
#endif
};

/** The Interrupt Channel configuration */
static FSTH_CONST T_FSCOM_Channel FSF_sISRChannel = // No check cycle supervision
{
    { 0u, 0u },                    // No cycle multiplier
    true,                          // Is scheduled regularly?
    _FSF_bDefaultPreconditionTRUE, // No pre-condition
    _FSF_vDefaultOneTimeCheck      // The Register one-time check
};

/** The Interrupt Latency Channel configuration */
static FSTH_CONST T_FSCOM_Channel FSF_sISRLatencyChannel = // No check cycle supervision
{
    { 0u, 0u },                    // No cycle multiplier
    true,                          // Is scheduled regularly?
    _FSF_bDefaultPreconditionTRUE, // No pre-condition
    _FSF_vDefaultOneTimeCheck      // The Register one-time check
};

/** The Interrupt Minimum Frequency Channel configuration */
static FSTH_CONST T_FSCOM_Channel FSF_sISRMinFreqChannel = // No check cycle supervision
{
    { 0u, 0u },                    // No cycle multiplier
    true,                          // Is scheduled regularly?
    _FSF_bDefaultPreconditionTRUE, // No pre-condition
    _FSF_vDefaultOneTimeCheck      // The Register one-time check
};

/** The Interrupt Maximum Frequency Channel configuration */
static FSTH_CONST T_FSCOM_Channel FSF_sISRMaxFreqChannel = // No check cycle supervision
{
    { 0u, 0u },                    // No cycle multiplier
    true,                          // Is scheduled regularly?
    _FSF_bDefaultPreconditionTRUE, // No pre-condition
    _FSF_vDefaultOneTimeCheck      // The Register one-time check
};

/** The Default Register Channel checker. */
static FSTH_CONST T_FSCOM_Channel FSF_sRegisterChannel = // No check cycle supervision
{
    { 0u, 0u },                    // No cycle multiplier
    true,                          // Is scheduled regularly?
    _FSF_bDefaultPreconditionTRUE, // No pre-condition
    FSF_vRegister_OneTimeCheck     // The Register one-time check
};

/** The Stack Channel configuration */
static FSTH_CONST T_FSCOM_Channel FSF_sStackChannel = // No check cycle supervision
{
    { 0u, 0u },                    // No cycle multiplier
    true,                          // Is scheduled regularly?
#ifndef VARIANT_BOOTMANAGER
    FSF_bStackPrecondition,        // No pre-condition
#else
    _FSF_bDefaultPreconditionTRUE, // No pre-condition
#endif
    _FSF_vDefaultOneTimeCheck      // The Register one-time check
};

/** The RAM Consistency Channel configuration */
static FSTH_CONST T_FSCOM_Channel FSF_sRAMConsistencyChannel = // No check cycle supervision
{
    { 0u, 0u },                    // No cycle multiplier
    false,                         // Is scheduled regularly?
    _FSF_bDefaultPreconditionTRUE, // No pre-condition
    _FSF_vDefaultOneTimeCheck      // No one-time check
};

/** The FSCOM configuration area 
 *  Note: The channel tab consists of pointers to channel located in ROM. The
 *  channel tab itself must be located in the ROM area too!
 */
FSTH_CONST T_FSCOM_Channel *FSTH_CONST FSF_psChannelTab[(uint8_t)FSF_NR_OF_CHANNELS] = 
{
#if defined(FUNCTIONAL_SAFETY_REGISTER_CHECK_ACTIVE) || defined(TH_FSCOM)
    &FSF_sRegisterChannel,
#else
    &FSF_sDefaultChannel,
#endif

#if defined(FUNCTIONAL_SAFETY_RAM_CHECK_ACTIVE) || defined(TH_FSCOM)
    &FSF_sRAMChannel,
#else
    &FSF_sDefaultChannel,
#endif

#if defined(FUNCTIONAL_SAFETY_ROM_CHECK_ACTIVE) || defined(TH_FSCOM)
    &FSF_sROMChannel,
#else
    &FSF_sDefaultChannel,
#endif

#if defined(FUNCTIONAL_SAFETY_STACK_CHECK_ACTIVE) || defined(TH_FSCOM)
    &FSF_sStackChannel,
#else
    &FSF_sDefaultChannel,
#endif

#if defined(FUNCTIONAL_SAFETY_ISR_CHECK_ACTIVE) || defined(TH_FSCOM)
    &FSF_sISRChannel,           &FSF_sISRLatencyChannel,
    &FSF_sISRMinFreqChannel,    &FSF_sISRMaxFreqChannel,
#else
    &FSF_sDefaultChannel, &FSF_sDefaultChannel,
    &FSF_sDefaultChannel, &FSF_sDefaultChannel,
#endif

#if defined(FUNCTIONAL_SAFETY_RAMCON_CHECK_ACTIVE) || defined(TH_FSCOM)
    &FSF_sRAMConsistencyChannel
#else
    &FSF_sDefaultChannel
#endif
};

FSTH_CONST uint8_t FSF_ucNrOfChannels = (uint8_t)FSF_NR_OF_CHANNELS;
#ifdef FS_PEC
    FSTH_CONST uint16_t FSF_ui16WDC1_2_3 = (uint16_t)(
        (T1_WD_CYCLE * 1000u) + (T2_WD_CYCLE * 1000u) + (T3_WD_CYCLE * 1000u));
    FSTH_CONST uint16_t FSF_ui16WDC1_3 =
        (uint16_t)((T1_WD_CYCLE * 1000u) + (T3_WD_CYCLE * 1000u));
#else
    FSTH_CONST uint16_t FSF_ui16WDC1_2_3 = 0xFFFFu;
    FSTH_CONST uint16_t FSF_ui16WDC1_3 = 0u;
#endif
FSTH_CONST uint8_t FSF_ucPreSchedCycles = DEFAULT_PRESCHEDCYCLES;

/** Project:            unknown
 *  ROM Configuration:  default configuration, to be defined at project level
 */

/* ISR Channel related */
#ifndef TH_FSCOM
/* Application ISR configuration*/
uint16_t FSF_auiIsrTime[(uint8_t)FS_ISR_NUMBER_OF_LARGE_ISR];   //!< Array of integers, which
                                                   //!< contains the accumulated
                                                   //!< times needed by the
                                                   //!< different interrupts.
uint16_t FSF_auiIsrCount[(uint8_t)FS_ISR_NUMBER_OF_LARGE_ISR];  //!< Array of counters, which
                                                   //!< are incremented for each
                                                   //!< occurred interrupt
#ifdef FS_USE_SHORT_ISR_CONTROL
uint16_t FSF_auiShortIsrCount
    [(uint8_t)FS_ISR_NUMBER_OF_SHORT_ISR];  //!< Array of counters, which are incremented
                                   //!< for each occurred interrupt belonging to
                                   //!< short interrupt control.
FSTH_CONST uint8_t FSF_ucNumberOfShortInterrupts =
    (uint8_t)FS_ISR_NUMBER_OF_SHORT_ISR;  //!< Number of short interrupts considered by
                                 //!< functional safety.
#else                            // FS_USE_SHORT_ISR_CONTROL
// No short interrupt control needed.
#endif                           // FS_USE_SHORT_ISR_CONTROL
FSTH_CONST uint8_t FSF_ucNumberOfInterrupts =
    (uint8_t)FS_ISR_NUMBER_OF_LARGE_ISR;  //!< Number of large interrupts considered by
                                 //!< functional safety.
FSTH_CONST TisrControl FSF_atIsrCntrTbl[(uint8_t)FS_ISR_NUMBER_OF_LARGE_ISR] = {
    // uiLimitLatency, uiLimitMaxFrequency, uiLimitMinFrequency, uiCycleTime,
    // ucControl Standardised interrupts
    {100, 0, 0, 10000 / 256, FS_ISR_CONTROL_NONE},  // FS_ISR_SCI_TX,
    {100, 0, 0, 10000 / 256, FS_ISR_CONTROL_NONE},  // FS_ISR_SCI_RX,
    {100, 0, 0, 10000 / 256, FS_ISR_CONTROL_NONE},  // FS_ISR_SPI_TX,
    {100, 0, 0, 10000 / 256, FS_ISR_CONTROL_NONE},  // FS_ISR_SPI_RX,
    // Non-standardised interrupts
    // {100, 0, 0, 10000/256, FS_ISR_CONTROL_NONE}, // FS_ISR_SCI2_TX,
    // {100, 0, 0, 10000/256, FS_ISR_CONTROL_NONE}, // FS_ISR_SCI2_RX,
    // {100, 0, 0, 10000/256, FS_ISR_CONTROL_NONE}, // FS_ISR_SCI2_ERR,
    // {100, 0, 0, 10000/256, FS_ISR_CONTROL_NONE}, // FS_ISR_ADC,
    // {100, 0, 0, 10000/256, FS_ISR_CONTROL_NONE}, // FS_ISR_S_ISR_KBRD, 
    // {100, 0, 0, 10000/256, FS_ISR_CONTROL_NONE}, // FS_ISR_SC1_ERR,
    // {100, 0, 0, 10000/256, FS_ISR_CONTROL_NONE}  // FS_ISR_MMIIC,
};  //!< Control table for interrupt service routines.
#ifdef FS_USE_SHORT_ISR_CONTROL
FSTH_CONST TisrShortControl FSF_atIsrShortCntrTbl[(uint8_t)FS_ISR_NUMBER_OF_SHORT_ISR] = {
    // {  uiLimitMaxFrequency,    uiLimitMinFrequency, uiCycleTime, ucControl},
    // // FS_ISR_<Name of INT>
    {0, 0, 10000 / 256, FS_ISR_CONTROL_NONE}  //! FS_ISR_EXAMPLE
};
#else   // FS_USE_SHORT_ISR_CONTROL
// No short interrupt control needed.
bool bCheckShortInterrupts(const TisrShortControl *pShortIsrCtrl,
                           uint8_t ucIsrId) {
    return false;
}
// Empty function to replace the not needed function in the library.
#endif  // FS_USE_SHORT_ISR_CONTROL

#endif  // TH_FSCOM

#ifdef FSF_ISRCONFIG
    TIsrConfig atIsrLongConfig[FS_ISR_NUMBER_OF_LARGE_ISR] = {0};
    TIsrConfig atIsrShortConfig[FS_ISR_NUMBER_OF_SHORT_ISR] = {0};
#endif  // FSF_ISRCONFIG

#if defined(FSF_SW_CRC_32_BIT) && defined(FSF_CRC_32_BIT)
    // const uint32_t CRCTable[256] = 
    // {
    // };
    #error("Standard CRC library should be used")
#endif

#ifndef TH_FSCOM
    // const uint32_t FSF_CRC __attribute__((at(XCRC))); // = 0xAABBCCDD;

    /** RAM Segment Table. Its content has been defined as part of a project.inc
     *  Segments have to be aligned to 4 !!!
     */
    FSTH_CONST T_RAM_Segment FSF_atRamSegments[] =
    {
        {(uint8_t *)RAM1_PHY_START, (uint8_t *)RAM1_PHY_END },
    #ifdef RAM2_PHY_START
        /* Axivion Next Line MisraC2012-11.9: Use of literal zero (0) as null-pointer-constant, use macro NULL instead */
        {(uint8_t *)RAM2_PHY_START, (uint8_t *)RAM2_PHY_END },
    #endif
    #ifdef RAM3_PHY_START
        {(uint8_t *)RAM3_PHY_START, (uint8_t *)RAM3_PHY_END },
    #endif
    };

    #ifdef OPERATIONAL_RAM_CHECK
        /* RAM Configuration:  table of check patterns */
        FSTH_CONST T_RAM_Pattern FSF_asPatternTable[] = 
        {
            {MARCH_PATTERN_0_0, MARCH_PATTERN_1_0},
            {MARCH_PATTERN_0_1, MARCH_PATTERN_1_1}
        };

        /* The RAM Module descriptor */
        FSTH_CONST T_RAM_Module FSF_atRamModule = 
        {
            (uint8_t)5, (uint8_t)(sizeof(FSF_atRamSegments) / sizeof(T_RAM_Segment)),
            (uint8_t)(sizeof(FSF_asPatternTable) / sizeof(T_RAM_Pattern))
        };
    #endif

    /******************************************************************************/
    /* STACK CHANNEL RELATED                                                      */
    /* Provide real configuration data -> Stack definition for ARM Core           */
    /******************************************************************************/
    FSTH_CONST T_Stack _FSF_Stack[1] =
    {
        {(uint16_t *)(STACK_BASE),
        (uint16_t *)((uint32_t)STACK_LIMIT + 2uL)}
    };
    FSTH_CONST uint16_t _FSF_ui16StackMagic = (uint16_t)STACKMAGIC;
#else // TH_FSCOM
    /* todo */
    #warning " Test Harness build is active !!! "
    FSTH_CONST uint16_t _FSF_ui16StackMagic = (uint16_t)STACKMAGIC;
#endif // TH_FSCOM

/* RAM Consistency Channel related */
#ifdef TH_FSCOM
    /* Variable for Test Harness */
    static uint8_t FSF_RAMCON_Ram[] = 
    {
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2,
        2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3
    };

    const uint32_t FSF_uiRamConStart = (uint32_t)&FSF_RAMCON_Ram[0];
    const uint32_t FSF_uiRamConEnd = (uint32_t)&FSF_RAMCON_Ram[15];
    const uint16_t FSF_uiRamConMirrorOfset = (uint16_t)15;
#else
    /* Variable for normal operation */
#endif

/******************************************************************************/
/* PRIVATE FUNCTION DEFINITIONS                                               */
/******************************************************************************/


/******************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                */
/******************************************************************************/
/** Start or continue the application-specific start up process.
    In order to start or continue an applications start up phase, control is
   transferred to this routine.

    \pre        FSCOM initialised correctly. The external watchdog
   initialised.\n

    \note       to be overloaded according to a project specific needs
*/
void _FSF_vApplicationStartUp(void)
{

}

/**
   Return the actual stack pointer
<pre>
   --------------------------------------------------------------------------
   This stack channel requires local variables to be placed on the hardware
   stack. For the time being, the so called "compiled stack" is not supported
   --------------------------------------------------------------------------
</pre>
    This observer returns the actual position of the stack pointer. Performing
certain tasks in order to manipulate the stack pointer for test would result
into more or less non-portable solutions, especially when testing the same piece
of code and for different environments. Thus, for test harness purposes this
value can be read from a reserved location.

    To be noticed: due to the preconditions above, only the address of an
stack-located variable is returned. Its physical location will be some bytes
deeper inside the stack, as it is placed within the new stack frame of the
called observer. Re-alignment is not done, as this would involve some porting
problems due to different stack frame structures.

    \param    index of stack to be considered
    \return   actual location of the stack pointer
    \pre      Location and size of the stack provided correctly.
    \post     Magic numbers written correctly. Current stack pointer content
checked correctly. \remark   Probably, some CPU architectures or compilers do
not allow the default implementation below or do not provide direct access to the
stack. For those, an asm implementation may solve the problem.
*/
uint8_t *_FSF_pucGetStackPointer(void)
{
    return (uint8_t *)(__get_MSP());
}

#ifndef TH_FSCOM
/* uint8_t     _FSF_ucGetStackCount(void);
  Return 2 stacks available for this multiple stack configuration
*/
uint8_t _FSF_ucGetStackCount(void)
{
    return NUMBER_OF_STACKS;
}
#endif

/** Read data from ROM module.
    Default implementation for reading data from ROM module. \n

    \param      pSrc        ROM address where to read from\n
    \param      ui16length  length of data to read \n\n

    \return     uint8_t *     address of data that have been read, or NULL.\n\n

    \pre        Called only at operational time. Called only for ROM modules
   ready for reading \n \post       If NULL is returned, data couldn't be read
   as requested. In that case the current \n check cycle is suspended and resumed
   again when dispatched the next time.\n\n

    \note       Overloading is not designed here,as this is up to an
   application. \n
*/
uint8_t *_FSF_pucReadROMData(uint8_t *pSrc, uint16_t ui16length)
{
    (void)ui16length;
    return pSrc;
}


void FSF_vRamOpercheck(uint8_t *pucRAM, uint8_t *pucDEXBuffer, uint8_t ucPatNoInv,
                       uint8_t ucPatInv)
{
#ifdef OPERATIONAL_RAM_CHECK
    TRunTimeRamTestStatus result;

    (void)ucPatNoInv;
    (void)ucPatInv;
    /* Note: The following pointer to pointer conversions and comparing of pointers will work correctly
             The upper layer is responsible to give a correctly aligned address to parameter pucRAM and pucDEXBuffer!!
     */
    /*lint -e{946} "relational operator applied to pointers [MISRA 2012 Rule 18.2, required], [MISRA 2012 Rule 18.3, required]" */
    if ((pucRAM >= &(FSF_arMarchDataBuffer[0])) &&
        (pucRAM <= &(FSF_arMarchDataBuffer[MARCHBUFLENGTH-1])) ) // Buffer test
    {    
        result = FSF_tRunTimeRamBufferCheckMarchC((uint32_t *)pucRAM); //lint !e926 !e927 !e2445 explicit cast from pointer to pointer
    }
    else
    {   /* Regular piece of RAM Test */
        result = FSF_tRunTimeRamCheckMarchC((uint32_t *)pucRAM,        //lint !e926 !e927 !e2445 explicit cast from pointer to pointer
                                            (uint32_t *)pucDEXBuffer); //lint !e926 !e927 !e2445 explicit cast from pointer to pointer
    }
    if (result != FSF_RAM_TESK_OK)
    {
        FSF_vRam_OperationalException();
    }
#endif
}

#if !defined(TH_FSCOM)
/* number of RAM segments = SizeOf(Table of RamSegments) / SizeOf(Structure RAM_Segment) */
uint32_t FSF_u32GetNumberOfRamSegments(void)
{
    return (sizeof(FSF_atRamSegments)/sizeof(T_RAM_Segment));
}
#endif


/** Default Precondition.

    Return precondition true by default.
    Return precondition true in either case.

    \return     bool    true.

    \pre        None\n
    \note       to be overloaded according to a project specific needs\n
*/
bool _FSF_bDefaultPreconditionTRUE(void)
{
#ifdef TH_FSCOM
    TH_FSFP_PushOutPar(TP_FSF_bDefaultPreconditionTRUE);
#endif
    return true;
}

/** Default One-time check.

    Do nothing.

    \pre        None\n
    \note       to be overloaded according to a project specific needs\n
*/
void _FSF_vDefaultOneTimeCheck(void)
{
#ifdef TH_FSCOM
    TH_FSFP_PushOutPar(TP_FSF_vDefaultOneTimeCheck);
#endif
}

/** Notify an error to the application.
    Notify an application about a problem and this call happens as part of safe
   state, that is control can't be returned to FSCOM. If so, FSCOM subsequently
   calls its internal error handling routine. \param      ucSubCheckId
   FSF_ERROR_CODE that describes the problem\n \param      ucChannelId
   identifier of the affected channel or FSF_NOCHANNEL\n \param      ucUserPar
   User defined parameter\n

    \note       to be overloaded according to a project specific needs
*/

void _FSF_vApplicationErrorNotify(uint8_t ucSubCheckId, uint8_t ucChannelId,
                                  uint8_t ucUserPar) 
{
#ifndef TH_FSCOM
    static uint8_t ucErrId;
    static uint8_t ucChannel;
    static uint8_t ucUserId;

    for(;;)
    {
        HSUP_vNop();
        ucErrId = ucSubCheckId;
        ucChannel = ucChannelId;        
        ucUserId = ucUserPar;
    }
    /* Axivion Disable Style MisraC2012-2.1: "Unreachable code" */
    (void)ucErrId;
    (void)ucChannel;
    (void)ucUserId;

    return;
    /* Axivion Enable Style MisraC2012-2.1 */
#endif
}

/** Trigger the external watchdog.
    Trigger the external watchdog according the requirements for T3

    \note       to be overloaded according to a project specific needs\n
                Reference chapter 7.5 of [1]
*/
#ifndef TH_FSCOM
    void _FSF_vTriggerExtWatchdog(void) {}
#else
    void _FSF_vTriggerExtWatchdog(void) {}
#endif

/** Get external watchdog state.
    Read in the state of the external watchdog.

    \return     bool    true, the watchdog remains in its safe state, false
   otherwise\n

    \pre        None\n
    \post       Depending on how the external watchdog is really implemented,
   when triggering the external watchdog and subsequently reading back its state
   again there might be some hysteresis to be taken into consideration. This
   has to be solved at application level as this depends on a specific project.
    \note       to be overloaded according to a project specific needs\n
                Reference chapter 7.5 of [1]
*/
bool _FSF_bReadExtWatchdog(void) 
{
    if (FSF_ucTaskState == (uint8_t)FSF_STATE_STARTED)
    {
        return false;
    } 
    else
    {
        return true;
    }
}

/** Trigger the internal watchdog

    \note       to be overloaded according to a project specific needs\n
                Reference chapter 7.4
*/
void _FSF_vTriggerInternalWatchdog(void)
{
#if !defined(TH_FSCOM) && !defined(VARIANT_BOOTMANAGER)
    WDT_trigger();
#endif
}

/** Report a fatal exception.

    \note       to be overloaded according to a project specific needs\n
                Reference chapter 7.2
*/
void _FSF_vReportFatal(void)
{
#ifndef TH_FSCOM
    for(;;)
    {
        HSUP_vNop();
        // Continuously trigger the internal watchdog
        _FSF_vTriggerInternalWatchdog(); //lint !e522 !e523 Highest operation, function '_FSF_vTriggerInternalWatchdog', lacks side-effects
    }
#endif
}


/** Check, if the addressed ROM module is ready for reading.
    Default implementation for checking the ROM modules ready state. \n

    \param      pData       ROM address from where to read \n
    \param      ui16length  length of data about to read \n

    \return     bool        true, if the ROM module is ready for reading, false
   otherwise.\n\n

    \pre        Called only at operational time. Called each time BEFORE start
   of reading\n \post       If false is returned, the current check cycle is
   suspended and resumed again \n when dispatched the next time.\n\n

    \note       Overloading is not designed here, as this is up to an
   application. \n
*/
/*lint -e{818} parameter 'pData' could be pointer to const [MISRA 2012 Rule 8.13, advisory] */
bool _FSF_bIsROMReady(uint8_t *pData, uint16_t ui16length)
{
    (void)*pData;
    (void)ui16length;
    return true;
}

/** Check, if ROM at once is possible to start.
    Default implementation for checking the ROM check ready state. \n

    \param      none

    \return     bool        true, if the ROM check may start, false
   otherwise.\n\n

    \pre        Called only at operational time. Called each time BEFORE start
   of check\n \post       If false is returned, the current check cycle is
   suspended and resumed again \n when dispatched the next time.\n\n

    \note       Overloading is not designed here, as this is up to an
   application. \n
*/
bool _FSF_bIsROMAtOnceAllowed(void)
{
    return true;
}

/** Notify completion of the ROM check.
    This handler get called every time a full ROM check has completed. \n

    \param      none\n

    \pre        Called only at operational time. NOT CALLED IN CASE OF A CRC
   ERROR!\n

    \note       Overloading is not designed here, as this is up to an
   application. \n
*/
void _FSF_vNotifyROMCheckComplete(void) 
{
    //  Left empty. To be overloaded by a concrete application
}

/** Get type of ROM check.
    At run time, an application may define what type of ROM check has to be
   performed. \n

    \param      none\n
    \return     true, if the whole has to be checked at once, false otherwise.

    \pre        Called only at operational time. \n

    \note       Overloading is not designed here, as this is up to an
   application. \n
*/
bool _FSF_bIsROMAtOnce(void) 
{
#ifdef TH_FSCOM
    return TH_FSF_bROM_At_Once;
#else
    return false;
#endif
}

/** While checking the ROM at once, this interface decides if interrupts get
   maintained or not. This is mainly intended due to performance reasons. While
   checking the ROM without maintaining the interrupts, some overflow conditions
   may occurre. Otherwise, maintaining a huge number of interrupt sources is
   time consuming, hence it will lengthen the whole ROM check time. So, by this
   interface, the user has the ability to decide whether to maintain the
   interrupts or not. The ROM channel internally provide this interface and it
   returns true by default. This interface may be overloaded, if necessary. As
   an example: if interrupts need to be  maintained at least once every second,
   and checking the whole ROM space at once takes only half a second, in that
    case there is no need to maintain the interrupt sources while checking.\n

    \param      none\n
    \return     true, if interrupts have to maintain, false otherwise.

    \pre        Called only at operational time.\n

    \note       Overloading is not designed here, as this is up to an
   application. \n
*/
bool _FSF_bCheckINTduringROMCheck(void)
{
    return true;
}

/**
Some little configuration helpers
*/
void FSF_vIsrEntryConf(uint8_t ucIsrId)
{
    (void)ucIsrId;
#ifdef FSF_ISRCONFIG
    /* Memorize the entry timestamp and count the frequency */
    atIsrLongConfig[ucIsrId].uiIEntry = (uint16_t)(TIM_uiGetCircleMicroSeconds());
    atIsrLongConfig[ucIsrId].uiICount++;
#endif
}

/**
Some little configuration helpers
*/
void FSF_vIsrExitConf(uint8_t ucIsrId)
{
    (void)ucIsrId;
#ifdef FSF_ISRCONFIG
    /* Compute the latency */
    atIsrLongConfig[ucIsrId].uiILatency += (uint16_t)(
        TIM_uiGetCircleMicroSeconds() - atIsrLongConfig[ucIsrId].uiIEntry);
#endif
}

/**
Some little configuration helpers
*/
void FSF_vIsrShortEntryConf(uint8_t ucIsrId)
{
    (void)ucIsrId;
#ifdef FSF_ISRCONFIG
    /* Count the frequency */
    atIsrShortConfig[ucIsrId].uiICount++;
#endif
}

/**
Some little configuration helpers
*/
void FSF_vIsrClearConf(uint8_t ucIsrId)
{
    (void)ucIsrId;
#ifdef FSF_ISRCONFIG
    TIsrConfig *p = &atIsrLongConfig[ucIsrId];
    /* Clear all members */
    p->uiICount = (uint16_t)0;
    p->uiIEntry = (uint16_t)0;
    p->uiILatency = (uint16_t)0;
#endif
}

/**
Some little configuration helpers
*/
void FSF_vIsrClearShortConf(uint8_t ucIsrId)
{
    (void)ucIsrId;
#ifdef FSF_ISRCONFIG
    /* Count the frequency */
    atIsrShortConfig[ucIsrId].uiICount = (uint16_t)0;
#endif
}

/* RAM Channel Global Functions*/
#if defined(TH_FSCOM)
    /* Timing measurement helpers */
    // uint16_t  _FSF_Get50NanosecondsTimer  (void)
    //{
    //   return (uint16_t)0;
    //}
#endif

void _FSF_vDoWhenPieceRamCheckStarted(void) 
{
    // This handler get called every time when check of RAM piece (operational
    // RAM check) is started. This function can be used to disable some
    // activites during operational RAM check \n
}
void _FSF_vDoWhenPieceRamCheckFinished(void)
{
    // This handler get called every time when check of RAM piece (operational
    // RAM check) is started. This function again enable disabled activites
    // after RAM piece check.
}

/* RAM Consistency Channel Global Functions*/
void _FSF_RAM_CON_vDoCheckInFsComModule(void)
{
#ifdef FSF_RAM_CON_BY_CHECK_VALUE
    _FSF_RAM_CON_vFsComModuleCheckBySum();
#else
    #ifdef FSF_RAM_CON_BY_MIRROR
        _FSF_RAM_CON_vFsComModuleCheckByMirror();
    #else
        #if (!defined(VARIANT_BOOTMANAGER)) && (!defined(TH_FSCOM))
        #warning "RAM Consistency Check is not active"
        #endif
    #endif // FSF_RAM_CON_BY_MIRROR
#endif // FSF_RAM_CON_BY_CHECK_VALUE
}

void _FSF_RAM_CON_vDoUpdateCheckInFsComModule(void)
{
#ifdef FSF_RAM_CON_BY_CHECK_VALUE
    _FSF_RAM_CON_vFsComModuleUpdateCheckBySum();
#else
    #ifdef FSF_RAM_CON_BY_MIRROR
        _FSF_RAM_CON_vFsComModuleUpdateCheckByMirror();
    #else
        /* do nothing */
    #endif // FSF_RAM_CON_BY_MIRROR
#endif // FSF_RAM_CON_BY_CHECK_VALUE
}

void _FSF_RAM_CON_vSetErrCounter(void)
{
#ifdef FSF_RAM_CON_BY_CHECK_VALUE
    _FSF_RAM_CON_vSetErrCntBySum();
#else
    #ifdef FSF_RAM_CON_BY_MIRROR
        _FSF_RAM_CON_vSetErrCntByMirror();
    #else
        _FSF_RAM_CON_vSetErrCntWithoutRamCon();
    #endif // FSF_RAM_CON_BY_MIRROR
#endif // FSF_RAM_CON_BY_CHECK_VALUE
}

void _FSF_RAM_CON_vDoCheckInRomModule(void)
{
#ifdef FSF_RAM_CON_BY_CHECK_VALUE
    _FSF_RAM_CON_vRomModuleCheckBySum();
#else
    #ifdef FSF_RAM_CON_BY_MIRROR
        _FSF_RAM_CON_vRomModuleCheckByMirror();
    #else
        /* do nothing */
    #endif // FSF_RAM_CON_BY_MIRROR
#endif // FSF_RAM_CON_BY_CHECK_VALUE
}

void _FSF_RAM_CON_vDoUpdateCheckInRomModule(void)
{
#ifdef FSF_RAM_CON_BY_CHECK_VALUE
    _FSF_RAM_CON_vRomModuleUpdateCheckBySum();
#else
    #ifdef FSF_RAM_CON_BY_MIRROR
        _FSF_RAM_CON_vRomModuleUpdateCheckByMirror();
    #else
        /* do nothing */
    #endif // FSF_RAM_CON_BY_MIRROR
#endif // FSF_RAM_CON_BY_CHECK_VALUE
}

#ifdef FSF_CRC_32_BIT
/** Calculate a CRC-32 value over a data range.

    \param      CRC32Value  current CRC-16 value \n
    \param      pData       data address where to start calculation \n
    \param      ui16length  length of data to calculate \n\n

    \return     uint16_t  newly calculated value.\n\n

    \pre        Called only at operational time. \n
    \post       ROM channel work area left unchanged \n

    \note       This calculator is only allowed to call for ROM parts that are
   ready for access. \n
*/
uint32_t uiCalculateCRC32(uint32_t CRC32Value,
                          FSTH_CONST uint32_t *pData,
                          uint16_t ui16length)
{
#ifdef FSF_SW_CRC
    uint8_t temp;
    uint8_t *p_temp_data = (uint8_t *)pData;
    uint32_t crc = CRC32Value;
    uint32_t Size = (uint32_t)(ui16length * sizeof(uint32_t));

    while (Size--) {
        temp = (crc >> 24);
        crc = (crc << 8) ^ CRCTable[temp ^ *p_temp_data];
        p_temp_data++;
    }
    return crc;
#else
    return (CRC_Ethernet_Update(CRC32Value,
                                (uint32_t)(ui16length * sizeof(uint32_t)),
                                (const uint8_t *)pData));  //lint !e928 cast tested to work correctly
#endif  // FSF_SW_CRC
}

#else

/** Calculate a CRC-16 value over a data range.

    \param      CRC16Value  current CRC-16 value \n
    \param      pData       data address where to start calculation \n
    \param      ui16length  length of data to calculate \n\n

    \return     uint16_t    newly calculated value.\n\n

    \pre        Called only at operational time. \n
    \post       ROM channel work area left unchanged \n

    \note       This calculator is only allowed to call for ROM parts that are
   ready for access. \n
*/
uint16_t uiCalculateCRC(uint16_t CRC16Value, FSTH_CONST uint8_t *pData,
                        uint16_t ui16length)
{
    uint8_t tmp, locrc, hicrc;
    while (ui16length != (uint16_t)0) {
        hicrc = (uint8_t)CRC16Value;
        locrc = (uint8_t)(CRC16Value >> 8);
        tmp = locrc ^= *pData;
        //  Msg(2:0506) Dereferencing pointer value that is possibly NULL.
        //  REFERENCE - ISO-6.3.3.2 Semantics
        //  PCA comment: code tested and reviewed to work correctly. Thus, left
        //  unchanged.
        pData++;
        //  Msg(4:0489) Increment or decrement operation performed on pointer.
        //  MISRA-C:2004 Rule 17.4 <next>
        //  PCA comment: code tested and reviewed to work correctly. Thus, left
        //  unchanged.

        ui16length--;

        locrc ^= (tmp >> 4);
        hicrc ^= (locrc << 4);
        hicrc ^= (locrc >> 3);
        locrc ^= (locrc << 5);

        CRC16Value = (((uint16_t)hicrc) << 8) | locrc;
    }
    return CRC16Value;
}

#endif

uint32_t _FSF_ui32ResetCrcCalculation(void) 
{
    return CRC_Ethernet_Init();
    /*
    #if defined(STM32_ARM) && !defined(SYSTEMTEST_F20)
            CRC_CR_RESET = 1; / Resets the CRC calculation unit and sets the
    data register to 0xFFFFFFFF #endif #if defined(FM4) &&
    !defined(SYSTEMTEST_F20) CRC_CRCCR = 0x03; / Resets the CRC calculation and
    sets the data register to 0xFFFFFFFF #endif
    */
}

uint32_t _FSF_ui32FinalizeCrcCalculation(uint32_t crc)
{
#ifdef __Cypress__
    return ~crc;
#else
    return CRC_Ethernet_Finish(crc);
#endif
}

void FSF_vSetApplicationSafeState(void)
{
    /* here should be app specific implementation if it is needed */
}


/** \brief Do One Time ROM check.
    One Time ROM check implementation for calculate CRC in one function calling.
   \n

    \param      none\n

    \return     none\n

    \pre        Called in FSF_vBoot function during microcontroler
   initialization \n \post       If error, go into fatal error reaction \n

    \note       Overloading is not designed here,as this is up to an
   application. \n
*/
void FSF_vStartUpRomCrcCheck(void)
{
#if defined(FUNCTIONAL_SAFETY_ROM_CHECK_ACTIVE) && (defined(VARIANT_BOOTMANAGER) || defined(WIRED_FIRMWARE_UPDATE))
    if(!FWU_isIntegCheckDisabledCalc())
    {
        /* BootManager or romLoader */
        #if defined(WIRED_FIRMWARE_UPDATE) || defined(VARIANT_BOOTMANAGER)
            uint32_t blCrcExpected  = *((uint32_t*)(FS_BL_CRC_ADR));
            uint32_t blCrcLength = (uint32_t)((FS_BL_END_ADR - FS_BL_START_ADR) + 1u);
            const uint8_t *blCrcData = (const uint8_t *)FS_BL_START_ADR;
            uint32_t blCrcCalc;

            blCrcCalc = CRC_Ethernet_Calc(blCrcLength, blCrcData);

            if (blCrcCalc != blCrcExpected)
            {
                if(!FWU_isIntegCheckErrorIgnored())
                {
                    FSF_vReportFatal();
                }
            }
        #endif

        /* Application only for FWU1 because BootManager check CRC of App */
        #if defined(WIRED_FIRMWARE_UPDATE)
            uint32_t appCrcExpected = *((uint32_t*)(FS_APP_CRC_ADR));
            uint32_t appCrcLength = (uint32_t)((FS_APP_END_ADR - FS_APP_START_ADR) + 1u);
            const uint8_t *appCrcData = (const uint8_t *)FS_APP_START_ADR;
            uint32_t appCrcCalc;

            appCrcCalc = CRC_Ethernet_Calc(appCrcLength, appCrcData);

            if (appCrcCalc != appCrcExpected)
            {
                if(!FWU_isIntegCheckErrorIgnored())
                {
                    FSF_vReportFatal();
                }
            }
        #endif
    }
#endif
}
/*lint -restore -e714 -e765 -e923*/
