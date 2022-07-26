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
 *  PLATFORM          NXP MKV5X
 ******************************************************************************/

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/*
    This is a GCC scatter file template for platform NXP mkv5x.
    It will be processed by a C-Preprocessor before interpreted by the linker
 */



/* Linker script to configure memory regions. */
MEMORY
{
  ROM0 (rx) :    ORIGIN = ROOT_REGION, LENGTH = 0x10             // 4 vectors
  ROM1 (rx) :    ORIGIN = 0x10000010,  LENGTH = 0x3F0
  ROM_SYS (rx) : ORIGIN = 0x10000400,  LENGTH = 0x10
  ROM2 (rx) :    ORIGIN = ROM2_START,  LENGTH = ROM2_SIZE
  RAM1 (rwx) :   ORIGIN = RAM1_START,  LENGTH = RAM1_SIZE
#ifdef RAM2_START
  RAM2 (rwx) :   ORIGIN = RAM2_START, LENGTH = RAM2_SIZE
#endif

#ifdef FWU_FLEX_PARTITION_START
  FLEX_PARTITION (rx) : ORIGIN = FWU_FLEX_PARTITION_START, LENGTH = FWU_FLEX_PARTITION_SIZE
#endif

#ifdef FWU_OTP_MEMORY_START_ADDRESS
  OTP_MEMORY (rx) : ORIGIN = FWU_OTP_MEMORY_START_ADDRESS, LENGTH = FWU_OTP_MEMORY_SIZE
#endif

#ifdef FEATURE_FUNCTIONAL_SAFETY_ENABLED
  FS_EXCLUDED_RAM (rwx) : ORIGIN = RAM1_END +1 - RAMCONT_EXCLUDED_DATA_SIZE,  LENGTH = RAMCONT_EXCLUDED_DATA_SIZE
#endif
}

SECTIONS
{
#ifdef need_fixed_vectors
    .boot1 :
    {
        KEEP(*(.vectorsFixed))
    } > ROM0
#endif
    .system :
    {
        KEEP(*(.BACKDOOR_KEY))
        KEEP(*(.OPTION_BYTE))
    } > ROM_SYS

#ifdef REMOTE_FIRMWARE_UPDATE
    .boot2 :
    {
        KEEP(*(.FWU_DATA_ANCHOR))
        KEEP(*(.FWU_OPT_BYTES))
#if RELOC_ADDR == 0
// assert: just a small application like bootloader or BootManager will set RELOC_ADDR = 0
        KEEP(*(.vectorsCore))
        KEEP(*(.vectors))
// place this here (gcc linker cannot split code segment automatically)
        *hwdt01*(.text*)
        *hsupArmCM.o(.text*)
        *BootManager.o(.text*)
        *BootManagerModule.o(.text*)
        *BootManagerSharedData.o(.text*)
        *BootManagerSharedDataUc.o(.text*)
#endif
    } > ROM1
#endif


#ifdef FWU_FLEX_PARTITION_START
    .flexpartition :
    {
        KEEP(*(.FWU_FLEX_PARTITION_HEADER))
        KEEP(*(.FWU_FLEX_PARTITION))
    } > FLEX_PARTITION
#endif

#ifdef FWU_OTP_MEMORY_START_ADDRESS
    .fwu_otp_memory :
    {
        KEEP(*(.FWU_OTP_MEMORY))
    } > OTP_MEMORY
#endif

#if RELOC_ADDR != 0
    .text :
    {
#ifdef need_ModuleHeader
        KEEP(*(.FWU_SW_MODULE_HEADER))
        *ArmCM.o(.text*)
        *dbusmapping.o(.text*)
        . = VTOR_ALIGN;
#endif
        KEEP(*(.vectorsCore))
        KEEP(*(.vectors))
      #if defined(APP_VARIANT) || defined(COPY_FLASH_DRV_TO_RAM)
        *(EXCLUDE_FILE(*intflash_*.o) .text*)
      #else
        *(.text*)
      #endif
        KEEP(*(.init))
        KEEP(*(.fini))

        /* .ctors */
        *crtbegin.o(.ctors)
        *crtbegin?.o(.ctors)
        *(EXCLUDE_FILE(*crtend?.o *crtend.o) .ctors)
        *(SORT(.ctors.*))
        *(.ctors)

        /* .dtors */
        *crtbegin.o(.dtors)
        *crtbegin?.o(.dtors)
        *(EXCLUDE_FILE(*crtend?.o *crtend.o) .dtors)
        *(SORT(.dtors.*))
        *(.dtors)
      #if defined(APP_VARIANT) || defined(COPY_FLASH_DRV_TO_RAM)
        *(EXCLUDE_FILE(*intflash_*.o) .rodata*)
      #else
        *(.rodata*)
      #endif
        KEEP(*(.eh_frame*))
    } > ROM2
#else
    .text :
    {
        *(.text*)

        KEEP(*(.init))
        KEEP(*(.fini))
        /* .ctors */
        *crtbegin.o(.ctors)
        *crtbegin?.o(.ctors)
        *(EXCLUDE_FILE(*crtend?.o *crtend.o) .ctors)
        *(SORT(.ctors.*))
        *(.ctors)

        /* .dtors */
        *crtbegin.o(.dtors)
        *crtbegin?.o(.dtors)
        *(EXCLUDE_FILE(*crtend?.o *crtend.o) .dtors)
        *(SORT(.dtors.*))
        *(.dtors)
        *(.rodata*)
        KEEP(*(.eh_frame*))
    } > ROM2
#endif

    .ARM.extab :
    {
        *(.ARM.extab* .gnu.linkonce.armextab.*)
    } > ROM2

    __exidx_start = .;
    .ARM.exidx :
    {
        *(.ARM.exidx* .gnu.linkonce.armexidx.*)
    } > ROM2

    __exidx_end = .;
    __etext = .;

    .no_init (NOLOAD) :
    {
        *(.FWU_SHARED_DATA)
        . = FWU_SHARED_DATA_SIZE;
#if defined(VARIANT_BOOTMANAGER) || defined(APP_VARIANT)
        __uninit_data_start = .;
        KEEP(*(.UNINIT_DATA))
        . = __uninit_data_start + UNINIT_DATA_SIZE;
#endif
        // Include all timer segments
        #include "timer/timer_seg.h"
    } > RAM1


    .data :
    {
        __data_start__ = .;
        *(.data*)

        . = ALIGN(4);
        /* preinit data */
        PROVIDE_HIDDEN (__preinit_array_start = .);
        KEEP(*(.preinit_array))
        PROVIDE_HIDDEN (__preinit_array_end = .);

        . = ALIGN(4);
        /* init data */
        PROVIDE_HIDDEN (__init_array_start = .);
        KEEP(*(SORT(.init_array.*)))
        KEEP(*(.init_array))
        PROVIDE_HIDDEN (__init_array_end = .);


        . = ALIGN(4);
        /* finit data */
        PROVIDE_HIDDEN (__fini_array_start = .);
        KEEP(*(SORT(.fini_array.*)))
        KEEP(*(.fini_array))
        PROVIDE_HIDDEN (__fini_array_end = .);
        . = ALIGN(4);
    #if defined(APP_VARIANT) || defined(COPY_FLASH_DRV_TO_RAM)
        // Flash routines to be loaded into RAM
        *(.text.DRVIF_*)
        *(.rodata.DRVIF_*)
        *(.text.ABSIF_*)
        . = ALIGN(4);
    #endif
        /* All data end */
        __data_end__ = .;

    } > RAM1 AT>ROM2

    .bss :
    {
        __bss_start__ = .;
        *(.bss*)
        *(COMMON)
        __bss_end__ = .;
    } > RAM1

    .heap :
    {
        __end__ = .;
        end = __end__;
        *(.heap*)
        __HeapLimit = .;
    } > RAM1

    /* .stack_dummy section doesn't contains any symbols. It is only
     * used for linker to calculate size of stack sections, and assign
     * values to stack symbols later */
    .stack_dummy :
    {
        *(.stack)
    } > RAM1

    /* Set stack top to end of RAM1, and stack limit move down by
     * size of stack_dummy section */
#ifndef FEATURE_FUNCTIONAL_SAFETY_ENABLED
    __StackTop = ORIGIN(RAM1) + LENGTH(RAM1) - STACK_MAGIC_SIZE;
#else
    __StackTop = ORIGIN(RAM1) + LENGTH(RAM1) - STACK_MAGIC_SIZE - RAMCONT_EXCLUDED_DATA_SIZE;
#endif
    __StackLimit = __StackTop - SIZEOF(.stack_dummy);
    PROVIDE(__stack = __StackTop);

    /* Excluded region -  excluded data from RAM Continuous (RunTime) check */
#ifdef FEATURE_FUNCTIONAL_SAFETY_ENABLED
    .fs_excluded_ram :
    {
        KEEP(*(.RAMCONT_EXCLUDED_DATA))
    } > FS_EXCLUDED_RAM
#endif

    /* Check if data + heap + stack exceeds RAM1 limit */
    ASSERT(__StackLimit >= __HeapLimit, "region RAM1 overflowed with stack")
}

