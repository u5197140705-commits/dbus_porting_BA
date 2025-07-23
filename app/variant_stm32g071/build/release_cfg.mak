#*******************************************************************************
#  Copyright (c) 2017 BSH Hausgeraete GmbH,
#  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
#
#  All rights reserved. This program and the accompanying materials
#  are protected by international copyright laws.
#  Please contact copyright holder for licensing information.
#
#*******************************************************************************
#  Description      Application variant release definitions
#*******************************************************************************

# If you have updatable partitions you need to make sure that the release target
# creates an according binary file in release folder. The following definitions show

# how to cut the binaries out of application file


ifeq ($(variant),MyVariant)

ifdef EXAMPLE_PARTITION
#Example: (creates a file "process_data" in release folder)
bin_gen += process_data
release_dep += $(release_path)/process_data.mot
process_data_start = 0x800C000
process_data_end   = 0x800CFFF
process_data_output_file_bin = $(release_path)/process_data
process_data_output_file     = $(release_path)/process_data.mot
release_file_list_non_copy  += $(process_data_output_file_bin)

process_data_output_range    = $(process_data_start),$(process_data_end)
#patching the CRC in the module header
patchbin_crc_app += -CRC($(process_data_start)+4,$(process_data_end))($(process_data_start)):$(crctype)
endif

#general syntax:
# bin_gen += <my_section>
# release_dep += $(release_path)/<my_section>.mot
# <my_section>_start = 0x12345678
# <my_section>_end   = 0x87654321
# <my_section>_output_file_bin = $(release_path)/<my_section>
# <my_section>_output_file     = $(release_path)/<my_section>.mot
# release_file_list_non_copy  += $(<my_section>_output_file)

# <my_section>_output_range    = $(<my_section>_start),$(<my_section>_end)
# #patching the CRC in the module header
# patchbin_crc_app += -CRC($(<my_section>_start)+4,$(<my_section>_end))($(<my_section>_start)):$(crctype)

endif
