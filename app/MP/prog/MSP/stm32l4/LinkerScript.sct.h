/*******************************************************************************
 *  Copyright (c) 2017 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT           Generic SW
 *  PLATFORM          STM32L4
 ******************************************************************************/




/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/*
    This is a Keil-Arm scatter file template for platform stm32l4.
    It will be processed by a C-Preprocessor before interpreted by the linker
 */

#ifdef SIGNATURE_STRUCT_SIZE
// Signature Region
LR_FWU_SIGNATURE ROM1_END + 1 - SIGNATURE_STRUCT_SIZE
{
    ER_FWU_SIGNATURE ROM1_END + 1 - SIGNATURE_STRUCT_SIZE
    {
        *(FWU_SIGNATURE)
    }
}
#endif


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
LR_FWU0 ROOT_REGION +0x10
{
    ER_FWU_DATA_ANCHOR +0
    {
        *(FWU_DATA_ANCHOR)
    }
}

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
LR_ROM ROM1_START+VTOR_ALIGN ROM1_SIZE-VTOR_ALIGN
#else
LR_ROM ROM1_START ROM1_SIZE
#endif
{
    ER_VECTOTS +0 VECTOR_TABLE_SIZE
    {
        *(vectorsCore, +FIRST)
        *(vectors)
    }
#ifdef need_ModuleHeader
    ER_ROM +0 ROM1_SIZE-VTOR_ALIGN-VECTOR_TABLE_SIZE
#else
    ER_ROM +0 ROM1_SIZE-VECTOR_TABLE_SIZE
#endif
    {
        .ANY1 (+RO)
    }
#ifdef APP_VARIANT
    // region for flash routines, if used in application
    ER_ROM_FLDRV RAM1_END + 1 - STACKSIZE - FLASH_DRIVER_SIZE FLASH_DRIVER_SIZE
    {
        *intflash_*.o (+RO)
    }
#endif
}



#ifdef NO_RAM_INIT
LR_RAM TRASH_AREA_START // put the initializer to trash
#else
LR_RAM +0
#endif
{
    ER_FWU_SHARED_DATA RAM1_START UNINIT
    {
       * (FWU_SHARED_DATA)
    }
    ER_BL_FAST_BOOT +0 UNINIT
    {
       * (BL_FAST_BOOT)
    }
    ER_FLASH_ROUTINES +0
    {
        *(FLASH_ROUTINES)
    }
    ER_RW +0
    {
        * (+RW)
    }
    ER_ZI +0
    {
        * (+ZI)
    }

    // Include all timer segments
    #include "timer/timer_seg.h"

    ARM_LIB_HEAP +0  EMPTY HEAPSIZE
    {
    }
}


// Stack Region
LR_STACK RAM1_END + 1 - STACKSIZE
{
    ARM_LIB_STACK RAM1_END + 1 EMPTY - STACKSIZE
    {
    }
}

// Throw link exception if LR_RAM overlaps the stack
// Note: In exception message  '>' used in stead of '<' (linker issue)
ScatterAssert(RAM1_END - RAM1_START + 1 > STACKSIZE + ImageLength(LR_RAM))



// Regions for Functional safety --> TODO final memory mapping not yet fixed for STM32L4
#ifdef FSRAMSTART
LR_FS1 +0
{
    ER_FSAFEGENERIC FSRAMSTART 0x20
    {
        *(FSAFE_GENERIC)
    }
    ER_FSAFEMIRROR FSMIRRORRAMSTART 0x20
    {
        *(FSAFE_MIRROR)
    }
}

LR_FS2 +0
{
    ER_FSAFE_OT_RAMCHECK FSRAMCHECKSTART
    {
        *(FSAFE_OT_RAMCHECK)
    }
}
#endif

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

