/*******************************************************************************
 *  Copyright (c) 2022 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT           Generic SW
 *  PLATFORM          STM32H7
 ******************************************************************************/




/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/*
    This is a Keil-Arm scatter file template for platform stm32h7.
    It will be processed by a C-Preprocessor before interpreted by the linker
 */

#ifdef need_fixed_vectors
LR_BOOT ROOT_REGION
{
    ER_FIXED_VECTORS ROOT_REGION
    {
        *(vectorsFixed)
    }
}
#endif

// Special regions for firmware update
#ifdef REMOTE_FIRMWARE_UPDATE
LR_FWU0 ROOT_REGION +0x10
{
    ER_FWU_DATA_ANCHOR +0
    {
        *(FWU_DATA_ANCHOR, +FIRST)
        *(FWU_OPT_BYTES)
    }
}
#endif

#ifdef FWU_OTP_MEMORY_START_ADDRESS
LR_OTP FWU_OTP_MEMORY_START_ADDRESS FWU_OTP_MEMORY_SIZE
{
    ER_OTP FWU_OTP_MEMORY_START_ADDRESS FWU_OTP_MEMORY_SIZE
    {
        *(FWU_OTP_MEMORY)
    }
}
#endif

#ifdef FWU_FLEX_PARTITION_USED
// separated Flex-Partition table
LR_FWU1 FWU_FLEX_PARTITION_START FWU_FLEX_PARTITION_SIZE
{
    ER_FWU_FLEX_PARTITION +0 FWU_FLEX_PARTITION_SIZE
    {
        *(FWU_FLEX_PARTITION_HEADER, +FIRST)
        *(FWU_FLEX_PARTITION)
    }
}
#endif


#ifdef need_ModuleHeader
LR_FWU_SW_MODULE_HEADER ROM1_START VTOR_ALIGN
{
    ER_FWU_SW_MODULE_HEADER +0 VTOR_ALIGN-ANY_CONTINGENCY
    {
        *(FWU_SW_MODULE_HEADER, +FIRST)
        .ANY2 (+RO)
    }
}
#endif


#ifdef need_ModuleHeader
LR_ROM ROM1_START + VTOR_ALIGN  ROM1_SIZE - VTOR_ALIGN
#else
LR_ROM ROM1_START ROM1_SIZE
#endif
{
    ER_VECTORS +0 VECTOR_TABLE_SIZE
    {
        *(vectorsCore, +FIRST)
        *(vectors)
    }
#ifdef need_ModuleHeader
    ER_ROM +0  ROM1_SIZE - VTOR_ALIGN - VECTOR_TABLE_SIZE
#else
    ER_ROM +0  ROM1_SIZE - VECTOR_TABLE_SIZE
#endif
    {
        *(0x70000001)  ; SHT_ARM_EXIDX sections // needed for C++
        *(.init_array)                          // needed for C++
        .ANY1 (+RO)
    }
#if defined(APP_VARIANT) || defined(COPY_FLASH_DRV_TO_RAM)
    // Flash driver and routines used by flash driver placed before stack at end of RAM 
    ER_ROM_FLDRV RAM1_END + 1 - STACKSIZE - FLASH_DRIVER_SIZE - RAMCONT_EXCLUDED_DATA_SIZE FLASH_DRIVER_SIZE
    {
        *intflash_*.o (+RO)
    #if defined(COPY_FLASH_DRV_TO_RAM)
        *mem_drv.o (+RO)
        *hwdt01.o (+RO)
    #endif
    }
#endif
}



#ifdef NO_RAM_INIT
LR_RAM TRASH_AREA_START // put the initializer to trash
#else
LR_RAM +0
#endif
{
    ER_FWU_SHARED_DATA (RAM1_START) UNINIT
    {
        *(FWU_SHARED_DATA)
    }
#if defined(APP_VARIANT)
    ER_UNINIT_DATA (RAM1_START + FWU_SHARED_DATA_SIZE) UNINIT
    {
        *(UNINIT_DATA)	
    }   
    ER_FLASH_ROUTINES (RAM1_START + FWU_SHARED_DATA_SIZE + UNINIT_DATA_SIZE)
#else
    ER_FLASH_ROUTINES (RAM1_START + FWU_SHARED_DATA_SIZE)
#endif
    {
        *(FLASH_ROUTINES)
    }
    ER_RW +0
    {
        *(+RW)
    }
    ER_ZI +0
    {
        *(+ZI)
    }

    // Include all timer segments
    #include "timer/timer_seg.h"

    ARM_LIB_HEAP +0  EMPTY HEAPSIZE
    {
    }
}



#ifdef FEATURE_FUNCTIONAL_SAFETY_ENABLED
/* Excluded region -  excluded data from RAM Continuous (RunTime) check */
LR_RAMCONT_EXCLUDED_DATA +0 {
    ER_RAMCONT_EXCLUDED_DATA RAM1_END + 1 - RAMCONT_EXCLUDED_DATA_SIZE
    {
        *(RAMCONT_EXCLUDED_DATA)
    }
}
#endif

// Stack Region
LR_STACK RAM1_END + 1 - (STACKSIZE + RAMCONT_EXCLUDED_DATA_SIZE) {
    ARM_LIB_STACK RAM1_END + 1 - STACK_MAGIC_SIZE - RAMCONT_EXCLUDED_DATA_SIZE EMPTY - (STACKSIZE - STACK_MAGIC_SIZE)
    {
    }
}


// Throw link exception if LR_RAM overlaps the stack
ScatterAssert(RAM1_SIZE > STACKSIZE + ImageLength(LR_RAM))

// Throw link exception if all ROM-LRs exceed available ROM size
#ifdef need_ModuleHeader
ScatterAssert(ROM1_SIZE-VTOR_ALIGN > ImageLength(LR_ROM)
#else
ScatterAssert(ROM1_SIZE            > ImageLength(LR_ROM)
#endif
#if !defined(NO_RAM_INIT)
  + LoadLength(LR_RAM)                    // consider RAM1 initializers
#endif
)
