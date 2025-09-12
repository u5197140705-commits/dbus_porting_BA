#*******************************************************************************
#  Copyright (c) 2017 BSH Hausgeraete GmbH,
#  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
#
#  All rights reserved. This program and the accompanying materials
#  are protected by international copyright laws.
#  Please contact copyright holder for licensing information.
#
#*******************************************************************************
#  PROJECT          IC DBus
#*******************************************************************************
#  Description      build include for dbus
#*******************************************************************************

#protection guard preventing multiple inclusion
ifndef dbus_build_mak
    dbus_build_mak := 1

    ifndef testClient

    code_gen_cfg += dbus/config.json

    # Selector of peripheral mapping for DBus:
    # - hal (HUART)
    # - mcal (MUART)
    # - dbuscan (DBusCAN chip)
    dbus_mapping ?= hal
    ifeq ($(dbus_mapping),hal)
        defines += DBM_HAL
    else ifeq ($(dbus_mapping),mcal)
        defines += DBM_MCAL
    else ifeq ($(dbus_mapping),dbuscan)
        defines += DBM_DBUSCAN
        dbus_platform = _$(dbus_mapping)
    else
        $(info Unknown DBus mapping selection!)
    endif

    # define the compilation units for the micro abstraction layer
    obj     += $(addprefix dbus/, bal dbusdll$(dbus_platform) dbuspresentation dbus_lock dbus_rtos_interface)

    # define the library configuration files which must be provided by application
    ifdef programmer
        obj += dbus/cfg_templates/balXs
    else
        cfg_obj += $(addprefix dbus/, balXs)
        obj     += $(addprefix dbus/, bal_breaks$(dbus_platform))
    endif
    cfg_obj += $(addprefix dbus/, dbusdll$(dbus_platform)Xs dbusmapping$(dbus_platform) dbuspresentationXs dbuspresentation_updateXS dbuspresentation_productionXS)
    ifneq ($(dbus_mapping),dbuscan)
        cfg_obj += $(addprefix dbus/, $(platform)/dbusmappingMc)
    endif
    cfg_hdr += $(addprefix dbus/, dbuspresentation_subsys0rxXS.h)

    ifeq ($(FW_update_type),FWU3)
        search_path += common/prog/firmware_update/hsi
        ifdef programmer
            defines += DBUS2_UPDATE_HSI
            defines += DBUS2_NO_BREAK_FUNCTIONALITY
        else
            defines += DBUS2_UPDATE
        endif
    endif

    ifneq (,$(filter $(FW_update_type), FWU1 FWU4))
        ifeq ($(fwu_production_data), true)
            defines += DBUS2_UPDATE
            defines += DBUS2_PROD_MSG_IN_APP
            ifeq ($(platform),efr32xg21)
                ifeq ($(fwu_appliance_data), true)
                    defines += DBUS2_APPLIANCE_MSG_IN_APP
                    dbus_transmit_buffer_size ?= 70
                    dbus_receive_buffer_size  ?= 80
                endif
            endif
        endif
    endif

    dbus_receive_buffer_size  ?= 50
    dbus_transmit_buffer_size ?= 50
    defs_so_far := $(defines)
    ifeq (, $(findstring __MESSAGE_INPUT_BUFFER_SIZE=, $(defs_so_far)))
        defines += __MESSAGE_INPUT_BUFFER_SIZE=$(dbus_receive_buffer_size)U
    endif
    defines += DLL_TRANSMIT_BUFFER_DATA_LENGTH=$(dbus_transmit_buffer_size)U

    defines += DBUS2_NODE_ID=Variant_$(variant)
    #defines+= ID=platform:$(platform) # There is currently conflict with "ID" define and "ID" element of versioning structure
    defines += __DBUS_NODE_ADDRESS=$(dbus_node_address) __TESTMSGSRV_SUBNODE_ADDRESS=$(TESTMSGSRV_SUBNODE_ADDRESS)
    defines += DBUS2_INCLUDED

    ifneq ($(dbus_mapping),dbuscan)
        defines += DBUS2_CONTROLLER_SPECIFIC_MAPPING
        defines += DBM_DERIVATIVE_SPECIFIC_ERROR_TYPE=uint16_t
        defines += DBUS_UART_CHANNEL=$(dbus_uart_channel)
        defines += DBUS_UART_CHANNEL_$(dbus_uart_channel)

        # dbus idle setting and definition
        ifeq ($(dbm_use_irq_for_idle_detection), true)
            defines += __DBM_USE_IRQ_FOR_IDLE_DETECTION
            ifneq ($(dbus_rx_pin),)
                dbus_idle_pin ?= $(dbus_rx_pin)
                dbus_idle_port = $(call match,$(dbus_idle_pin),(\w)\d)
                defines += DBUS_IDLE_PORT=$(dbus_idle_port)
                dbus_hint_channel ?= $(call match,$(dbus_idle_pin),\w([0-3]?\d?))
                defines += DBUS_HINT_CHANNEL=$(dbus_hint_channel)
            endif
        endif

        # bit time definition for default baudrate
        ifeq ($(strip $(dbus_default_baudrate)),96)
            defines += __BIT_TIME=104u
        else ifeq ($(strip $(dbus_default_baudrate)),192)
            defines += __BIT_TIME=52u
        else ifeq ($(strip $(dbus_default_baudrate)),384)
            defines += __BIT_TIME=26u
        else ifeq ($(strip $(dbus_default_baudrate)),576)
            defines += __BIT_TIME=26u
        else ifeq ($(strip $(dbus_default_baudrate)),1152)
            defines += __BIT_TIME=26u
        else ifeq ($(strip $(dbus_default_baudrate)),1250)
            defines += __BIT_TIME=26u
        else ifeq ($(strip $(dbus_default_baudrate)),2304)
            defines += __BIT_TIME=26u
        else ifeq ($(strip $(dbus_default_baudrate)),2500)
            defines += __BIT_TIME=26u
        else
            $(error Unsupported default baudrate $(strip $(dbus_default_baudrate)) for SW DBus. Supported options are 96,192,384,576,1152,1250,2304,2500)
        endif

        # dbus break pin setting and definition
        ifneq ($(dbus_tx_pin),)
        # As dbus_tx_pin is not set (properly) for some platforms, DBUS_BREAK_PIN is defined in a corresponding dbusmappingMc.c then
            ifeq (,$(filter $(platform), efr32xg21 psoc4 rx100 rx200))
                dbus_break_pin ?= $(strip $(dbus_tx_pin))
                defines += DBUS_BREAK_PIN=$(dbus_break_pin)
            endif
        endif
    else #ifneq ($(dbus_mapping),dbuscan)
        ifeq (,$(filter $(strip $(dbus_default_baudrate)), 96 192 384 576 1250 2500 5000 10000))
            $(error Unsupported default baudrate (strip $(dbus_default_baudrate)) for DBus with DBusCAN. Supported options are 96,192,384,576,1250,2500,5000,10000)
        endif
    endif #ifneq ($(dbus_mapping),dbuscan)

    doxy_image_path_ += dbus/doc/images
    endif

    #end of protection guard preventing multiple inclusion
endif
