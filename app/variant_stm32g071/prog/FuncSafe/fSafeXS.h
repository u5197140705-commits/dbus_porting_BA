/*******************************************************************************
 *  Copyright (c) 2019 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          SW_LIB
 *  COMP_ABBREV      FSF
 ******************************************************************************/

#ifndef __FSAFEXS_H
#define __FSAFEXS_H

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include "LibTypes.h"

#include "fSafe.h"
#include "processor.h"
#include "firmware_update/mal/fwu_opt_bytes.h"
/* DEFINITIONS ****************************************************************/

/* Switch off:
 * info 755: global macro not referenced [MISRA 2012 Rule 2.5, advisory]
 * info 757: global declarator 'FSF_arMarchDataBuffer' not referenced
 * info 758: global enum not referenced
 * info 768: global structure member not referenced
 * info 769: global enumeration constant not referenced
 * Warning 526: symbol 'xx' is not defined
 *    seems to be a lintplus issue the mentioned symbols are defined
 * Note 9045: complete definition of 'xx' is unnecessary in this translation unit [MISRA 2012 Directive 4.8, advisory]
 */
/*lint -save -e755 -e757 -e758 -e768 -e769 -e526 -e9045*/

/* Channel Addressing Identifiers
*/

/*
Since FSCOM does not know details on a particular application, the channels an application
provides will be addressed by its pre-defined identifier as listed below. Applications are
NOT completely free in allocating identifiers. Points to consider:
1.) Applications have to add their new channels by allocating channel identifiers which have to start from FSF_APP_CHANNEL_0
2.) The order of the FSF_REG_TEST (must always be located at position zero) and the FSF_RAM_TEST  (must always be located at position 1) is fixed and not allowed to change
3.) Channels related to a typical controller (Register, RAM, ROM, Interrupt and Stack)
cannot be used by applications and cannot be replaced freely.
*/

enum    FSF_CHANNEL_ID {


        FSF_REG_TEST,                    //!< ID for reg. checking
        FSF_RAM_TEST,                    //!< ID for RAM checking
        FSF_FIRST_APPL_TEST      = 2u,   //!< ID of first oper. test
        FSF_ROM_TEST             = 2u,   //!< ID for ROM checking
        FSF_STACK_TEST,                  //!< ID for stack checking
        FSF_ISR_CHECK,                   //!< ID for functional safety interrupt service routine task handler
        FSF_ISR_LATENCY_CHECK,           //!< ID for interrupt service routine latency monitoring
        FSF_ISR_MIN_FREQUENCY_CHECK,     //!< ID for interrupt service routine minimum frequency check
        FSF_ISR_MAX_FREQUENCY_CHECK,     //!< ID for interrupt service routine maximum frequency check
        FSF_RAM_CONSISTENCY_CHECK,       //!< ID for RAM variables consistency check
        FSF_APP_CHANNEL_0,               //!< ID for application channel 0
        FSF_APP_CHANNEL_1,               //!< ID for application channel 1
        FSF_APP_CHANNEL_2,               //!< ID for application channel 2
        FSF_APP_CHANNEL_3,               //!< ID for application channel 3
        FSF_APP_CHANNEL_4,               //!< ID for application channel 4
        FSF_APP_CHANNEL_5,               //!< ID for application channel 5
        FSF_APP_CHANNEL_6,               //!< ID for application channel 6
        FSF_APP_CHANNEL_7,               //!< ID for application channel 7
        FSF_APP_CHANNEL_8,               //!< ID for application channel 8
        FSF_APP_CHANNEL_9,               //!< ID for application channel 9
        FSF_APP_CHANNEL_10,              //!< ID for application channel 10
        FSF_APP_CHANNEL_11,              //!< ID for application channel 11
        FSF_APP_CHANNEL_12,              //!< ID for application channel 12
        FSF_APP_CHANNEL_13,              //!< ID for application channel 13
        FSF_APP_CHANNEL_14,              //!< ID for application channel 14
        FSF_APP_CHANNEL_15,              //!< ID for application channel 15
        FSF_LASTCHANNEL,                 //!< ID for the last available channel
        FSF_NOCHANNEL             = 250u,//!< ID to be used for not available channel
        FSF_NOT_SPECIFIED_CHANNEL = 250u,//FSF_NOCHANNEL should not be used...
        FSF_EXT_WDOG,                    //!< ID used for external watchdog error (not a specific channel)
        FSF_LASTID                       //!< Number of channel ID's
};


/** The first channel available for an application will be starting from FSF_APP_CHANNEL_0.
*/

/* Watchdog and scheduling definitions*/

#define T1_WD_CYCLE             28u     //!< Milliseconds (at least!)
#define T2_WD_CYCLE             20u     //!< Milliseconds (maximum)
#define T3_WD_CYCLE             2u      //!< Microseconds (maximum)
#define DEFAULT_PRESCHEDCYCLES  10u     //!< Scheduling cycles passed before normal operation
#define EXTWD_PRETRIGGERS       1000u   //!< Count down these number of cycles before reading the external watchdog device

/* Local macros ***************************************************************************************/


#ifndef TH_FSCOM
/*ISR definitions*/

/*Description of configuration*/

/* DEFINITIONS  for ISR controlling
********************************************************************/

/** \b Full interrupt control: Non-standardised enumeration.


Information to Control table for interrupt service routines   \n
<TABLE style="text-align: center">
<TR style="background-color:rgb(255,255,206)">
<TD> id number </TD>
<TD> Vector Address </TD>
<TD> Vector number </TD>
<TD> Interrupt Vector </TD>
<TD> Interrupt number </TD>
<TD> Define FS </TD>
<TD> Typ </TD>
<TD> Interrupt Source </TD>
<TD> Function </TD>
<TD> Define FV </TD>
<TD> SW modul </TD>
<TD> Latency Frequency </TD>
<TD> Max Frequency </TD>
<TD> Min Frequency </TD>
<TD> uiCycleTime </TD>
</TR>

<TR>
<TD> 0 </TD>
<TD> - </TD>
<TD> - </TD>
<TD> - </TD>
<TD> - </TD>
<TD> FS_ISR_SCI_TX </TD>
<TD> Long </TD>
<TD> - </TD>
<TD> used bv the D-Bus-2 library </TD>
<TD> - </TD>
<TD> dBus2Dll.lib </TD>
<TD> 100 </TD>
<TD> 0 </TD>
<TD> 0 </TD>
<TD> 10000/256 </TD>
</TR>

<TR>
<TD> 1 </TD>
<TD> - </TD>
<TD> - </TD>
<TD> - </TD>
<TD> - </TD>
<TD> FS_ISR_SCI_RX </TD>
<TD> Long </TD>
<TD> - </TD>
<TD> used bv the D-Bus-2 library </TD>
<TD> - </TD>
<TD> dBus2Dll.lib </TD>
<TD> 100 </TD>
<TD> 0 </TD>
<TD> 0 </TD>
<TD> 10000/256 </TD>
</TR>

<TR>
<TD> 2 </TD>
<TD> - </TD>
<TD> - </TD>
<TD> - </TD>
<TD> - </TD>
<TD> FS_ISR_SPI_TX </TD>
<TD> Long </TD>
<TD> - </TD>
<TD> used bv the SPI library </TD>
<TD> - </TD>
<TD> - </TD>
<TD> 100 </TD>
<TD> 0 </TD>
<TD> 0 </TD>
<TD> 10000/256 </TD>
</TR>

<TR>
<TD> 3 </TD>
<TD> - </TD>
<TD> - </TD>
<TD> - </TD>
<TD> - </TD>
<TD> FS_ISR_SPI_RX </TD>
<TD> Long </TD>
<TD> - </TD>
<TD> used bv the SPI library </TD>
<TD> - </TD>
<TD> - </TD>
<TD> 100 </TD>
<TD> 0 </TD>
<TD> 0 </TD>
<TD> 10000/256 </TD>
</TR>

<TR>
<TD> 4 </TD>
<TD> - </TD>
<TD> - </TD>
<TD> - </TD>
<TD> - </TD>
<TD> FS_ISR_SCI2_TX </TD>
<TD> Long </TD>
<TD> - </TD>
<TD> - </TD>
<TD> - </TD>
<TD> - </TD>
<TD> 100 </TD>
<TD> 0 </TD>
<TD> 0 </TD>
<TD> 10000/256 </TD>
</TR>

<TR>
<TD> 5 </TD>
<TD> $FFEA </TD>
<TD> 10 </TD>
<TD> INTVEC_TIM2_OVF </TD>
<TD> 0 </TD>
<TD> FSF_ISR_OVF </TD>
<TD> Short </TD>
<TD> TIM2 Overflow </TD>
<TD> for tacho speed measurment </TD>
<TD> FSF_UNI_vIntrlTovl() </TD>
<TD> sync.c </TD>
<TD> 0 </TD>
<TD> 12 </TD>
<TD> 8 </TD>
<TD> 20 </TD>
</TR>

<TR>
<TD> 6 </TD>
<TD> $FFEE </TD>
<TD> 8 </TD>
<TD> INTVEC_TIM2_CH0 </TD>
<TD> 1 </TD>
<TD> FSF_ISR_TACHO </TD>
<TD> Short </TD>
<TD> TIM2 Channel 0 </TD>
<TD> handling upon an capture event </TD>
<TD> FSF_UNI_vIntrlCAPTTacho() </TD>
<TD> unimot.c </TD>
<TD> 0 </TD>
<TD> 14400 </TD>
<TD> 0 </TD>
<TD> 15 </TD>
</TR>

<TR>
<TD> 7 </TD>
<TD> $FFE4 </TD>
<TD> 5 </TD>
<TD> INTVEC_TIM1_CH0 </TD>
<TD> 2 </TD>
<TD> FSF_ISR_NETSYNC </TD>
<TD> Short </TD>
<TD> TIM1 Channel 0 </TD>
<TD> net synchronisation </TD>
<TD> SYC_vIntrlCAPSync() </TD>
<TD> sync.c </TD>
<TD> 0 </TD>
<TD> 1000 </TD>
<TD> 0 </TD>
<TD> 100 </TD>
</TR>

<TR>
<TD> 8 </TD>
<TD> $FFEC </TD>
<TD> 9 </TD>
<TD> INTVEC_TIM2_CH1 </TD>
<TD> 3 </TD>
<TD> FSF_ISR_WINSYNC </TD>
<TD> Short </TD>
<TD> TIM2 Channel 1 </TD>
<TD> halfwave or sync window or synthetic 50Hz </TD>
<TD> SYC_vIntrOCMPSyncWindow() </TD>
<TD> unimot.c </TD>
<TD> 0 </TD>
<TD> 1500 </TD>
<TD> 0 </TD>
<TD> 100 </TD>
</TR>

<TR>
<TD> 9 </TD>
<TD> $FFF2 </TD>
<TD> 6 </TD>
<TD> INTVEC_TIM1_CH1 </TD>
<TD> 4 </TD>
<TD> FSF_ISR_IGNITION </TD>
<TD> Short </TD>
<TD> TIM1 Channel 1 </TD>
<TD> Ignition control </TD>
<TD> UNI_vIntrOCMPIgnition() </TD>
<TD> unimot.c </TD>
<TD> 0 </TD>
<TD> 10000 </TD>
<TD> 0 </TD>
<TD> 100 </TD>
</TR>

</TABLE>

This enumeration is needed in interrupts for controlling \b runtime and \b frequency of the interrupts.
Each interrupt, which is not standardised, but needs full interrupt control (runtime and frequency check) has its own ID according to this enumeration.
Please refer to #FSF_eStandardisedIsrControl for a definition of standardised IDs (standardised IDs may not be reused in this enumeration).*/
enum FSF_eFullIsrControl{
        FS_ISR_SCI_TX,  //!<Standardised ID for SCI_TX interrupt, which is used by the D-Bus-2 library.
        FS_ISR_SCI_RX,  //!<Standardised ID for SCI_RX interrupt, which is used by the D-Bus-2 library.
        FS_ISR_SPI_TX,  //!<Standardised ID for SPI_TX interrupt, which is used by the SPI library.
        FS_ISR_SPI_RX,  //!<Standardised ID for SPI_RX interrupt, which is used by the SPI library.
/* End of standardised interrupt ids. The following ids are project specific. */
/* NB! Make sure that no interrupt has the same id as the standardised! */
//        FS_ISR_SCI2_TX = 4, //!<Enum element for identifying SCI2_TX interrupt. This is the first element, which is not standardised.
//        FS_ISR_SCI2_RX, //!<Enum element for identifying an interrupt
//        FS_ISR_SCI2_ERR,//!<Enum element for identifying an interrupt
//        FS_ISR_ADC,     //!<Enum element for identifying an interrupt
//        FS_ISR_KBRD,    //!<Enum element for identifying an interrupt
//        FS_ISR_SC1_ERR, //!<Enum element for identifying an interrupt
//        FS_ISR_MMIIC,   //!<Enum element for identifying an interrupt
        FS_ISR_NUMBER_OF_LARGE_ISR //!<Last enum element in enumeration used for identifying interrupts, which represents how many interrupts are considered.
};

/*#define FS_USE_SHORT_ISR_CONTROL*/
#ifdef DOXY_DEVELOPERS_DOC
//Doxygen documentation of definition
#define FS_USE_SHORT_ISR_CONTROL //!< Definition used in combination with #ifdef to check whether short interrupt control is needed (FS_ISR_NUMBER_OF_SHORT_ISR cannot be used in combination with #ifdef or #if as this is not a #define, but an enumerated value...).
#endif

#ifndef DOXY_DEVELOPERS_DOC
    //Doxygen documentation of definition
    #define FS_USE_SHORT_ISR_CONTROL //!< Definition used in combination with #ifdef to check whether short interrupt control is needed (FS_ISR_NUMBER_OF_SHORT_ISR cannot be used in combination with #ifdef or #if as this is not a #define, but an enumerated value...).
#endif

#ifdef FS_USE_SHORT_ISR_CONTROL
    /** \b Short interrupt control: Non-standardised enumeration.

    This enumeration is needed in interrupts for controlling the \b frequency of interrupts.
    Each interrupt, which only needs frequency check, has its own ID according to this enumeration.*/
    enum FSF_eShortIsrControl{
        FS_ISR_EXAMPLE_INT,         //!<Enum element for identifying an interrupt
        FS_ISR_NUMBER_OF_SHORT_ISR  //!< Last enum element in enumeration used for identifying interrupts, which represents how many interrupts are considered by short interrupt control. NB! If no interrupts belong to this group, hence FS_ISR_NUMBER_OF_SHORT_ISR will have the value 0, then it is important not to define: \b FS_USE_SHORT_ISR_CONTROL
    };
#else
    //No Short interrupt control needed.
#endif

#endif

/* ROM definitions ***********************************************************/

/** ROM Check cycle time for the ROM check in 100 millisecond pieces */
#if defined(RTOS) || defined(ZRTOS)
#define ROM_CYCLE_CHECK_TIME        200u
#else
#define ROM_CYCLE_CHECK_TIME        20u
#endif

/** ROM CRC Error passed to the application */
#define FSF_ROM_CRC_ERROR           0u

/** Inital value of crc. */
#define FSF_CRC_INIT                0xFFFFFFFFuL


/* RAM definitions ***********************************************************/
/** RAM Check cycle time for the ROM check in 100 millisecond pieces */
#if defined(RTOS) || defined(ZRTOS)
    #define RAM_CYCLE_CHECK_TIME    500u              //!< 100 Milliseconds packets (at least!)
#else
    #define RAM_CYCLE_CHECK_TIME    50u               //!< 100 Milliseconds packets (at least!)
#endif

#define MARCH_PATTERN_0_0           (uint8_t)0x00     //!< Check pattern 0 (not inverted)
#define MARCH_PATTERN_1_0           (uint8_t)0xFF     //!< Check pattern 0 (inverted)

#define MARCH_PATTERN_0_1           (uint8_t)0xF0     //!< Check pattern 1 (not inverted)
#define MARCH_PATTERN_1_1           (uint8_t)0x0F     //!< Check pattern 1 (inverted)

#define MARCH_PATTERN_0_2           (uint8_t)0x33     //!< Check pattern 2 (not inverted)
#define MARCH_PATTERN_1_2           (uint8_t)0xCC     //!< Check pattern 2 (inverted)

#define MARCH_PATTERN_0_3           (uint8_t)0x55     //!< Check pattern 3 (not inverted)
#define MARCH_PATTERN_1_3           (uint8_t)0xAA     //!< Check pattern 3 (inverted)

#define RAM_BLOCK_OVERLAP          (2 * sizeof(uint32_t))

#ifdef PSOC4
    #define RAM_SIZE_OF_EXCLUDED_DATA           ((uint32_t)VECTOR_TABLE_SIZE + (uint32_t)FWU_SHARED_DATA_SIZE + (uint32_t)UNINIT_DATA_SIZE)
#else
    #define RAM_SIZE_OF_EXCLUDED_DATA           ((uint32_t)FWU_SHARED_DATA_SIZE + (uint32_t)UNINIT_DATA_SIZE)
#endif

/* STACK definitions *********************************************************/
#define NUMBER_OF_STACKS (1u)

/* Task Scheduler Definitions ************************************************/
// Module definitions, dependencies and priorities have only meaning when dealing with the
// scheduler, they are left off when included elsewhere. Applications have to include this
// header, but using its own definitions.

/* Dependencies */
#define DEP_STACK   MOD_FSF,
#define DEP_ROM     MOD_FSF,
#define DEP_ISR     MOD_FSF,
#define DEP_FSF     MOD_TIM,
#define DEP_REG     MOD_FSF,

/* Priorities */
#define FSF_STACK_PRIO      SCH_PRIOR_HI
#define FSF_ROM_PRIO        SCH_PRIOR_HI
#define FSF_ISR_PRIO        SCH_PRIOR_HI
#define FSF_REG_PRIO        SCH_PRIOR_HI
#define FSF_FSF_PRIO        SCH_PRIOR_LO


/* Application channel definition ********************************************/
#define FSF_NR_OF_CHANNELS      FSF_APP_CHANNEL_0   //!< No Application channel available


/*****************************************************************************/
/* TYPE DEFINITIONS **********************************************************/
/*****************************************************************************/

/* ISR Type definitions ******************************************************/

/**
\brief Structure for interrupt service routine limitations, used for interrupts with time management.

Please refer to TisrSmallControl for short interrupt service routines.
*/
typedef struct _TisrControl {
    uint16_t uiLimitLatency;        ///< max allowed (cycle peak) latency time in micro seconds, 0 ->don´t care
    uint16_t uiLimitMaxFrequency;   ///< max allowed events per cycle, 0 ->don´t care
    uint16_t uiLimitMinFrequency;   ///< min events per cycle, 0 ->don´t care
    uint16_t uiCycleTime;           ///< this is the time for one test cycle in 0.065535s units -> when set to 100, channels interrupts are checked every 6.5535s
    uint8_t  ucControl;             ///< define what to supervise at run time
} TisrControl;

/**
\brief Structure for \b short interrupt service routine limitations, used for interrupts with time management

Please refer to TisrControl for larger interrupt service routines.
*/
typedef struct _TisrShortControl {
    uint16_t uiLimitMaxFrequency;   ///< max allowed events per cycle, 0 ->don´t care
    uint16_t uiLimitMinFrequency;   ///< min events per cycle, 0 ->don´t care
    uint16_t uiCycleTime;           ///< this is the time for one test cycle in 0.065535s units -> when set to 100, channels interrupts are checked every 6.5535s
    uint8_t  ucControl;             ///< define what to supervise at run time
} TisrShortControl;

/**
Data structure for supporting configuration of interrupt frequency and latency. For each interrupt configured at the application side one entry exists.
*/
typedef struct _TIsrConfig {
    uint16_t uiICount;  ///< Count frequency
    uint16_t uiIEntry;  ///< Memorize entry timestamp
    uint16_t uiILatency;///< Computed latency
} TIsrConfig;


/* ROM Type definitions ******************************************************/

/* Large ROM Type Definitions */
/**
\brief ROM module structure definitions and Read/Ready functions.
*/
/** Check, if the addressed ROM module is ready for reading.
    An application has to provide this check routine, especially for those type of ROM \n
    modules that are accessed via external or serial connections (i.e. EEPROM). When using \n
    externally connected ROM modules, some data buffering might be used in order to improve \n
    reading. Thus, the data length will be passed too, so data read ahead is supported. \n\n

    \param      uint8_t *     ROM address from where to read \n
    \param      uint16_t      length of data about to read \n

    \return     bool        true, if the ROM module is ready for reading, false otherwise.\n\n

    \pre        Called only at operational time. Called each time BEFORE start of reading\n
    \post       If false is returned, the currect check cycle is supended and resumed again \n
                when dispatched the next time.\n\n

    \note       Overloading is not designed here, as this is up to an application. \n
*/
typedef bool (*T_ROM_Ready)(uint8_t * , uint16_t ); /*lint !e955 Parameter name missing from prototype for function */

/** Read data from ROM module.
    Reading data from ROM modules is completely up to a specific application. So, \n
    function pointers are used to encapsulate this functionality. \n\n

    \param      uint8_t *     ROM address where to read from\n
    \param      uint16_t      length of data to read \n\n

    \return     uint8_t *     address of data that have been read, or NULL.\n\n

    \pre        Called only at operational time. Called only for ROM modules ready for reading \n
    \post       If NULL is returned, data couldn't read as requiested. In that case the currect \n
                check cycle is supended and resumed again when dispatched the next time.\n\n

    \note       Overloading is not designed here,as this is up to an application. \n
*/
typedef uint8_t *(* T_ROM_Read) (uint8_t *, uint16_t ); /*lint !e955 Parameter name missing from prototype for function */

/** The ROM Segment descriptor */
typedef struct _T_ROM_Segment{
    uint8_t *pucStartSeg; //!< Pointer to start of segment
    uint8_t *pucEndSeg;   //!< Pointer to end of segment
} T_ROM_Segment;

/** The ROM Module descriptor */
typedef struct _T_ROM_Module{
    FSTH_CONST T_ROM_Segment * patSegment;  //!< Pointer to segment table
    uint8_t ucSegmentCount;                 //!< Count of segments in that table
    uint16_t ui16CycleBytesCount;       //!< Count of bytes to be calculated per cycle
    T_ROM_Read pfROMRead;               //!< Function pointer to ROM read routine
    T_ROM_Ready pfROMReady;             //!< Function pointer to ROM ready routine
#ifdef FSF_CRC_32_BIT
    uint32_t *pui32CRCValue;          //!< Pointer to CRC value belonging to this ROM module
#else
    uint16_t *pui16CRCValue;          //!< Pointer to CRC value belonging to this ROM module
#endif
} T_ROM_Module;

/* RAM Type definitions ******************************************************/
/**
\brief RAM module structure definitions
*/
/** The RAM Segment descriptor */
typedef struct _T_RAM_Segment{
    uint8_t * pucStartSeg;  //!< Pointer to start of RAM segment
    uint8_t * pucEndSeg;        //!< Pointer to end of RAM segment
} T_RAM_Segment;

/** The Check Pattern descriptor */
typedef struct _T_RAM_Pattern {
    uint8_t ucPatNotInv; //!< Pattern not inverted
    uint8_t ucPatInvers; //!< Pattern inverted
} T_RAM_Pattern;


/** The RAM Module descriptor */
typedef struct _T_RAM_Module {
    uint8_t ucBufferLength; //!< Length of data exchange buffer
    uint8_t ucSegmentCount; //!< Count of segments in that table
    uint8_t ucPatternCount; //!< Count of patterns used for the RAM check
} T_RAM_Module;


/** Result of operational RAM check*/
typedef enum _TRunTimeRamTestStatus {
    FSF_RAM_TESK_OK       = 0,
    FSF_RAM_TEST_FAILURE  = 1,
    FSF_RAM_TEST_NO_OF_STATES
}TRunTimeRamTestStatus;

/* Stack Type definitions ****************************************************/
/**
\brief Stack structure definitions.
*/
/** The Stack descriptor */
typedef struct _T_Stack {
    uint16_t * pui16FirstEntry; //!< Points to first WORD(!!!) entry
    uint16_t * pui16LastEntry;  //!< Points to last WORD(!!!) entry
} T_Stack;


/*****************************************************************************/
/* DECLARATIONS **************************************************************/
/*****************************************************************************/
/*AXIVION Disable Style MisraC2012-8.11 : array size not known here */
/* ISR Channel Declarations */
extern uint16_t FSF_auiIsrTime[];
extern uint16_t FSF_auiIsrCount[];
extern uint16_t FSF_auiShortIsrCount[];

extern FSTH_CONST TisrControl FSF_atIsrCntrTbl[];
extern FSTH_CONST TisrShortControl FSF_atIsrShortCntrTbl[];


#ifdef TH_FSCOM
extern volatile FSTH_CONST uint8_t FSF_ucNumberOfInterrupts;
extern volatile FSTH_CONST uint8_t FSF_ucNumberOfShortInterrupts;
#else   // TH_FSCOM
extern FSTH_CONST uint8_t FSF_ucNumberOfInterrupts;
extern FSTH_CONST uint8_t FSF_ucNumberOfShortInterrupts;
#endif  // TH_FSCOM

extern FSTH_CONST uint16_t FSF_uiIsrClockCycle;

#ifdef FSF_ISRCONFIG
extern TIsrConfig atIsrLongConfig[];
extern TIsrConfig atIsrShortConfig[];
#endif

/* ROM Channel Declarations */
/** A typical ROM configuration */
extern FSTH_CONST T_ROM_Segment FSF_atRomSegments_0[];  //!< ROM segments table
extern FSTH_CONST T_ROM_Module FSF_atRomModules[];      //!< ROM module table
extern FSTH_CONST uint8_t FSF_ucRomModuleCount;         //!< Counter of ROM modules
extern FSTH_CONST uint16_t * FSTH_CONST FSF_puiCRC_Table;       //!< Pointer to CRC Table
extern FSTH_CONST T_FSCOM_Channel FSF_sROMChannel;      //!< The ROM channel

/* RAM Channel Declarations */
/** A typical RAM configuration */
__ALIGNED(4) extern uint8_t FSF_arMarchDataBuffer[]; //!< RAM Data Exchange Buffer


//#ifdef STM32_ARM
//extern FSTH_CONST T_RAM_Segment FSF_atOtRamSegments;          //!< RAM segments table for OneTimeCheck
//#else
extern FSTH_CONST T_RAM_Segment FSF_atOtRamSegments[];          //!< RAM segments table for OneTimeCheck
//#endif
extern FSTH_CONST uint8_t FSF_ucOtRamSegCnt;
extern FSTH_CONST T_RAM_Segment FSF_atRamSegments[];        //!< RAM segments table
extern FSTH_CONST T_RAM_Pattern FSF_asPatternTable[];   //!< RAM check pattern table
extern FSTH_CONST T_RAM_Module FSF_atRamModule;         //!< RAM module descriptor
extern FSTH_CONST T_FSCOM_Channel FSF_sRAMChannel;      //!< The RAM channel


/* Stack Channel Declarations */
#ifndef TH_FSCOM
#ifdef MULTIPLE_STACK
extern FSTH_CONST T_Stack _FSF_Stack[2];
#else   // MULTIPLE_STACK
extern FSTH_CONST T_Stack _FSF_Stack[1];
#endif  // MULTIPLE_STACK
#endif
extern FSTH_CONST uint16_t _FSF_ui16StackMagic;

/*AXIVION Enable Style MisraC2012-8.11 */

/*****************************************************************************/
/* GLOBAL FUNCTIONS **********************************************************/
/*****************************************************************************/

/* Default channel functions */

/** Default Precondition.

    Return precondition true by default.
    Return precondition true in either case.

    \return     bool    true.

    \pre        None\n
    \note       to be overloaded according to a project specific needs\n
*/
extern bool _FSF_bDefaultPreconditionTRUE(void);

/** Default One-time check.

    Do nothing.

    \pre        None\n
    \note       to be overloaded according to a project specific needs\n
*/
extern void _FSF_vDefaultOneTimeCheck(void);

/* RAM channel functions */
/** The RAM one-time and operational check */
//  The wrapper function
extern void FSF_vRam_OneTimeCheck(void);

//  The real checker(s)
extern void FSF_vRamcheck(const T_RAM_Segment* pucRamSegments, uint32_t u32NumberOfRamSegments, uint32_t u32ExcludedData);

/** Perform one RAM check cycle.
    This service provide the means of cheking a piece of RAM.

    \param      pucRAM          pointer to the beginning of the RAM part that is to be checked
                                Length of the RAM is globally defined by MARCHBUFLENGTH

                pucDEXBuffer    pointer to the beginning of the RAM data exchange buffer. Length
                                of this buffer is equal to the part that is to be checked

                ucPatNoInv      The actual non-inverted check pattern to be used.

                ucPatInv        The actual inverted check pattern to be used.

    \pre        The one-time RAM check performed successfully.\n
    \post       Interrupts left untouched. The checked RAM space correctly restored.\n
*/
extern void FSF_vRamOpercheck(uint8_t * pucRAM, uint8_t * pucDEXBuffer, uint8_t ucPatNoInv, uint8_t ucPatInv);

/** Notify start of the piece RAM check.
    This handler get called every time when check of RAM piece (operational RAM check) is started. This function can be used to disable some activites during operational RAM check (DMA or DTC). \n

    \param      none\n

    \pre        Called only at operational time. \n

    \note       Overloading is not designed here, as this is up to an application. \n
*/
extern void _FSF_vDoWhenPieceRamCheckStarted(void);

/** Notify completion of piece RAM  check.
    This handler get called every time when check of RAM piece (operational RAM check) is finished. This function again enable disabled activites after RAM check (DMA or DTC). \n

    \param      none\n

    \pre        Called only at operational time. \n

    \note       Overloading is not designed here, as this is up to an application. \n
*/
extern void _FSF_vDoWhenPieceRamCheckFinished(void);

/* Register channel functions */
/**
* The Register one-time check
*/
//  The wrapper function
extern void FSF_vRegister_OneTimeCheck(void); //lint !e526  "symbol is not defined", yes it is!
//  The real checker
extern void FSF_vRegistercheck(void);
extern void FSF_vRegOpercheck(void);

/* ROM Channel Global Functions */
/** Read data from ROM module.
    Default implementation for reading data from ROM module. \n

    \param      uint8_t *     ROM address where to read from\n
    \param      uint16_t      length of data to read \n\n

    \return     uint8_t *     address of data that have been read, or NULL.\n\n

    \pre        Called only at operational time. Called only for ROM modules ready for reading \n
    \post       If NULL is returned, data couldn't read as requiested. In that case the currect \n
                check cycle is supended and resumed again when dispatched the next time.\n\n

    \note       Overloading is not designed here,as this is up to an application. \n
*/
extern uint8_t *_FSF_pucReadROMData(uint8_t *pSrc, uint16_t ui16length);

/** Check, if the addressed ROM module is ready for reading.
    Default implementation for checking the ROM modules ready state. \n

    \param      uint8_t *     ROM address from where to read from\n
    \param      uint16_t      length of data about to read \n

    \return     bool        true, if the ROM module is ready for reading, false otherwise.\n\n

    \pre        Called only at operational time. Called each time BEFORE start of reading\n
    \post       If false is returned, the currect check cycle is supended and resumed again \n
                when dispatched the next time.\n\n

    \note       Overloading is not designed here, as this is up to an application. \n
*/
extern bool _FSF_bIsROMReady(uint8_t *pData, uint16_t ui16length);

/** Check, if ROM at once is possible to start.
    Default implementation for checking the ROM check ready state. \n

    \param      none

    \return     bool        true, if the ROM check may start, false otherwise.\n\n

    \pre        Called only at operational time. Called each time BEFORE start of check\n
    \post       If false is returned, the currect check cycle is supended and resumed again \n
                when dispatched the next time.\n\n

    \note       Overloading is not designed here, as this is up to an application. \n
*/
extern bool _FSF_bIsROMAtOnceAllowed(void);

/** Notify completion of the ROM check.
    This handler get called every time a full ROM check has completed. \n

    \param      none\n

    \pre        Called only at operational time. NOT CALLED IN CASE OF A CRC ERROR!\n

    \note       Overloading is not designed here, as this is up to an application. \n
*/
extern void _FSF_vNotifyROMCheckComplete(void);

/** Get type of ROM check.
    At run time, an application may define what type of ROM check has to be performed. \n

    \param      none\n
    \return     true, if the whole has to be checked at once, false otherwise.

    \pre        Called only at operational time. \n

    \note       Overloading is not designed here, as this is up to an application. \n
*/
extern bool _FSF_bIsROMAtOnce(void);

/** While checking the ROM at once, this interface decides if interrupts get maintained or not.
    This is mainly intended due to performance reasons. While checking the ROM without maintaining
    the interrupts, some overflow conditions may occurre. Otherwise, maintaining a huge number of
    interrupt sources is time consuming, hence it will lengthen the whole ROM check time. So, by
    this interface, the user has the ability to decide whether to maintain the interrupts or not.
    The ROM channel internally provide this interface and it returns true by default. This interface
    may be overloaded, if necessary. As an example: if interrupts need to be  maintained at least
    once every second, and checking the whole ROM space at once takes only half a second, in that
    case there is no need to maintain the interrupt sources while checking.\n

    \param      none\n
    \return     true, if interrupts have to maintain, false otherwise.

    \pre        Called only at operational time.\n

    \note       Overloading is not designed here, as this is up to an application. \n
*/
extern bool _FSF_bCheckINTduringROMCheck(void);


/* Stack Channel Global Functions */
/**
* The stack channel pre-condition.
* This precondition returns true, if the current stack location lies within its predefined
* area and the magic entries at the beginning and the end of the stack remain untouched.
* \param  none\n.
* \return bool\n
*/
extern bool FSF_bStackPrecondition(void);

/**
   Return the actual stack pointer
<pre>
   --------------------------------------------------------------------------
   This stack channel requires local variables to be placed on the hardware
   stack. For the time being, the so called "compiled stack" is not supported
   --------------------------------------------------------------------------
</pre>
    This observer returns the actual position of the stack pointer. Performing certain tasks
    in order to manipulate the stack pointer for test would result into more or less non-portable
    solutions, especially when testing the same piece of code and for different environments.
    Thus, for test harness purposes this value can be read from a reserved location.

    To be noticed: due to the preconditions above, only the address of an stack-located variable
    is returned. Its physical location will be some bytes deeper inside the stack, as it is placed
    within the new stack frame of the called observer. Re-alignment is not done, as this would
    involve some porting problems due to different stack frame structures.

    \return   actual location of the stack pointer
    \pre      Location and size of the stack provided correctly.
    \post     Magic numbers written correctly. Current stack pointer content checked correctly.
    \remark   Probably, some CPU architectures or compilers do not allow the default implementation
              below or do not provide direct access to the stack. For those, an asm implementation may
              solve the problem.
*/
#ifdef MULTIPLE_STACK
extern uint8_t * _FSF_pucGetStackPointer(uint8_t ucStackIndex);
#else   //#ifdef MULTIPLE_STACK
extern uint8_t * _FSF_pucGetStackPointer(void);
#endif  //#ifdef MULTIPLE_STACK

/* uint8_t   _FSF_ucGetStackCount(void); */
extern uint8_t _FSF_ucGetStackCount(void);


/* Interrupt Channel Global Functions */
/** Some little configuration helpers */
extern void FSF_vIsrEntryConf(uint8_t ucIsrId);
extern void FSF_vIsrExitConf(uint8_t ucIsrId);
extern void FSF_vIsrShortEntryConf(uint8_t ucIsrId);
extern void FSF_vIsrClearConf(uint8_t ucIsrId);
extern void FSF_vIsrClearShortConf(uint8_t ucIsrId);

/* Boot functions */
/* Continue startup on completion of the Functional Safety Boot process */
/**
* Default point where to continue start up.
*
* This depends on a specific application.
*/
extern void _Startup(void);

extern uint32_t  _FSF_ui32FinalizeCrcCalculation(uint32_t crc);

extern uint32_t uiCalculateCRC32(uint32_t CRC32Value,
    FSTH_CONST uint32_t *pData,
    uint16_t ui16length );

/**
   Reset CRC unit of microcontroller
<pre>
   --------------------------------------------------------------------------
   This function is required by ROM channel, when HW CRC unit is used
   --------------------------------------------------------------------------
</pre>
    \param    none\n
    \return   init CRC value\n
    \remark   Probably, some CPU architectures or compilers do not allow the default implementation
              below or do not provide HW CRC unit. For those, an asm implementation may  solve the
              problem. (using SW CRC)
*/
extern uint32_t _FSF_ui32ResetCrcCalculation(void);


/**
   StartUp ROM Crc Check

    \param    void\n

    \return   none\n

    \remark   If Crc is not equal to stored value -> go to fatal state
               (FSF_vReportFatal() function is called).

*/
extern void FSF_vStartUpRomCrcCheck(void);//lint !e526  "symbol is not defined", yes it is!

/**
   Set Safe State immediately after reset

    \param    none\n

    \return   none\n

    \remark   If Crc is not equal to stored value -> go to fatal state
               (FSF_vReportFatal() function is called).

*/
extern void FSF_vSetApplicationSafeState(void);


extern TRunTimeRamTestStatus FSF_tRunTimeRamCheckMarchCFast(uint32_t * FSF_pui32RamPiece, uint32_t * FSF_aui32RamBuf);
extern TRunTimeRamTestStatus FSF_tRunTimeRamCheckMarchC(uint32_t * FSF_pui32RamPiece, uint32_t * FSF_aui32RamBuf);
extern TRunTimeRamTestStatus FSF_tRunTimeRamBufferCheckMarchC(uint32_t * pui32RamBuf);

extern uint32_t FSF_u32GetNumberOfRamSegments(void);

#ifdef __cplusplus
}
#endif
/*lint -restore -e755 -e757 -e758 -e768 -e769 -e526 -e9045 */
#endif //#ifndef __FSAFEXS_H
