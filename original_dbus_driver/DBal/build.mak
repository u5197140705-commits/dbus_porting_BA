#*******************************************************************************
#  Copyright (c) 2017 BSH Hausgeraete GmbH,
#  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
#
#  All rights reserved. This program and the accompanying materials
#  are protected by international copyright laws.
#  Please contact copyright holder for licensing information.
#
#*******************************************************************************
#  PROJECT          DBus Application layer
#*******************************************************************************
#  Description      build include for dbus application layer (DBal)
#                   DBus application layer is a counterpart of IO driver that
#                   is implemented in SyMaNa. 
#*******************************************************************************

#protection guard preventing multiple inclusion
ifndef dbal_build_mak
    dbal_build_mak := 1

    defines += DBAL_INCLUDED

    search_path += common/prog/dbus/DBal
    search_path += $(cfg_prog_path)/dbus/DBal
    obj += dbus/DBal/BshDBus2AppLayer
    obj += dbus/DBal/BshDBus2AppLayer_instances
    obj += dbus/DBal/IoConnectionHandling
    obj += dbus/DBal/BshDBus2AppLayer_crossQueue
    obj += dbus/DBal/BshDBus2AppLayer_debug
    cfg_hdr += $(addprefix dbus/DBal/, DBal_cfg.h)
    cfg_src += $(addprefix dbus/DBal/, DBal_cfg.c)

    #end of protection guard preventing multiple inclusion
endif
