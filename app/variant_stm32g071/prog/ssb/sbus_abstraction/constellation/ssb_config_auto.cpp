/*******************************************************************************
 *  Copyright (c) 2022 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          Smart Sensor Bus
 *  COMP_ABBREV      SSB_CFG
 ******************************************************************************/

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file     ssb_config_auto.cpp
 *
 *  \ingroup  sbus_abstraction/constellation
 *
 *  \brief    Class containing the configurations done by the SSB-Configurator
 *
 *  \details
 */

/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/

#include "constellation/ssb_config_auto.h"

/******************************************************************************/
/* INTERNAL C-PREPROCESSOR DEFINITIONS                                        */
/******************************************************************************/

#if SSBF_MNGR_NUMBER_OF_HUBS >= 1U
    #define SSBAL_DURATION_LOOP_PERIOD_PER_TIMER_IN_MS_HUB_0  (STIM_TIME_MS(static_cast<uint32_t>(1000U)))
    #define SSBF_MNGR_CLIENT_PLACEMENT_MASK_HUB_0             (static_cast<uint8_t>(0x0F))
                                      // 0x00,      0x01,     0x02, 0x04, 0x08
                                      // No Client, Client 0, ...         Client 3
#endif
#if SSBF_MNGR_NUMBER_OF_HUBS >= 2U
    #define SSBAL_DURATION_LOOP_PERIOD_PER_TIMER_IN_MS_HUB_1  (STIM_TIME_MS(static_cast<uint32_t>(1000U)))
    #define SSBF_MNGR_CLIENT_PLACEMENT_MASK_HUB_1             (static_cast<uint8_t>(0x00))
#endif
#if SSBF_MNGR_NUMBER_OF_HUBS >= 3U
    #define SSBAL_DURATION_LOOP_PERIOD_PER_TIMER_IN_MS_HUB_2  (STIM_TIME_MS(static_cast<uint32_t>(1000U)))
    #define SSBF_MNGR_CLIENT_PLACEMENT_MASK_HUB_2             (static_cast<uint8_t>(0x00))
#endif
#if SSBF_MNGR_NUMBER_OF_HUBS == 4U
    #define SSBAL_DURATION_LOOP_PERIOD_PER_TIMER_IN_MS_HUB_3  (STIM_TIME_MS(static_cast<uint32_t>(1000U)))
    #define SSBF_MNGR_CLIENT_PLACEMENT_MASK_HUB_3             (static_cast<uint8_t>(0x00))
#endif
#if SSBF_MNGR_NUMBER_OF_HUBS > 4U
    #error SSBF_MNGR_NUMBER_OF_HUBS too big
#endif
#ifndef SSBF_MNGR_NUMBER_OF_HUBS
    #error SSBF_MNGR_NUMBER_OF_HUBS not defined
#endif

using namespace ::SSBAL::SSBCO;

#if defined SSBCFG_IMUT_ST_ST_ST_ST_PUMU0

const uint8_t
SsbConfigurations_c::SsbConfigurationBytes[SSB_NUMBER_OF_CONFIGURATION_BYTES] =
{
    // ================================================================
    SSBAL_CFG_DAT_ITEM_BEGIN,   // 4 Sensor modules with IMU+T sensor from ST (for PUMU)
    static_cast<uint8_t>(0x33U), static_cast<uint8_t>(0x11U), // <cfg_idx_high>, <cfg_idx_low>,
    // ----------------------------------------------------------------

    //-------- Startup of Hubs and Clients: -----------------------------------------------------------

    static_cast<uint8_t>(0x7FU), static_cast<uint8_t>(0x00U), // WR: [reg_page_select       = 7F] = 00 Normal_Page
    static_cast<uint8_t>(0x6EU), static_cast<uint8_t>(0x00U), // WR: [reg_loop              = 6E] = 00 Stopping the loop
    // ----------------------------------------------------------------
    static_cast<uint8_t>(0x88U), static_cast<uint8_t>(0x10U), // RD: [reg_int_hub           = 08] = 10 
    static_cast<uint8_t>(0x73U), static_cast<uint8_t>(0x01U), // WR: [reg_res_hub           = 73] = 01 
    static_cast<uint8_t>(0x88U), static_cast<uint8_t>(0x10U), // RD: [reg_int_hub           = 08] = 10 Reset Hub

    static_cast<uint8_t>(0x73U), static_cast<uint8_t>(0x02U), // WR: [reg_res_hub           = 73] = 02 Power for all Clients off (sleep-bit)
    static_cast<uint8_t>(0x88U), static_cast<uint8_t>(0x10U), // RD: [reg_int_hub           = 08] = 10 

    static_cast<uint8_t>(0x73U), static_cast<uint8_t>(0x01U), // WR: [reg_res_hub           = 73] = 01 Power for all Clients on and reset Hub
    static_cast<uint8_t>(0x88U), static_cast<uint8_t>(0x10U), // RD: [reg_int_hub           = 08] = 10 

    static_cast<uint8_t>(0x7EU), static_cast<uint8_t>(0x4BU), // WR: [reg_key               = 7E] = 4B 
    // ----------------------------------------------------------------
    static_cast<uint8_t>(0x6DU), static_cast<uint8_t>(0x01U), // WR: [reg_pwr_res           = 6D] = 01 
    // ----------------------------------------------------------------
    static_cast<uint8_t>(0x6FU), static_cast<uint8_t>(0x70U), // WR: [reg_addr              = 6F] = 70 ; RD_CLT_0
    static_cast<uint8_t>(0x71U), static_cast<uint8_t>(0x98U), // WR: [reg_access            = 71] = 98 ; RD_CLT_0
    static_cast<uint8_t>(0xF2U), static_cast<uint8_t>(0x01U), // RD: [reg_rx_data           = 72] = 01 ; RD_CLT_0: [reg_clt_status        = 70] = 01 
    // ----------------------------------------------------------------
    static_cast<uint8_t>(0x6FU), static_cast<uint8_t>(0x7EU), // WR: [reg_addr              = 6F] = 7E ; WR_CLT_0
    static_cast<uint8_t>(0x70U), static_cast<uint8_t>(0x4BU), // WR: [reg_tx_data           = 70] = 4B ; WR_CLT_0
    static_cast<uint8_t>(0x71U), static_cast<uint8_t>(0x88U), // WR: [reg_access            = 71] = 88 ; WR_CLT_0: [reg_key               = 7E] = 4B 
    // ----------------------------------------------------------------
    static_cast<uint8_t>(0x6FU), static_cast<uint8_t>(0x7FU), // WR: [reg_addr              = 6F] = 7F ; WR_CLT_0
    static_cast<uint8_t>(0x70U), static_cast<uint8_t>(0x00U), // WR: [reg_tx_data           = 70] = 00 ; WR_CLT_0
    static_cast<uint8_t>(0x71U), static_cast<uint8_t>(0x88U), // WR: [reg_access            = 71] = 88 ; WR_CLT_0: [reg_page_select       = 7F] = 00 Normal_Page_Clt
    // ----------------------------------------------------------------
    static_cast<uint8_t>(0x6DU), static_cast<uint8_t>(0x02U), // WR: [reg_pwr_res           = 6D] = 02 
    // ----------------------------------------------------------------
    static_cast<uint8_t>(0x6FU), static_cast<uint8_t>(0x70U), // WR: [reg_addr              = 6F] = 70 ; RD_CLT_1
    static_cast<uint8_t>(0x71U), static_cast<uint8_t>(0x99U), // WR: [reg_access            = 71] = 99 ; RD_CLT_1
    static_cast<uint8_t>(0xF2U), static_cast<uint8_t>(0x01U), // RD: [reg_rx_data           = 72] = 01 ; RD_CLT_1: [reg_clt_status        = 70] = 01 
    // ----------------------------------------------------------------
    static_cast<uint8_t>(0x6FU), static_cast<uint8_t>(0x7EU), // WR: [reg_addr              = 6F] = 7E ; WR_CLT_1
    static_cast<uint8_t>(0x70U), static_cast<uint8_t>(0x4BU), // WR: [reg_tx_data           = 70] = 4B ; WR_CLT_1
    static_cast<uint8_t>(0x71U), static_cast<uint8_t>(0x89U), // WR: [reg_access            = 71] = 89 ; WR_CLT_1: [reg_key               = 7E] = 4B 
    // ----------------------------------------------------------------
    static_cast<uint8_t>(0x6FU), static_cast<uint8_t>(0x7FU), // WR: [reg_addr              = 6F] = 7F ; WR_CLT_1
    static_cast<uint8_t>(0x70U), static_cast<uint8_t>(0x00U), // WR: [reg_tx_data           = 70] = 00 ; WR_CLT_1
    static_cast<uint8_t>(0x71U), static_cast<uint8_t>(0x89U), // WR: [reg_access            = 71] = 89 ; WR_CLT_1: [reg_page_select       = 7F] = 00 Normal_Page_Clt
    // ----------------------------------------------------------------
    static_cast<uint8_t>(0x6DU), static_cast<uint8_t>(0x04U), // WR: [reg_pwr_res           = 6D] = 04 
    // ----------------------------------------------------------------
    static_cast<uint8_t>(0x6FU), static_cast<uint8_t>(0x70U), // WR: [reg_addr              = 6F] = 70 ; RD_CLT_2
    static_cast<uint8_t>(0x71U), static_cast<uint8_t>(0x9AU), // WR: [reg_access            = 71] = 9A ; RD_CLT_2
    static_cast<uint8_t>(0xF2U), static_cast<uint8_t>(0x01U), // RD: [reg_rx_data           = 72] = 01 ; RD_CLT_2: [reg_clt_status        = 70] = 01 
    // ----------------------------------------------------------------
    static_cast<uint8_t>(0x6FU), static_cast<uint8_t>(0x7EU), // WR: [reg_addr              = 6F] = 7E ; WR_CLT_2
    static_cast<uint8_t>(0x70U), static_cast<uint8_t>(0x4BU), // WR: [reg_tx_data           = 70] = 4B ; WR_CLT_2
    static_cast<uint8_t>(0x71U), static_cast<uint8_t>(0x8AU), // WR: [reg_access            = 71] = 8A ; WR_CLT_2: [reg_key               = 7E] = 4B 
    // ----------------------------------------------------------------
    static_cast<uint8_t>(0x6FU), static_cast<uint8_t>(0x7FU), // WR: [reg_addr              = 6F] = 7F ; WR_CLT_2
    static_cast<uint8_t>(0x70U), static_cast<uint8_t>(0x00U), // WR: [reg_tx_data           = 70] = 00 ; WR_CLT_2
    static_cast<uint8_t>(0x71U), static_cast<uint8_t>(0x8AU), // WR: [reg_access            = 71] = 8A ; WR_CLT_2: [reg_page_select       = 7F] = 00 Normal_Page_Clt
    // ----------------------------------------------------------------
    static_cast<uint8_t>(0x6DU), static_cast<uint8_t>(0x08U), // WR: [reg_pwr_res           = 6D] = 08 
    // ----------------------------------------------------------------
    static_cast<uint8_t>(0x6FU), static_cast<uint8_t>(0x70U), // WR: [reg_addr              = 6F] = 70 ; RD_CLT_3
    static_cast<uint8_t>(0x71U), static_cast<uint8_t>(0x9BU), // WR: [reg_access            = 71] = 9B ; RD_CLT_3
    static_cast<uint8_t>(0xF2U), static_cast<uint8_t>(0x01U), // RD: [reg_rx_data           = 72] = 01 ; RD_CLT_3: [reg_clt_status        = 70] = 01 
    // ----------------------------------------------------------------
    static_cast<uint8_t>(0x6FU), static_cast<uint8_t>(0x7EU), // WR: [reg_addr              = 6F] = 7E ; WR_CLT_3
    static_cast<uint8_t>(0x70U), static_cast<uint8_t>(0x4BU), // WR: [reg_tx_data           = 70] = 4B ; WR_CLT_3
    static_cast<uint8_t>(0x71U), static_cast<uint8_t>(0x8BU), // WR: [reg_access            = 71] = 8B ; WR_CLT_3: [reg_key               = 7E] = 4B 
    // ----------------------------------------------------------------
    static_cast<uint8_t>(0x6FU), static_cast<uint8_t>(0x7FU), // WR: [reg_addr              = 6F] = 7F ; WR_CLT_3
    static_cast<uint8_t>(0x70U), static_cast<uint8_t>(0x00U), // WR: [reg_tx_data           = 70] = 00 ; WR_CLT_3
    static_cast<uint8_t>(0x71U), static_cast<uint8_t>(0x8BU), // WR: [reg_access            = 71] = 8B ; WR_CLT_3: [reg_page_select       = 7F] = 00 Normal_Page_Clt
    // ----------------------------------------------------------------
    static_cast<uint8_t>(0x88U), static_cast<uint8_t>(0x0FU), // RD: [reg_int_hub           = 08] = 0F 
    static_cast<uint8_t>(0x89U), static_cast<uint8_t>(0x20U), // RD: [reg_int_clt0          = 09] = 20 
    static_cast<uint8_t>(0x8AU), static_cast<uint8_t>(0x20U), // RD: [reg_int_clt1          = 0A] = 20 
    static_cast<uint8_t>(0x8BU), static_cast<uint8_t>(0x20U), // RD: [reg_int_clt2          = 0B] = 20 
    static_cast<uint8_t>(0x8CU), static_cast<uint8_t>(0x20U), // RD: [reg_int_clt3          = 0C] = 20 
    // ----------------------------------------------------------------

    //-------- Configuration of Clients and Devices: --------------------------------------------------

    static_cast<uint8_t>(0x6FU), static_cast<uint8_t>(0x7EU), // WR: [reg_addr              = 6F] = 7E ; WR_CLT_0
    static_cast<uint8_t>(0x70U), static_cast<uint8_t>(0x4BU), // WR: [reg_tx_data           = 70] = 4B ; WR_CLT_0
    static_cast<uint8_t>(0x71U), static_cast<uint8_t>(0x88U), // WR: [reg_access            = 71] = 88 ; WR_CLT_0: [reg_key               = 7E] = 4B 
    // ----------------------------------------------------------------
    static_cast<uint8_t>(0x6FU), static_cast<uint8_t>(0x75U), // WR: [reg_addr              = 6F] = 75 ; WR_CLT_0
    static_cast<uint8_t>(0x70U), static_cast<uint8_t>(0x07U), // WR: [reg_tx_data           = 70] = 07 ; WR_CLT_0
    static_cast<uint8_t>(0x71U), static_cast<uint8_t>(0x88U), // WR: [reg_access            = 71] = 88 ; WR_CLT_0: [reg_temp_cfg          = 75] = 07 
    // ----------------------------------------------------------------
    static_cast<uint8_t>(0x6FU), static_cast<uint8_t>(0x71U), // WR: [reg_addr              = 6F] = 71 ; WR_CLT_0
    static_cast<uint8_t>(0x70U), static_cast<uint8_t>(0x01U), // WR: [reg_tx_data           = 70] = 01 ; WR_CLT_0
    static_cast<uint8_t>(0x71U), static_cast<uint8_t>(0x88U), // WR: [reg_access            = 71] = 88 ; WR_CLT_0: [reg_pwr_sensor        = 71] = 01 
    // ----------------------------------------------------------------
    static_cast<uint8_t>(0x6FU), static_cast<uint8_t>(0x71U), // WR: [reg_addr              = 6F] = 71 ; RD_CLT_0
    static_cast<uint8_t>(0x71U), static_cast<uint8_t>(0x98U), // WR: [reg_access            = 71] = 98 ; RD_CLT_0
    static_cast<uint8_t>(0xF2U), static_cast<uint8_t>(0x01U), // RD: [reg_rx_data           = 72] = 01 ; RD_CLT_0: [reg_pwr_sensor        = 71] = 01 
    // ----------------------------------------------------------------
    static_cast<uint8_t>(0x70U), static_cast<uint8_t>(0x11U), // WR: [reg_tx_data           = 70] = 11 ; WR_CLT_0_SENS_0
    static_cast<uint8_t>(0x71U), static_cast<uint8_t>(0x80U), // WR: [reg_access            = 71] = 80 ; WR_CLT_0_SENS_0: 11
    // ----------------------------------------------------------------
    static_cast<uint8_t>(0x70U), static_cast<uint8_t>(0x4CU), // WR: [reg_tx_data           = 70] = 4C ; WR_CLT_0_SENS_0
    static_cast<uint8_t>(0x71U), static_cast<uint8_t>(0x80U), // WR: [reg_access            = 71] = 80 ; WR_CLT_0_SENS_0: 4C
    // ----------------------------------------------------------------
    static_cast<uint8_t>(0x71U), static_cast<uint8_t>(0xC0U), // WR: [reg_access            = 71] = C0 ; WR_CLT_0_SENS_0: EoT
    // ----------------------------------------------------------------
    static_cast<uint8_t>(0x70U), static_cast<uint8_t>(0x91U), // WR: [reg_tx_data           = 70] = 91 ; WR_CLT_0_SENS_0
    static_cast<uint8_t>(0x71U), static_cast<uint8_t>(0x80U), // WR: [reg_access            = 71] = 80 ; WR_CLT_0_SENS_0: 91
    // ----------------------------------------------------------------
    static_cast<uint8_t>(0x71U), static_cast<uint8_t>(0xB0U), // WR: [reg_access            = 71] = B0 ; RD_CLT_0_SENS_0
    static_cast<uint8_t>(0xF2U), static_cast<uint8_t>(0x4CU), // RD: [reg_rx_data           = 72] = 4C ; RD_CLT_0_SENS_0: 4C
    // ----------------------------------------------------------------
    static_cast<uint8_t>(0x71U), static_cast<uint8_t>(0xD0U), // WR: [reg_access            = 71] = D0 ; RD_CLT_0_SENS_0: EoT
    // ----------------------------------------------------------------
    static_cast<uint8_t>(0x70U), static_cast<uint8_t>(0x10U), // WR: [reg_tx_data           = 70] = 10 ; WR_CLT_0_SENS_0
    static_cast<uint8_t>(0x71U), static_cast<uint8_t>(0x80U), // WR: [reg_access            = 71] = 80 ; WR_CLT_0_SENS_0: 10
    // ----------------------------------------------------------------
    static_cast<uint8_t>(0x70U), static_cast<uint8_t>(0x48U), // WR: [reg_tx_data           = 70] = 48 ; WR_CLT_0_SENS_0
    static_cast<uint8_t>(0x71U), static_cast<uint8_t>(0x80U), // WR: [reg_access            = 71] = 80 ; WR_CLT_0_SENS_0: 48
    // ----------------------------------------------------------------
    static_cast<uint8_t>(0x71U), static_cast<uint8_t>(0xC0U), // WR: [reg_access            = 71] = C0 ; WR_CLT_0_SENS_0: EoT
    // ----------------------------------------------------------------
    static_cast<uint8_t>(0x70U), static_cast<uint8_t>(0x90U), // WR: [reg_tx_data           = 70] = 90 ; WR_CLT_0_SENS_0
    static_cast<uint8_t>(0x71U), static_cast<uint8_t>(0x80U), // WR: [reg_access            = 71] = 80 ; WR_CLT_0_SENS_0: 90
    // ----------------------------------------------------------------
    static_cast<uint8_t>(0x71U), static_cast<uint8_t>(0xB0U), // WR: [reg_access            = 71] = B0 ; RD_CLT_0_SENS_0
    static_cast<uint8_t>(0xF2U), static_cast<uint8_t>(0x48U), // RD: [reg_rx_data           = 72] = 48 ; RD_CLT_0_SENS_0: 48
    // ----------------------------------------------------------------
    static_cast<uint8_t>(0x71U), static_cast<uint8_t>(0xD0U), // WR: [reg_access            = 71] = D0 ; RD_CLT_0_SENS_0: EoT
    // ----------------------------------------------------------------
    static_cast<uint8_t>(0x6FU), static_cast<uint8_t>(0x7EU), // WR: [reg_addr              = 6F] = 7E ; WR_CLT_1
    static_cast<uint8_t>(0x70U), static_cast<uint8_t>(0x4BU), // WR: [reg_tx_data           = 70] = 4B ; WR_CLT_1
    static_cast<uint8_t>(0x71U), static_cast<uint8_t>(0x89U), // WR: [reg_access            = 71] = 89 ; WR_CLT_1: [reg_key               = 7E] = 4B 
    // ----------------------------------------------------------------
    static_cast<uint8_t>(0x6FU), static_cast<uint8_t>(0x75U), // WR: [reg_addr              = 6F] = 75 ; WR_CLT_1
    static_cast<uint8_t>(0x70U), static_cast<uint8_t>(0x07U), // WR: [reg_tx_data           = 70] = 07 ; WR_CLT_1
    static_cast<uint8_t>(0x71U), static_cast<uint8_t>(0x89U), // WR: [reg_access            = 71] = 89 ; WR_CLT_1: [reg_temp_cfg          = 75] = 07 
    // ----------------------------------------------------------------
    static_cast<uint8_t>(0x6FU), static_cast<uint8_t>(0x71U), // WR: [reg_addr              = 6F] = 71 ; WR_CLT_1
    static_cast<uint8_t>(0x70U), static_cast<uint8_t>(0x01U), // WR: [reg_tx_data           = 70] = 01 ; WR_CLT_1
    static_cast<uint8_t>(0x71U), static_cast<uint8_t>(0x89U), // WR: [reg_access            = 71] = 89 ; WR_CLT_1: [reg_pwr_sensor        = 71] = 01 
    // ----------------------------------------------------------------
    static_cast<uint8_t>(0x6FU), static_cast<uint8_t>(0x71U), // WR: [reg_addr              = 6F] = 71 ; RD_CLT_1
    static_cast<uint8_t>(0x71U), static_cast<uint8_t>(0x99U), // WR: [reg_access            = 71] = 99 ; RD_CLT_1
    static_cast<uint8_t>(0xF2U), static_cast<uint8_t>(0x01U), // RD: [reg_rx_data           = 72] = 01 ; RD_CLT_1: [reg_pwr_sensor        = 71] = 01 
    // ----------------------------------------------------------------
    static_cast<uint8_t>(0x70U), static_cast<uint8_t>(0x11U), // WR: [reg_tx_data           = 70] = 11 ; WR_CLT_1_SENS_0
    static_cast<uint8_t>(0x71U), static_cast<uint8_t>(0x81U), // WR: [reg_access            = 71] = 81 ; WR_CLT_1_SENS_0: 11
    // ----------------------------------------------------------------
    static_cast<uint8_t>(0x70U), static_cast<uint8_t>(0x4CU), // WR: [reg_tx_data           = 70] = 4C ; WR_CLT_1_SENS_0
    static_cast<uint8_t>(0x71U), static_cast<uint8_t>(0x81U), // WR: [reg_access            = 71] = 81 ; WR_CLT_1_SENS_0: 4C
    // ----------------------------------------------------------------
    static_cast<uint8_t>(0x71U), static_cast<uint8_t>(0xC1U), // WR: [reg_access            = 71] = C1 ; WR_CLT_1_SENS_0: EoT
    // ----------------------------------------------------------------
    static_cast<uint8_t>(0x70U), static_cast<uint8_t>(0x91U), // WR: [reg_tx_data           = 70] = 91 ; WR_CLT_1_SENS_0
    static_cast<uint8_t>(0x71U), static_cast<uint8_t>(0x81U), // WR: [reg_access            = 71] = 81 ; WR_CLT_1_SENS_0: 91
    // ----------------------------------------------------------------
    static_cast<uint8_t>(0x71U), static_cast<uint8_t>(0xB1U), // WR: [reg_access            = 71] = B1 ; RD_CLT_1_SENS_0
    static_cast<uint8_t>(0xF2U), static_cast<uint8_t>(0x4CU), // RD: [reg_rx_data           = 72] = 4C ; RD_CLT_1_SENS_0: 4C
    // ----------------------------------------------------------------
    static_cast<uint8_t>(0x71U), static_cast<uint8_t>(0xD1U), // WR: [reg_access            = 71] = D1 ; RD_CLT_1_SENS_0: EoT
    // ----------------------------------------------------------------
    static_cast<uint8_t>(0x70U), static_cast<uint8_t>(0x10U), // WR: [reg_tx_data           = 70] = 10 ; WR_CLT_1_SENS_0
    static_cast<uint8_t>(0x71U), static_cast<uint8_t>(0x81U), // WR: [reg_access            = 71] = 81 ; WR_CLT_1_SENS_0: 10
    // ----------------------------------------------------------------
    static_cast<uint8_t>(0x70U), static_cast<uint8_t>(0x48U), // WR: [reg_tx_data           = 70] = 48 ; WR_CLT_1_SENS_0
    static_cast<uint8_t>(0x71U), static_cast<uint8_t>(0x81U), // WR: [reg_access            = 71] = 81 ; WR_CLT_1_SENS_0: 48
    // ----------------------------------------------------------------
    static_cast<uint8_t>(0x71U), static_cast<uint8_t>(0xC1U), // WR: [reg_access            = 71] = C1 ; WR_CLT_1_SENS_0: EoT
    // ----------------------------------------------------------------
    static_cast<uint8_t>(0x70U), static_cast<uint8_t>(0x90U), // WR: [reg_tx_data           = 70] = 90 ; WR_CLT_1_SENS_0
    static_cast<uint8_t>(0x71U), static_cast<uint8_t>(0x81U), // WR: [reg_access            = 71] = 81 ; WR_CLT_1_SENS_0: 90
    // ----------------------------------------------------------------
    static_cast<uint8_t>(0x71U), static_cast<uint8_t>(0xB1U), // WR: [reg_access            = 71] = B1 ; RD_CLT_1_SENS_0
    static_cast<uint8_t>(0xF2U), static_cast<uint8_t>(0x48U), // RD: [reg_rx_data           = 72] = 48 ; RD_CLT_1_SENS_0: 48
    // ----------------------------------------------------------------
    static_cast<uint8_t>(0x71U), static_cast<uint8_t>(0xD1U), // WR: [reg_access            = 71] = D1 ; RD_CLT_1_SENS_0: EoT
    // ----------------------------------------------------------------
    static_cast<uint8_t>(0x6FU), static_cast<uint8_t>(0x7EU), // WR: [reg_addr              = 6F] = 7E ; WR_CLT_2
    static_cast<uint8_t>(0x70U), static_cast<uint8_t>(0x4BU), // WR: [reg_tx_data           = 70] = 4B ; WR_CLT_2
    static_cast<uint8_t>(0x71U), static_cast<uint8_t>(0x8AU), // WR: [reg_access            = 71] = 8A ; WR_CLT_2: [reg_key               = 7E] = 4B 
    // ----------------------------------------------------------------
    static_cast<uint8_t>(0x6FU), static_cast<uint8_t>(0x75U), // WR: [reg_addr              = 6F] = 75 ; WR_CLT_2
    static_cast<uint8_t>(0x70U), static_cast<uint8_t>(0x07U), // WR: [reg_tx_data           = 70] = 07 ; WR_CLT_2
    static_cast<uint8_t>(0x71U), static_cast<uint8_t>(0x8AU), // WR: [reg_access            = 71] = 8A ; WR_CLT_2: [reg_temp_cfg          = 75] = 07 
    // ----------------------------------------------------------------
    static_cast<uint8_t>(0x6FU), static_cast<uint8_t>(0x71U), // WR: [reg_addr              = 6F] = 71 ; WR_CLT_2
    static_cast<uint8_t>(0x70U), static_cast<uint8_t>(0x01U), // WR: [reg_tx_data           = 70] = 01 ; WR_CLT_2
    static_cast<uint8_t>(0x71U), static_cast<uint8_t>(0x8AU), // WR: [reg_access            = 71] = 8A ; WR_CLT_2: [reg_pwr_sensor        = 71] = 01 
    // ----------------------------------------------------------------
    static_cast<uint8_t>(0x6FU), static_cast<uint8_t>(0x71U), // WR: [reg_addr              = 6F] = 71 ; RD_CLT_2
    static_cast<uint8_t>(0x71U), static_cast<uint8_t>(0x9AU), // WR: [reg_access            = 71] = 9A ; RD_CLT_2
    static_cast<uint8_t>(0xF2U), static_cast<uint8_t>(0x01U), // RD: [reg_rx_data           = 72] = 01 ; RD_CLT_2: [reg_pwr_sensor        = 71] = 01 
    // ----------------------------------------------------------------
    static_cast<uint8_t>(0x70U), static_cast<uint8_t>(0x11U), // WR: [reg_tx_data           = 70] = 11 ; WR_CLT_2_SENS_0
    static_cast<uint8_t>(0x71U), static_cast<uint8_t>(0x82U), // WR: [reg_access            = 71] = 82 ; WR_CLT_2_SENS_0: 11
    // ----------------------------------------------------------------
    static_cast<uint8_t>(0x70U), static_cast<uint8_t>(0x4CU), // WR: [reg_tx_data           = 70] = 4C ; WR_CLT_2_SENS_0
    static_cast<uint8_t>(0x71U), static_cast<uint8_t>(0x82U), // WR: [reg_access            = 71] = 82 ; WR_CLT_2_SENS_0: 4C
    // ----------------------------------------------------------------
    static_cast<uint8_t>(0x71U), static_cast<uint8_t>(0xC2U), // WR: [reg_access            = 71] = C2 ; WR_CLT_2_SENS_0: EoT
    // ----------------------------------------------------------------
    static_cast<uint8_t>(0x70U), static_cast<uint8_t>(0x91U), // WR: [reg_tx_data           = 70] = 91 ; WR_CLT_2_SENS_0
    static_cast<uint8_t>(0x71U), static_cast<uint8_t>(0x82U), // WR: [reg_access            = 71] = 82 ; WR_CLT_2_SENS_0: 91
    // ----------------------------------------------------------------
    static_cast<uint8_t>(0x71U), static_cast<uint8_t>(0xB2U), // WR: [reg_access            = 71] = B2 ; RD_CLT_2_SENS_0
    static_cast<uint8_t>(0xF2U), static_cast<uint8_t>(0x4CU), // RD: [reg_rx_data           = 72] = 4C ; RD_CLT_2_SENS_0: 4C
    // ----------------------------------------------------------------
    static_cast<uint8_t>(0x71U), static_cast<uint8_t>(0xD2U), // WR: [reg_access            = 71] = D2 ; RD_CLT_2_SENS_0: EoT
    // ----------------------------------------------------------------
    static_cast<uint8_t>(0x70U), static_cast<uint8_t>(0x10U), // WR: [reg_tx_data           = 70] = 10 ; WR_CLT_2_SENS_0
    static_cast<uint8_t>(0x71U), static_cast<uint8_t>(0x82U), // WR: [reg_access            = 71] = 82 ; WR_CLT_2_SENS_0: 10
    // ----------------------------------------------------------------
    static_cast<uint8_t>(0x70U), static_cast<uint8_t>(0x48U), // WR: [reg_tx_data           = 70] = 48 ; WR_CLT_2_SENS_0
    static_cast<uint8_t>(0x71U), static_cast<uint8_t>(0x82U), // WR: [reg_access            = 71] = 82 ; WR_CLT_2_SENS_0: 48
    // ----------------------------------------------------------------
    static_cast<uint8_t>(0x71U), static_cast<uint8_t>(0xC2U), // WR: [reg_access            = 71] = C2 ; WR_CLT_2_SENS_0: EoT
    // ----------------------------------------------------------------
    static_cast<uint8_t>(0x70U), static_cast<uint8_t>(0x90U), // WR: [reg_tx_data           = 70] = 90 ; WR_CLT_2_SENS_0
    static_cast<uint8_t>(0x71U), static_cast<uint8_t>(0x82U), // WR: [reg_access            = 71] = 82 ; WR_CLT_2_SENS_0: 90
    // ----------------------------------------------------------------
    static_cast<uint8_t>(0x71U), static_cast<uint8_t>(0xB2U), // WR: [reg_access            = 71] = B2 ; RD_CLT_2_SENS_0
    static_cast<uint8_t>(0xF2U), static_cast<uint8_t>(0x48U), // RD: [reg_rx_data           = 72] = 48 ; RD_CLT_2_SENS_0: 48
    // ----------------------------------------------------------------
    static_cast<uint8_t>(0x71U), static_cast<uint8_t>(0xD2U), // WR: [reg_access            = 71] = D2 ; RD_CLT_2_SENS_0: EoT
    // ----------------------------------------------------------------
    static_cast<uint8_t>(0x6FU), static_cast<uint8_t>(0x7EU), // WR: [reg_addr              = 6F] = 7E ; WR_CLT_3
    static_cast<uint8_t>(0x70U), static_cast<uint8_t>(0x4BU), // WR: [reg_tx_data           = 70] = 4B ; WR_CLT_3
    static_cast<uint8_t>(0x71U), static_cast<uint8_t>(0x8BU), // WR: [reg_access            = 71] = 8B ; WR_CLT_3: [reg_key               = 7E] = 4B 
    // ----------------------------------------------------------------
    static_cast<uint8_t>(0x6FU), static_cast<uint8_t>(0x75U), // WR: [reg_addr              = 6F] = 75 ; WR_CLT_3
    static_cast<uint8_t>(0x70U), static_cast<uint8_t>(0x07U), // WR: [reg_tx_data           = 70] = 07 ; WR_CLT_3
    static_cast<uint8_t>(0x71U), static_cast<uint8_t>(0x8BU), // WR: [reg_access            = 71] = 8B ; WR_CLT_3: [reg_temp_cfg          = 75] = 07 
    // ----------------------------------------------------------------
    static_cast<uint8_t>(0x6FU), static_cast<uint8_t>(0x71U), // WR: [reg_addr              = 6F] = 71 ; WR_CLT_3
    static_cast<uint8_t>(0x70U), static_cast<uint8_t>(0x01U), // WR: [reg_tx_data           = 70] = 01 ; WR_CLT_3
    static_cast<uint8_t>(0x71U), static_cast<uint8_t>(0x8BU), // WR: [reg_access            = 71] = 8B ; WR_CLT_3: [reg_pwr_sensor        = 71] = 01 
    // ----------------------------------------------------------------
    static_cast<uint8_t>(0x6FU), static_cast<uint8_t>(0x71U), // WR: [reg_addr              = 6F] = 71 ; RD_CLT_3
    static_cast<uint8_t>(0x71U), static_cast<uint8_t>(0x9BU), // WR: [reg_access            = 71] = 9B ; RD_CLT_3
    static_cast<uint8_t>(0xF2U), static_cast<uint8_t>(0x01U), // RD: [reg_rx_data           = 72] = 01 ; RD_CLT_3: [reg_pwr_sensor        = 71] = 01 
    // ----------------------------------------------------------------
    static_cast<uint8_t>(0x70U), static_cast<uint8_t>(0x11U), // WR: [reg_tx_data           = 70] = 11 ; WR_CLT_3_SENS_0
    static_cast<uint8_t>(0x71U), static_cast<uint8_t>(0x83U), // WR: [reg_access            = 71] = 83 ; WR_CLT_3_SENS_0: 11
    // ----------------------------------------------------------------
    static_cast<uint8_t>(0x70U), static_cast<uint8_t>(0x4CU), // WR: [reg_tx_data           = 70] = 4C ; WR_CLT_3_SENS_0
    static_cast<uint8_t>(0x71U), static_cast<uint8_t>(0x83U), // WR: [reg_access            = 71] = 83 ; WR_CLT_3_SENS_0: 4C
    // ----------------------------------------------------------------
    static_cast<uint8_t>(0x71U), static_cast<uint8_t>(0xC3U), // WR: [reg_access            = 71] = C3 ; WR_CLT_3_SENS_0: EoT
    // ----------------------------------------------------------------
    static_cast<uint8_t>(0x70U), static_cast<uint8_t>(0x91U), // WR: [reg_tx_data           = 70] = 91 ; WR_CLT_3_SENS_0
    static_cast<uint8_t>(0x71U), static_cast<uint8_t>(0x83U), // WR: [reg_access            = 71] = 83 ; WR_CLT_3_SENS_0: 91
    // ----------------------------------------------------------------
    static_cast<uint8_t>(0x71U), static_cast<uint8_t>(0xB3U), // WR: [reg_access            = 71] = B3 ; RD_CLT_3_SENS_0
    static_cast<uint8_t>(0xF2U), static_cast<uint8_t>(0x4CU), // RD: [reg_rx_data           = 72] = 4C ; RD_CLT_3_SENS_0: 4C
    // ----------------------------------------------------------------
    static_cast<uint8_t>(0x71U), static_cast<uint8_t>(0xD3U), // WR: [reg_access            = 71] = D3 ; RD_CLT_3_SENS_0: EoT
    // ----------------------------------------------------------------
    static_cast<uint8_t>(0x70U), static_cast<uint8_t>(0x10U), // WR: [reg_tx_data           = 70] = 10 ; WR_CLT_3_SENS_0
    static_cast<uint8_t>(0x71U), static_cast<uint8_t>(0x83U), // WR: [reg_access            = 71] = 83 ; WR_CLT_3_SENS_0: 10
    // ----------------------------------------------------------------
    static_cast<uint8_t>(0x70U), static_cast<uint8_t>(0x48U), // WR: [reg_tx_data           = 70] = 48 ; WR_CLT_3_SENS_0
    static_cast<uint8_t>(0x71U), static_cast<uint8_t>(0x83U), // WR: [reg_access            = 71] = 83 ; WR_CLT_3_SENS_0: 48
    // ----------------------------------------------------------------
    static_cast<uint8_t>(0x71U), static_cast<uint8_t>(0xC3U), // WR: [reg_access            = 71] = C3 ; WR_CLT_3_SENS_0: EoT
    // ----------------------------------------------------------------
    static_cast<uint8_t>(0x70U), static_cast<uint8_t>(0x90U), // WR: [reg_tx_data           = 70] = 90 ; WR_CLT_3_SENS_0
    static_cast<uint8_t>(0x71U), static_cast<uint8_t>(0x83U), // WR: [reg_access            = 71] = 83 ; WR_CLT_3_SENS_0: 90
    // ----------------------------------------------------------------
    static_cast<uint8_t>(0x71U), static_cast<uint8_t>(0xB3U), // WR: [reg_access            = 71] = B3 ; RD_CLT_3_SENS_0
    static_cast<uint8_t>(0xF2U), static_cast<uint8_t>(0x48U), // RD: [reg_rx_data           = 72] = 48 ; RD_CLT_3_SENS_0: 48
    // ----------------------------------------------------------------
    static_cast<uint8_t>(0x71U), static_cast<uint8_t>(0xD3U), // WR: [reg_access            = 71] = D3 ; RD_CLT_3_SENS_0: EoT
    // ----------------------------------------------------------------

    // -------- Preparation of measurement loop: ------------------------------------------------------

    static_cast<uint8_t>(0x03U), static_cast<uint8_t>(0x16U), // WR: [reg_tread_sensor      = 03] = 16 
    static_cast<uint8_t>(0x01U), static_cast<uint8_t>(0x25U), // WR: [reg_tloop_period      = 01] = 25 
    static_cast<uint8_t>(0x02U), static_cast<uint8_t>(0x0FU), // WR: [reg_en_client         = 02] = 0F 
    static_cast<uint8_t>(0x7EU), static_cast<uint8_t>(0x4BU), // WR: [reg_key               = 7E] = 4B 
    static_cast<uint8_t>(0x7FU), static_cast<uint8_t>(0x00U), // WR: [reg_page_select       = 7F] = 00 Normal_Page
    // ================================================================

    SSBAL_CFG_DAT_ITEM_BEGIN,  // Power down of sensor modules (only dummy data preliminarily here)
    static_cast<uint8_t>(0x33U), static_cast<uint8_t>(0x10U), // <cfg_idx_high>, <cfg_idx_low>,
    // ----------------------------------------------------------------

    //-------- Startup of Hubs and Clients: -----------------------------------------------------------

    static_cast<uint8_t>(0x7FU), static_cast<uint8_t>(0x00U), // WR: [reg_page_select       = 7F] = 00 Normal_Page
    static_cast<uint8_t>(0x6EU), static_cast<uint8_t>(0x00U), // WR: [reg_loop              = 6E] = 00 Stopping the loop
    // ================================================================

    SSBAL_CFG_DAT_END  // Only once after the configs of all cfg_idx
    // ----------------------------------------------------------------
};

#elif defined SSBCFG_IMUT_ST_n_n_n_PUMU0
const uint8_t
SsbConfigurations_c::SsbConfigurationBytes[SSB_NUMBER_OF_CONFIGURATION_BYTES] =
{
    // ================================================================
    SSBAL_CFG_DAT_ITEM_BEGIN,
    static_cast<uint8_t>(0x33U), static_cast<uint8_t>(0x11U), // <cfg_idx_high>, <cfg_idx_low>,
    // ----------------------------------------------------------------

    //-------- Startup of Hubs and Clients: -----------------------------------------------------------

    static_cast<uint8_t>(0x7FU), static_cast<uint8_t>(0x00U), // WR: [reg_page_select       = 7F] = 00 Normal_Page
    static_cast<uint8_t>(0x6EU), static_cast<uint8_t>(0x00U), // WR: [reg_loop              = 6E] = 00 Stopping the loop
    // ----------------------------------------------------------------
    static_cast<uint8_t>(0x88U), static_cast<uint8_t>(0x10U), // RD: [reg_int_hub           = 08] = 10 
    static_cast<uint8_t>(0x73U), static_cast<uint8_t>(0x01U), // WR: [reg_res_hub           = 73] = 01 
    static_cast<uint8_t>(0x88U), static_cast<uint8_t>(0x10U), // RD: [reg_int_hub           = 08] = 10 Reset Hub

    static_cast<uint8_t>(0x73U), static_cast<uint8_t>(0x02U), // WR: [reg_res_hub           = 73] = 02 Power for all Clients off (sleep-bit)
    static_cast<uint8_t>(0x88U), static_cast<uint8_t>(0x10U), // RD: [reg_int_hub           = 08] = 10 

    static_cast<uint8_t>(0x73U), static_cast<uint8_t>(0x01U), // WR: [reg_res_hub           = 73] = 01 Power for all Clients on and reset Hub
    static_cast<uint8_t>(0x88U), static_cast<uint8_t>(0x10U), // RD: [reg_int_hub           = 08] = 10 

    static_cast<uint8_t>(0x7EU), static_cast<uint8_t>(0x4BU), // WR: [reg_key               = 7E] = 4B 
    // ----------------------------------------------------------------
    static_cast<uint8_t>(0x6DU), static_cast<uint8_t>(0x01U), // WR: [reg_pwr_res           = 6D] = 01 
    // ----------------------------------------------------------------
    static_cast<uint8_t>(0x6FU), static_cast<uint8_t>(0x70U), // WR: [reg_addr              = 6F] = 70 ; RD_CLT_0
    static_cast<uint8_t>(0x71U), static_cast<uint8_t>(0x98U), // WR: [reg_access            = 71] = 98 ; RD_CLT_0
    static_cast<uint8_t>(0xF2U), static_cast<uint8_t>(0x01U), // RD: [reg_rx_data           = 72] = 01 ; RD_CLT_0: [reg_clt_status        = 70] = 01 
    // ----------------------------------------------------------------
    static_cast<uint8_t>(0x6FU), static_cast<uint8_t>(0x7EU), // WR: [reg_addr              = 6F] = 7E ; WR_CLT_0
    static_cast<uint8_t>(0x70U), static_cast<uint8_t>(0x4BU), // WR: [reg_tx_data           = 70] = 4B ; WR_CLT_0
    static_cast<uint8_t>(0x71U), static_cast<uint8_t>(0x88U), // WR: [reg_access            = 71] = 88 ; WR_CLT_0: [reg_key               = 7E] = 4B 
    // ----------------------------------------------------------------
    static_cast<uint8_t>(0x6FU), static_cast<uint8_t>(0x7FU), // WR: [reg_addr              = 6F] = 7F ; WR_CLT_0
    static_cast<uint8_t>(0x70U), static_cast<uint8_t>(0x00U), // WR: [reg_tx_data           = 70] = 00 ; WR_CLT_0
    static_cast<uint8_t>(0x71U), static_cast<uint8_t>(0x88U), // WR: [reg_access            = 71] = 88 ; WR_CLT_0: [reg_page_select       = 7F] = 00 Normal_Page_Clt
    // ----------------------------------------------------------------
    static_cast<uint8_t>(0x88U), static_cast<uint8_t>(0x0FU), // RD: [reg_int_hub           = 08] = 0F 
    static_cast<uint8_t>(0x89U), static_cast<uint8_t>(0x20U), // RD: [reg_int_clt0          = 09] = 20 
    // ----------------------------------------------------------------

    //-------- Configuration of Clients and Devices: --------------------------------------------------

    static_cast<uint8_t>(0x6FU), static_cast<uint8_t>(0x7EU), // WR: [reg_addr              = 6F] = 7E ; WR_CLT_0
    static_cast<uint8_t>(0x70U), static_cast<uint8_t>(0x4BU), // WR: [reg_tx_data           = 70] = 4B ; WR_CLT_0
    static_cast<uint8_t>(0x71U), static_cast<uint8_t>(0x88U), // WR: [reg_access            = 71] = 88 ; WR_CLT_0: [reg_key               = 7E] = 4B 
    // ----------------------------------------------------------------
    static_cast<uint8_t>(0x6FU), static_cast<uint8_t>(0x75U), // WR: [reg_addr              = 6F] = 75 ; WR_CLT_0
    static_cast<uint8_t>(0x70U), static_cast<uint8_t>(0x07U), // WR: [reg_tx_data           = 70] = 07 ; WR_CLT_0
    static_cast<uint8_t>(0x71U), static_cast<uint8_t>(0x88U), // WR: [reg_access            = 71] = 88 ; WR_CLT_0: [reg_temp_cfg          = 75] = 07 
    // ----------------------------------------------------------------
    static_cast<uint8_t>(0x6FU), static_cast<uint8_t>(0x71U), // WR: [reg_addr              = 6F] = 71 ; WR_CLT_0
    static_cast<uint8_t>(0x70U), static_cast<uint8_t>(0x01U), // WR: [reg_tx_data           = 70] = 01 ; WR_CLT_0
    static_cast<uint8_t>(0x71U), static_cast<uint8_t>(0x88U), // WR: [reg_access            = 71] = 88 ; WR_CLT_0: [reg_pwr_sensor        = 71] = 01 
    // ----------------------------------------------------------------
    static_cast<uint8_t>(0x6FU), static_cast<uint8_t>(0x71U), // WR: [reg_addr              = 6F] = 71 ; RD_CLT_0
    static_cast<uint8_t>(0x71U), static_cast<uint8_t>(0x98U), // WR: [reg_access            = 71] = 98 ; RD_CLT_0
    static_cast<uint8_t>(0xF2U), static_cast<uint8_t>(0x01U), // RD: [reg_rx_data           = 72] = 01 ; RD_CLT_0: [reg_pwr_sensor        = 71] = 01 
    // ----------------------------------------------------------------
    static_cast<uint8_t>(0x70U), static_cast<uint8_t>(0x11U), // WR: [reg_tx_data           = 70] = 11 ; WR_CLT_0_SENS_0
    static_cast<uint8_t>(0x71U), static_cast<uint8_t>(0x80U), // WR: [reg_access            = 71] = 80 ; WR_CLT_0_SENS_0: 11
    // ----------------------------------------------------------------
    static_cast<uint8_t>(0x70U), static_cast<uint8_t>(0x4CU), // WR: [reg_tx_data           = 70] = 4C ; WR_CLT_0_SENS_0
    static_cast<uint8_t>(0x71U), static_cast<uint8_t>(0x80U), // WR: [reg_access            = 71] = 80 ; WR_CLT_0_SENS_0: 4C
    // ----------------------------------------------------------------
    static_cast<uint8_t>(0x71U), static_cast<uint8_t>(0xC0U), // WR: [reg_access            = 71] = C0 ; WR_CLT_0_SENS_0: EoT
    // ----------------------------------------------------------------
    static_cast<uint8_t>(0x70U), static_cast<uint8_t>(0x91U), // WR: [reg_tx_data           = 70] = 91 ; WR_CLT_0_SENS_0
    static_cast<uint8_t>(0x71U), static_cast<uint8_t>(0x80U), // WR: [reg_access            = 71] = 80 ; WR_CLT_0_SENS_0: 91
    // ----------------------------------------------------------------
    static_cast<uint8_t>(0x71U), static_cast<uint8_t>(0xB0U), // WR: [reg_access            = 71] = B0 ; RD_CLT_0_SENS_0
    static_cast<uint8_t>(0xF2U), static_cast<uint8_t>(0x4CU), // RD: [reg_rx_data           = 72] = 4C ; RD_CLT_0_SENS_0: 4C
    // ----------------------------------------------------------------
    static_cast<uint8_t>(0x71U), static_cast<uint8_t>(0xD0U), // WR: [reg_access            = 71] = D0 ; RD_CLT_0_SENS_0: EoT
    // ----------------------------------------------------------------
    static_cast<uint8_t>(0x70U), static_cast<uint8_t>(0x10U), // WR: [reg_tx_data           = 70] = 10 ; WR_CLT_0_SENS_0
    static_cast<uint8_t>(0x71U), static_cast<uint8_t>(0x80U), // WR: [reg_access            = 71] = 80 ; WR_CLT_0_SENS_0: 10
    // ----------------------------------------------------------------
    static_cast<uint8_t>(0x70U), static_cast<uint8_t>(0x48U), // WR: [reg_tx_data           = 70] = 48 ; WR_CLT_0_SENS_0
    static_cast<uint8_t>(0x71U), static_cast<uint8_t>(0x80U), // WR: [reg_access            = 71] = 80 ; WR_CLT_0_SENS_0: 48
    // ----------------------------------------------------------------
    static_cast<uint8_t>(0x71U), static_cast<uint8_t>(0xC0U), // WR: [reg_access            = 71] = C0 ; WR_CLT_0_SENS_0: EoT
    // ----------------------------------------------------------------
    static_cast<uint8_t>(0x70U), static_cast<uint8_t>(0x90U), // WR: [reg_tx_data           = 70] = 90 ; WR_CLT_0_SENS_0
    static_cast<uint8_t>(0x71U), static_cast<uint8_t>(0x80U), // WR: [reg_access            = 71] = 80 ; WR_CLT_0_SENS_0: 90
    // ----------------------------------------------------------------
    static_cast<uint8_t>(0x71U), static_cast<uint8_t>(0xB0U), // WR: [reg_access            = 71] = B0 ; RD_CLT_0_SENS_0
    static_cast<uint8_t>(0xF2U), static_cast<uint8_t>(0x48U), // RD: [reg_rx_data           = 72] = 48 ; RD_CLT_0_SENS_0: 48
    // ----------------------------------------------------------------
    static_cast<uint8_t>(0x71U), static_cast<uint8_t>(0xD0U), // WR: [reg_access            = 71] = D0 ; RD_CLT_0_SENS_0: EoT
    // ----------------------------------------------------------------

    // -------- Preparation of measurement loop: ------------------------------------------------------

    static_cast<uint8_t>(0x03U), static_cast<uint8_t>(0x16U), // WR: [reg_tread_sensor      = 03] = 16 
    static_cast<uint8_t>(0x01U), static_cast<uint8_t>(0x25U), // WR: [reg_tloop_period      = 01] = 25 
    static_cast<uint8_t>(0x02U), static_cast<uint8_t>(0x01U), // WR: [reg_en_client         = 02] = 01 
    static_cast<uint8_t>(0x7EU), static_cast<uint8_t>(0x4BU), // WR: [reg_key               = 7E] = 4B 
    static_cast<uint8_t>(0x7FU), static_cast<uint8_t>(0x00U), // WR: [reg_page_select       = 7F] = 00 Normal_Page
    // ================================================================
    SSBAL_CFG_DAT_END  // Only once after the configs of all cfg_idx
    // ----------------------------------------------------------------
};

#elif defined SSBCFG_HUMID_BEA0
    #error "SSB-Error 3a: Humidity sensor not yet supported in ssb_config_auto.cpp"

#else
    #error "SSB-Error 3: Switch SSBCFG_MAIN__... or SSBCFG_... missed in ssb_project_cfg.h"
#endif

const struct SsbExplictiteConfigurationParameters_s
SsbConfigurations_c::SsbExplictiteConfigurationParameters[SSB_MAX_NUMBER_OF_CLIENTS] =
{
#if SSBF_MNGR_NUMBER_OF_HUBS >= 1U
    {SSBF_MNGR_CLIENT_PLACEMENT_MASK_HUB_0,
     SSBAL_DURATION_LOOP_PERIOD_PER_TIMER_IN_MS_HUB_0}
#endif
#if SSBF_MNGR_NUMBER_OF_HUBS >= 2U
    ,
    {SSBF_MNGR_CLIENT_PLACEMENT_MASK_HUB_1,
     SSBAL_DURATION_LOOP_PERIOD_PER_TIMER_IN_MS_HUB_1}
#endif
#if SSBF_MNGR_NUMBER_OF_HUBS >= 3U
    ,
    {SSBF_MNGR_CLIENT_PLACEMENT_MASK_HUB_2,
     SSBAL_DURATION_LOOP_PERIOD_PER_TIMER_IN_MS_HUB_2}
#endif
#if SSBF_MNGR_NUMBER_OF_HUBS == 4U
    ,
    {SSBF_MNGR_CLIENT_PLACEMENT_MASK_HUB_3,
     SSBAL_DURATION_LOOP_PERIOD_PER_TIMER_IN_MS_HUB_3}
#endif
};
#if SSBF_MNGR_NUMBER_OF_HUBS > 4U
    #error SSBF_MNGR_NUMBER_OF_HUBS too big
#endif
#ifndef SSBF_MNGR_NUMBER_OF_HUBS
    #error SSBF_MNGR_NUMBER_OF_HUBS not defined
#endif

/*lint -e40 @@ */

