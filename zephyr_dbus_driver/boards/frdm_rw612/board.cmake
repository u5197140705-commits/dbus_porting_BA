# SPDX-License-Identifier: Apache-2.0

if(NOT BOARD_FRDM_RW612)
  return()
endif()

# Add the board's Kconfig fragment
kconfig_add_fragment(${CMAKE_CURRENT_LIST_DIR}/Kconfig.board)