/*******************************************************************************
 *  Copyright (c) 2017 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  VARIANT          RAMLOADER
 ******************************************************************************/

#ifndef RAMLOADER_H
#define RAMLOADER_H

#define CRC32_SIZE	UINT32_C(0x4)
#define STACK_MAGIC_SIZE	UINT32_C(0x4)
#define BUILT_IN_VARIANT
#define FWU_VARIANT
#define BOOTLOADER_VARIANT
#define UC_VENDOR	GD
#define PROC_FAMILY	GD32F30X
#define GD32_ARM
#define GD32F30
#define GD32F30X_HD
#define GD32F303
#define GD32F303C
#define GD32F303CB
#define RAM0START	0x20000000UL
#define RAM0TOP	0x200007FFUL
#define ROM1START	0x20000800UL
#define ROM1END	0x200021FFUL
#define FLASH_SIZE	128UL
#define I2C_BY_MCAL
#define MAL_ENABLE_OPT_FLASH_DRIVER
#define MAL_RELEASE_KEY_DEF	0x5E
#define MAL_DEVELOP_KEY_DEF	0xDE
#define MAL_ENABLE_DEF	0xEA
#define MAL_IGNORE_ERROR_DEF	0x1E
#define MAL_DISABLE_CALC_DEF	0xDC
#define MAL_RESERVED_DEF	0xFF
#define MAL_ENABLE_INT_FLASH_DRIVER
#define WATCHDOG_INCLUDED
#define MCAL_INCLUDED
#define MCAL_MDIO_INCLUDED
#define MCAL_MEXTI_INCLUDED
#define MCAL_MWDT_INCLUDED
#define MCAL_MSPI_INCLUDED
#define MCAL_MCOM_INCLUDED
#define MCAL_MSUP_INCLUDED
#define RAM1_PHY_START	UINT32_C(0x20000000)
#define RAM1_PHY_END	UINT32_C(0x20007FFF)
#define RAM1_PHY_SIZE	UINT32_C(0x8000)
#define ROM1_PHY_START	UINT32_C(0x8000000)
#define ROM1_PHY_END	UINT32_C(0x801FFFF)
#define ROM3_PHY_START	UINT32_C(0x1FFFF800)
#define ROM3_PHY_END	UINT32_C(0x1FFFF80F)
#define ROM3_PHY_SIZE	UINT32_C(0x10)
#define ROM1_START	UINT32_C(0x20000800)
#define ROM1_END	UINT32_C(0x200021FF)
#define ROM1_SIZE	UINT32_C(0x1A00)
#define ROM3_START	UINT32_C(0x1FFFF800)
#define ROM3_END	UINT32_C(0x1FFFF80F)
#define ROM3_SIZE	UINT32_C(0x10)
#define RAM1_START	UINT32_C(0x20000000)
#define RAM1_END	UINT32_C(0x200007FF)
#define RAM1_SIZE	UINT32_C(0x800)
#define PAGE_SIZE	UINT32_C(0x800)
#define HEAPSIZE	UINT32_C(0x0)
#define STACKSIZE	UINT32_C(0x400)
#define FWU_SHARED_DATA_SIZE	UINT32_C(0x40)
#define UNINIT_DATA_SIZE	UINT32_C(0x40)
#define RAMCONT_EXCLUDED_DATA_SIZE	UINT32_C(0x0)
#define CORE_CLOCK	120U
#define CORE_CLOCK_120M
#define PCLOCK1	60U
#define PCLOCK2	120U
#define EXT_CRYSTAL_OSC	8U
#define VTOR_ALIGN	UINT32_C(0x200)
#define VECTOR_TABLE_SIZE	UINT32_C(0x150)
#define FLASH_DRIVER_SIZE	UINT32_C(0x1B0)
#define NDEBUG
#define DEBUG	0x00U
#define NO_ISR_SUPPORT
#define NO_RAM_INIT
#define DBUS_DEFAULT_BAUDRATE	96U
#define DBUSCAN_INCLUDED
#define GD32F303CBT6
#define MIN_WRITEBLOCK_SIZE	UINT32_C(0x2)
#define REMOTE_FIRMWARE_UPDATE
#define CORTEX_M4
#define CORTEX_M4F
#define ALIGNMENT	4
#define __LITTLE_ENDIAN_ADAPTATION
#define ROOT_REGION	UINT32_C(0x8000000)
#define ARM_HW_FPU_PRESENT
#define TRASH_AREA_START	0x1000000
#define RELOC_ADDR	0x0
#define ANY_CONTINGENCY	0x50
#define UID	UINT64_C(0)
#define VERSION_MAJOR	UINT16_C(0)
#define VERSION_MINOR	UINT16_C(0)
#define VERSION_REVISION	UINT16_C(0)

#endif /* RAMLOADER_H */
