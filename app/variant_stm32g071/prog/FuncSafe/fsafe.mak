#******************************************************************************
#   Copyright (c) 2023 BSH Hausgeraete GmbH,
#   Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
# 
#   All rights reserved. This program and the accompanying materials
#   are protected by international copyright laws.
#   Please contact copyright holder for licensing information.
# 
# *****************************************************************************
# *  PROJECT          Functional Safety Settings                              *
# *****************************************************************************
#
#
#******************************************************************************
#*  DOCUMENTATION                                                             *
#******************************************************************************
# This make file is intended to cut all Functional Safety settings
# and let user to adjust it according to application specific needs.
#
# !!! Attention !!! 
#
# When some settings are changed in this make file, new project header file has
# to be generated. It means that project header has to be deleted, clean is
# also necessary and then run make target for variant.
#
# *****************************************************************************
# *  FUNCIONAL SAFETY EXAMPLE SETTINGS                                        *
# *****************************************************************************
#                               *********************
#                               *        RAM        *
#                               *********************
#                                _ _ _ _ _ _ _ _ _ _
#                     RAM_START |                   |
#                               |  FWU SHARED DATA  | FWU_SHARED_DATA_SIZE (0x40)
#              RAM_START + 0x40 |_ _ _ _ _ _ _ _ _ _|
#                               |                   |
#                               |    UNINIT DATA    | UNINIT_DATA_SIZE (0x40)
#              RAM_START + 0x80 |_ _ _ _ _ _ _ _ _ _|
#                               |                   |
#                               .                   .
#                               .                   .
#                               |_ _ _ _ _ _ _ _ _ _|
#                    STACK_BASE |FAFE               |
#                               |                   |
#                               |       STACK       | STACKSIZE (0x400)
#                               |                   | 
#                   STACK_LIMIT |_ _ _ _ _ _ _ _FAFE|
#                               | + - - - - - - - + |
#                               | |MarchDataBuffer| | MARCHBUFLENGTH (0x20)
#                               | + - - - - - - - + |
#                               |      RAMCONT      | RAMCONT_EXCLUDED_DATA_SIZE (0x40)
#                               |   EXCLUDED DATA   |
#                       RAM_END |_ _ _ _ _ _ _ _ _ _|
#
#
#
#                               *********************
#                               *      ROM FWU1     *
#                               *********************
#                                _ _ _ _ _ _ _ _ _ _ 
#   FWU_ROMLOADER_START_ADDRESS |                   | FS_BL_START_ADR
#                               |                   |
#                               |     ROMLOADER     | FWU_ROMLOADER_SIZE
#                               |                   |
#     FWU_ROMLOADER_END_ADDRESS |_ _ _ _ _ _ _ CRC32| FS_BL_END_ADR/FS_BL_CRC_ADR
# FWU_APPLICATION_START_ADDRESS |                   | FS_APP_START_ADR
#                               |                   |
#                               |                   |
#                               |        APP        | FWU_APPLICATION_SIZE
#                               |                   |
#                               |                   |
#   FWU_APPLICATION_END_ADDRESS |_ _ _ _ _ _ _ CRC32| FS_APP_END_ADR/FS_APP_CRC_ADR
#
#
#
#                               *********************
#                               *      ROM FWU3     *
#                               *********************
#                                _ _ _ _ _ _ _ _ _ _ 
# FWU_BOOTMANAGER_START_ADDRESS |                   | FS_BL_START_ADR
#                               |                   |
#                               |    BOOT MANAGER   | FWU_BOOTMANAGER_SIZE
#                               |                   |
#   FWU_BOOTMANAGER_END_ADDRESS |_ _ _ _ _ _ _ CRC32| FS_BL_END_ADR/FS_BL_CRC_ADR
#         FWU_LRO_START_ADDRESS |CRC32              |
#                               |                   |
#                               |     LROMLOADER    | FWU_LRO_SIZE
#                               |                   |
#           FWU_LRO_END_ADDRESS |_ _ _ _ _ _ _ _ _ _|
#  FWU_PROGRAMMER_START_ADDRESS |CRC32              |
#                               |                   |
#                               |     PROGRAMER     | FWU_PROGRAMMER_SIZE
#                               |                   |
#    FWU_PROGRAMMER_END_ADDRESS |_ _ _ _ _ _ _ _ _ _|
#  FWU_OTP_MEMORY_START_ADDRESS |                   |
#                               |     OTP_MEMORY    | FWU_OTP_MEMORY_SIZE
#    FWU_OTP_MEMORY_END_ADDRESS |_ _ _ _ _ _ _ _ _ _|
#        FWU_APP1_START_ADDRESS |CRC32              | FS_APP_CRC_ADR/FS_APP_START_ADR
#                               |                   |
#                               |                   |
#                               |        APP        | FWU_APP1_SIZE
#                               |                   |
#                               |                   |
#          FWU_APP1_END_ADDRESS |_ _ _ _ _ _ _ _ _ _| FS_APP_END_ADR
#
#
# *****************************************************************
# *              Definition for FS General setting                *
# *****************************************************************

is_testharness     ?= DISABLED

#While Test harness testing all parts must be variable not constants
ifeq ($(is_testharness), DISABLED)
    defines += FSTH_CONST=const
    
    is_reg_channel     ?= ENABLED
    is_rom_channel     ?= ENABLED
    is_ram_channel     ?= ENABLED
    is_stack_channel   ?= ENABLED
    is_isr_channel     ?= ENABLED
    is_ram_con_channel ?= ENABLED
    is_pec             ?= DISABLED
else
    defines += TH_FSCOM
    defines += FSTH_CONST
    dep_component += ped_fw/testengine

    # Needed defines for Test harness    
    RAMCON_TYPE = NONE
    MARCHBUFLENGTH = 20
    defines += MARCHBUFLENGTH=0x$(MARCHBUFLENGTH)   
    defines += FSF_RAM_4_BYTES_AT_ONCE
    #defines += FSF_CYCLE_TIM_SIZE_32BIT
    defines += FSF_CRC_32_BIT
    defines += OPERATIONAL_RAM_CHECK

    is_reg_channel     ?= DISABLED
    is_rom_channel     ?= DISABLED
    is_ram_channel     ?= DISABLED
    is_stack_channel   ?= DISABLED
    is_isr_channel     ?= DISABLED
    is_ram_con_channel ?= DISABLED
    is_pec             ?= DISABLED
endif

# *****************************************************************
# *                       ROM CHECK SECTION                       *
# *****************************************************************
ifeq ($(is_rom_channel), ENABLED)

    defines += FUNCTIONAL_SAFETY_ROM_CHECK_ACTIVE   
    defines += FSF_CRC_32_BIT   

    #********************* Patchbin definitions ***********************
    # required definition for StartUp ROM check

    # only FWU1 needs CRC for FS application 
    # For FWU3 CRC is stored in application header 
    ifeq ($(FW_update_type), FWU1)
        ifdef app_variant
            # romLoader
            FS_BL_START_ADR  ?= $(FWU_ROMLOADER_START_ADDRESS)
            FS_BL_END_ADR    ?= $(call calcx,(0x$(FWU_ROMLOADER_END_ADDRESS) - 0x$(crc32_size)))
            FS_BL_CRC_ADR    ?= $(call calcx,(0x$(FWU_ROMLOADER_END_ADDRESS) - 0x$(crc32_size) + 1))
            defines += FS_BL_START_ADR=UINT32_C(0x$(FS_BL_START_ADR))
            defines += FS_BL_END_ADR=UINT32_C(0x$(FS_BL_END_ADR))
            defines += FS_BL_CRC_ADR=UINT32_C(0x$(FS_BL_CRC_ADR))

            patchbin_crc_app += -crc32(0x$(FS_BL_START_ADR),0x$(FS_BL_END_ADR))(0x$(FS_BL_CRC_ADR)):0x4C11DB7,0xFFFFFFFF

            # application
            FS_APP_START_ADR ?= $(FWU_APPLICATION_START_ADDRESS)
            FS_APP_END_ADR   ?= $(call calcx,(0x$(FWU_APPLICATION_END_ADDRESS) - 0x$(crc32_size)))
            FS_APP_CRC_ADR   ?= $(call calcx,(0x$(FWU_APPLICATION_END_ADDRESS) - 0x$(crc32_size) + 1))
            defines += FS_APP_START_ADR=UINT32_C(0x$(FS_APP_START_ADR))
            defines += FS_APP_END_ADR=UINT32_C(0x$(FS_APP_END_ADR))
            defines += FS_APP_CRC_ADR=UINT32_C(0x$(FS_APP_CRC_ADR))

            patchbin_crc_app += -crc32(0x$(FS_APP_START_ADR),0x$(FS_APP_END_ADR))(0x$(FS_APP_CRC_ADR)):0x4C11DB7,0xFFFFFFFF

        endif
    endif 
    ifeq ($(FW_update_type), FWU3)
        # BootManager doesn't have Module Header. CRC is placed at the end of
        # BootManager memory area. Application CRC is placed in its Module Header.

        # bootManager
        FS_BL_START_ADR  ?= $(FWU_BOOTMANAGER_START_ADDRESS)
        FS_BL_END_ADR    ?= $(call calcx,(0x$(FWU_BOOTMANAGER_END_ADDRESS) - 0x$(crc32_size)))
        FS_BL_CRC_ADR    ?= $(call calcx,(0x$(FWU_BOOTMANAGER_END_ADDRESS) - 0x$(crc32_size) + 1))
        defines += FS_BL_START_ADR=UINT32_C(0x$(FS_BL_START_ADR))
        defines += FS_BL_END_ADR=UINT32_C(0x$(FS_BL_END_ADR))
        defines += FS_BL_CRC_ADR=UINT32_C(0x$(FS_BL_CRC_ADR))

        patchbin_crc_app += -crc32(0x$(FS_BL_START_ADR),0x$(FS_BL_END_ADR))(0x$(FS_BL_CRC_ADR)):0x4C11DB7,0xFFFFFFFF

        # application
        FS_APP_START_ADR ?= $(call calcx,(0x$(FWU_APP1_START_ADDRESS) + 0x$(crc32_size)))
        FS_APP_END_ADR   ?= $(FWU_APP1_END_ADDRESS)
        FS_APP_CRC_ADR   ?= $(FWU_APP1_START_ADDRESS)
        defines += FS_APP_START_ADR=UINT32_C(0x$(FS_APP_START_ADR))
        defines += FS_APP_END_ADR=UINT32_C(0x$(FS_APP_END_ADR))
        defines += FS_APP_CRC_ADR=UINT32_C(0x$(FS_APP_CRC_ADR))
        # CRC of app is calculated in patchbin.mak
    endif
endif

# *****************************************************************
# *               OPERATIONAL RAM CHECK SECTION                   *
# *****************************************************************
RAM_BLOCKSIZE = 0x00000006       # block size, checked at one call
                                 # 6*4 = 24 bytes per one function 
                                 # call (possible value 2..6)
defines += RAM_BLOCKSIZE=$(RAM_BLOCKSIZE)

ifeq ($(is_ram_channel), ENABLED)
    MARCHBUFLENGTH = 20

    defines += FUNCTIONAL_SAFETY_RAM_CHECK_ACTIVE
    defines += FSF_RAM_4_BYTES_AT_ONCE   # 4 bytes are written at once
    defines += OPERATIONAL_RAM_CHECK
    defines += MARCHBUFLENGTH=0x$(MARCHBUFLENGTH)
endif

# *****************************************************************
# *                     STACK CHECK SECTION                       *
# *****************************************************************
ifeq ($(is_stack_channel), ENABLED)
    defines += FUNCTIONAL_SAFETY_STACK_CHECK_ACTIVE
endif

# *****************************************************************
# *                     ISR CHANNEL SECTION                       *
# *****************************************************************
ifeq ($(is_isr_channel), ENABLED)
    defines += FUNCTIONAL_SAFETY_ISR_CHECK_ACTIVE
endif

# *****************************************************************
# *                REGISTER CHANNEL SECTION                       *
# *****************************************************************
ifeq ($(is_reg_channel),ENABLED)
    defines += FUNCTIONAL_SAFETY_REGISTER_CHECK_ACTIVE
endif

# *****************************************************************
# *               RAM CONSISTENCY CHECK SECTION                   *
# *****************************************************************
ifeq ($(is_ram_con_channel),ENABLED)
    RAMCON_TYPE = FSF_RAM_CON_BY_MIRROR   #FSF_RAM_CON_BY_CHECK_VALUE
    defines +=  FUNCTIONAL_SAFETY_RAMCON_CHECK_ACTIVE
    defines +=  $(RAMCON_TYPE)
endif

test_fs_settings:
	@echo derivative                = $(derivative)
	@echo flash_size                = $(FLASH_SIZE)
	@echo FS_BL_START_ADR           = $(FS_BL_START_ADR)
	@echo FS_BL_END_ADR             = $(FS_BL_END_ADR)
	@echo FS_BL_CRC_ADR             = $(FS_BL_CRC_ADR)
	@echo FS_APP_START_ADR          = $(FS_APP_START_ADR)
	@echo FS_APP_END_ADR            = $(FS_APP_END_ADR)
	@echo FS_APP_CRC_ADR            = $(FS_APP_CRC_ADR)
	@echo feature_functional_safety = $(feature_functional_safety)
	@echo is_testharness            = $(is_testharness)
	@echo is_reg_channel            = $(is_reg_channel)
	@echo is_rom_channel            = $(is_rom_channel)
	@echo is_ram_channel            = $(is_ram_channel)
	@echo is_stack_channel          = $(is_stack_channel)
	@echo is_isr_channel            = $(is_isr_channel)
	@echo is_ram_con_channel        = $(is_ram_con_channel)
ifneq ($(is_ram_con_channel),)	
	@echo ram_consistency_type      = $(RAMCON_TYPE)
endif
	@echo is_pec                    = $(is_pec)


