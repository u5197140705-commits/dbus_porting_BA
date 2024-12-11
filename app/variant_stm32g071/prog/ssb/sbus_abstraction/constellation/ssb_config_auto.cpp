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
    #define SSBAL_DURATION_LOOP_PERIOD_PER_TIMER_IN_MS_HUB_0  (STIM_TIME_MS((uint32_t)1000U))
    #define SSBF_MNGR_CLIENT_PLACEMENT_MASK_HUB_0             ((uint8_t)0x0F)
                                      // 0x00,      0x01,     0x02, 0x04, 0x08
                                      // No Client, Client 0, ...         Client 3
#endif
#if SSBF_MNGR_NUMBER_OF_HUBS >= 2U
    #define SSBAL_DURATION_LOOP_PERIOD_PER_TIMER_IN_MS_HUB_1  (STIM_TIME_MS((uint32_t)1000U))
    #define SSBF_MNGR_CLIENT_PLACEMENT_MASK_HUB_1             ((uint8_t)0x00)
#endif
#if SSBF_MNGR_NUMBER_OF_HUBS >= 3U
    #define SSBAL_DURATION_LOOP_PERIOD_PER_TIMER_IN_MS_HUB_2  (STIM_TIME_MS((uint32_t)1000U))
    #define SSBF_MNGR_CLIENT_PLACEMENT_MASK_HUB_2             ((uint8_t)0x00)
#endif
#if SSBF_MNGR_NUMBER_OF_HUBS == 4U
    #define SSBAL_DURATION_LOOP_PERIOD_PER_TIMER_IN_MS_HUB_3  (STIM_TIME_MS((uint32_t)1000U))
    #define SSBF_MNGR_CLIENT_PLACEMENT_MASK_HUB_3             ((uint8_t)0x00)
#endif
#if SSBF_MNGR_NUMBER_OF_HUBS > 4U
    #error SSBF_MNGR_NUMBER_OF_HUBS too big
#endif
#ifndef SSBF_MNGR_NUMBER_OF_HUBS
    #error SSBF_MNGR_NUMBER_OF_HUBS not defined
#endif

using namespace ::SSBAL::SSBCO;

const uint8_t
SsbConfigurations_c::SsbConfigurationBytes[SSB_NUMBER_OF_CONFIGURATION_BYTES] =
{
    // ================================================================
    SSBAL_CFG_DAT_ITEM_BEGIN,
    0x33U, 0x11U, // <cfg_idx_high>, <cfg_idx_low>,
    // ----------------------------------------------------------------

    //-------- Startup of Hubs and Clients: -----------------------------------------------------------

    0x7FU, 0x00U, // WR: [reg_page_select       = 7F] = 00 Normal_Page
    0x6EU, 0x00U, // WR: [reg_loop              = 6E] = 00 Stopping the loop
    // ----------------------------------------------------------------
    0x88U, 0x10U, // RD: [reg_int_hub           = 08] = 10 
    0x73U, 0x01U, // WR: [reg_res_hub           = 73] = 01 
    0x88U, 0x10U, // RD: [reg_int_hub           = 08] = 10 Reset Hub

    0x73U, 0x02U, // WR: [reg_res_hub           = 73] = 02 Power for all Clients off (sleep-bit)
    0x88U, 0x10U, // RD: [reg_int_hub           = 08] = 10 

    0x73U, 0x01U, // WR: [reg_res_hub           = 73] = 01 Power for all Clients on and reset Hub
    0x88U, 0x10U, // RD: [reg_int_hub           = 08] = 10 

    0x7EU, 0x4BU, // WR: [reg_key               = 7E] = 4B 
    // ----------------------------------------------------------------
    0x6DU, 0x01U, // WR: [reg_pwr_res           = 6D] = 01 
    // ----------------------------------------------------------------
    0x6FU, 0x70U, // WR: [reg_addr              = 6F] = 70 ; RD_CLT_0
    0x71U, 0x98U, // WR: [reg_access            = 71] = 98 ; RD_CLT_0
    0xF2U, 0x01U, // RD: [reg_rx_data           = 72] = 01 ; RD_CLT_0: [reg_clt_status        = 70] = 01 
    // ----------------------------------------------------------------
    0x6FU, 0x7EU, // WR: [reg_addr              = 6F] = 7E ; WR_CLT_0
    0x70U, 0x4BU, // WR: [reg_tx_data           = 70] = 4B ; WR_CLT_0
    0x71U, 0x88U, // WR: [reg_access            = 71] = 88 ; WR_CLT_0: [reg_key               = 7E] = 4B 
    // ----------------------------------------------------------------
    0x6FU, 0x7FU, // WR: [reg_addr              = 6F] = 7F ; WR_CLT_0
    0x70U, 0x00U, // WR: [reg_tx_data           = 70] = 00 ; WR_CLT_0
    0x71U, 0x88U, // WR: [reg_access            = 71] = 88 ; WR_CLT_0: [reg_page_select       = 7F] = 00 Normal_Page_Clt
    // ----------------------------------------------------------------
    0x6DU, 0x02U, // WR: [reg_pwr_res           = 6D] = 02 
    // ----------------------------------------------------------------
    0x6FU, 0x70U, // WR: [reg_addr              = 6F] = 70 ; RD_CLT_1
    0x71U, 0x99U, // WR: [reg_access            = 71] = 99 ; RD_CLT_1
    0xF2U, 0x01U, // RD: [reg_rx_data           = 72] = 01 ; RD_CLT_1: [reg_clt_status        = 70] = 01 
    // ----------------------------------------------------------------
    0x6FU, 0x7EU, // WR: [reg_addr              = 6F] = 7E ; WR_CLT_1
    0x70U, 0x4BU, // WR: [reg_tx_data           = 70] = 4B ; WR_CLT_1
    0x71U, 0x89U, // WR: [reg_access            = 71] = 89 ; WR_CLT_1: [reg_key               = 7E] = 4B 
    // ----------------------------------------------------------------
    0x6FU, 0x7FU, // WR: [reg_addr              = 6F] = 7F ; WR_CLT_1
    0x70U, 0x00U, // WR: [reg_tx_data           = 70] = 00 ; WR_CLT_1
    0x71U, 0x89U, // WR: [reg_access            = 71] = 89 ; WR_CLT_1: [reg_page_select       = 7F] = 00 Normal_Page_Clt
    // ----------------------------------------------------------------
    0x6DU, 0x04U, // WR: [reg_pwr_res           = 6D] = 04 
    // ----------------------------------------------------------------
    0x6FU, 0x70U, // WR: [reg_addr              = 6F] = 70 ; RD_CLT_2
    0x71U, 0x9AU, // WR: [reg_access            = 71] = 9A ; RD_CLT_2
    0xF2U, 0x01U, // RD: [reg_rx_data           = 72] = 01 ; RD_CLT_2: [reg_clt_status        = 70] = 01 
    // ----------------------------------------------------------------
    0x6FU, 0x7EU, // WR: [reg_addr              = 6F] = 7E ; WR_CLT_2
    0x70U, 0x4BU, // WR: [reg_tx_data           = 70] = 4B ; WR_CLT_2
    0x71U, 0x8AU, // WR: [reg_access            = 71] = 8A ; WR_CLT_2: [reg_key               = 7E] = 4B 
    // ----------------------------------------------------------------
    0x6FU, 0x7FU, // WR: [reg_addr              = 6F] = 7F ; WR_CLT_2
    0x70U, 0x00U, // WR: [reg_tx_data           = 70] = 00 ; WR_CLT_2
    0x71U, 0x8AU, // WR: [reg_access            = 71] = 8A ; WR_CLT_2: [reg_page_select       = 7F] = 00 Normal_Page_Clt
    // ----------------------------------------------------------------
    0x6DU, 0x08U, // WR: [reg_pwr_res           = 6D] = 08 
    // ----------------------------------------------------------------
    0x6FU, 0x70U, // WR: [reg_addr              = 6F] = 70 ; RD_CLT_3
    0x71U, 0x9BU, // WR: [reg_access            = 71] = 9B ; RD_CLT_3
    0xF2U, 0x01U, // RD: [reg_rx_data           = 72] = 01 ; RD_CLT_3: [reg_clt_status        = 70] = 01 
    // ----------------------------------------------------------------
    0x6FU, 0x7EU, // WR: [reg_addr              = 6F] = 7E ; WR_CLT_3
    0x70U, 0x4BU, // WR: [reg_tx_data           = 70] = 4B ; WR_CLT_3
    0x71U, 0x8BU, // WR: [reg_access            = 71] = 8B ; WR_CLT_3: [reg_key               = 7E] = 4B 
    // ----------------------------------------------------------------
    0x6FU, 0x7FU, // WR: [reg_addr              = 6F] = 7F ; WR_CLT_3
    0x70U, 0x00U, // WR: [reg_tx_data           = 70] = 00 ; WR_CLT_3
    0x71U, 0x8BU, // WR: [reg_access            = 71] = 8B ; WR_CLT_3: [reg_page_select       = 7F] = 00 Normal_Page_Clt
    // ----------------------------------------------------------------
    0x88U, 0x0FU, // RD: [reg_int_hub           = 08] = 0F 
    0x89U, 0x20U, // RD: [reg_int_clt0          = 09] = 20 
    0x8AU, 0x20U, // RD: [reg_int_clt1          = 0A] = 20 
    0x8BU, 0x20U, // RD: [reg_int_clt2          = 0B] = 20 
    0x8CU, 0x20U, // RD: [reg_int_clt3          = 0C] = 20 
    // ----------------------------------------------------------------

    //-------- Configuration of Clients and Devices: --------------------------------------------------

    0x6FU, 0x7EU, // WR: [reg_addr              = 6F] = 7E ; WR_CLT_0
    0x70U, 0x4BU, // WR: [reg_tx_data           = 70] = 4B ; WR_CLT_0
    0x71U, 0x88U, // WR: [reg_access            = 71] = 88 ; WR_CLT_0: [reg_key               = 7E] = 4B 
    // ----------------------------------------------------------------
    0x6FU, 0x75U, // WR: [reg_addr              = 6F] = 75 ; WR_CLT_0
    0x70U, 0x07U, // WR: [reg_tx_data           = 70] = 07 ; WR_CLT_0
    0x71U, 0x88U, // WR: [reg_access            = 71] = 88 ; WR_CLT_0: [reg_temp_cfg          = 75] = 07 
    // ----------------------------------------------------------------
    0x6FU, 0x71U, // WR: [reg_addr              = 6F] = 71 ; WR_CLT_0
    0x70U, 0x01U, // WR: [reg_tx_data           = 70] = 01 ; WR_CLT_0
    0x71U, 0x88U, // WR: [reg_access            = 71] = 88 ; WR_CLT_0: [reg_pwr_sensor        = 71] = 01 
    // ----------------------------------------------------------------
    0x6FU, 0x71U, // WR: [reg_addr              = 6F] = 71 ; RD_CLT_0
    0x71U, 0x98U, // WR: [reg_access            = 71] = 98 ; RD_CLT_0
    0xF2U, 0x01U, // RD: [reg_rx_data           = 72] = 01 ; RD_CLT_0: [reg_pwr_sensor        = 71] = 01 
    // ----------------------------------------------------------------
    0x70U, 0x11U, // WR: [reg_tx_data           = 70] = 11 ; WR_CLT_0_SENS_0
    0x71U, 0x80U, // WR: [reg_access            = 71] = 80 ; WR_CLT_0_SENS_0: 11
    // ----------------------------------------------------------------
    0x70U, 0x4CU, // WR: [reg_tx_data           = 70] = 4C ; WR_CLT_0_SENS_0
    0x71U, 0x80U, // WR: [reg_access            = 71] = 80 ; WR_CLT_0_SENS_0: 4C
    // ----------------------------------------------------------------
    0x71U, 0xC0U, // WR: [reg_access            = 71] = C0 ; WR_CLT_0_SENS_0: EoT
    // ----------------------------------------------------------------
    0x70U, 0x91U, // WR: [reg_tx_data           = 70] = 91 ; WR_CLT_0_SENS_0
    0x71U, 0x80U, // WR: [reg_access            = 71] = 80 ; WR_CLT_0_SENS_0: 91
    // ----------------------------------------------------------------
    0x71U, 0xB0U, // WR: [reg_access            = 71] = B0 ; RD_CLT_0_SENS_0
    0xF2U, 0x4CU, // RD: [reg_rx_data           = 72] = 4C ; RD_CLT_0_SENS_0: 4C
    // ----------------------------------------------------------------
    0x71U, 0xD0U, // WR: [reg_access            = 71] = D0 ; RD_CLT_0_SENS_0: EoT
    // ----------------------------------------------------------------
    0x70U, 0x10U, // WR: [reg_tx_data           = 70] = 10 ; WR_CLT_0_SENS_0
    0x71U, 0x80U, // WR: [reg_access            = 71] = 80 ; WR_CLT_0_SENS_0: 10
    // ----------------------------------------------------------------
    0x70U, 0x48U, // WR: [reg_tx_data           = 70] = 48 ; WR_CLT_0_SENS_0
    0x71U, 0x80U, // WR: [reg_access            = 71] = 80 ; WR_CLT_0_SENS_0: 48
    // ----------------------------------------------------------------
    0x71U, 0xC0U, // WR: [reg_access            = 71] = C0 ; WR_CLT_0_SENS_0: EoT
    // ----------------------------------------------------------------
    0x70U, 0x90U, // WR: [reg_tx_data           = 70] = 90 ; WR_CLT_0_SENS_0
    0x71U, 0x80U, // WR: [reg_access            = 71] = 80 ; WR_CLT_0_SENS_0: 90
    // ----------------------------------------------------------------
    0x71U, 0xB0U, // WR: [reg_access            = 71] = B0 ; RD_CLT_0_SENS_0
    0xF2U, 0x48U, // RD: [reg_rx_data           = 72] = 48 ; RD_CLT_0_SENS_0: 48
    // ----------------------------------------------------------------
    0x71U, 0xD0U, // WR: [reg_access            = 71] = D0 ; RD_CLT_0_SENS_0: EoT
    // ----------------------------------------------------------------
    0x6FU, 0x7EU, // WR: [reg_addr              = 6F] = 7E ; WR_CLT_1
    0x70U, 0x4BU, // WR: [reg_tx_data           = 70] = 4B ; WR_CLT_1
    0x71U, 0x89U, // WR: [reg_access            = 71] = 89 ; WR_CLT_1: [reg_key               = 7E] = 4B 
    // ----------------------------------------------------------------
    0x6FU, 0x75U, // WR: [reg_addr              = 6F] = 75 ; WR_CLT_1
    0x70U, 0x07U, // WR: [reg_tx_data           = 70] = 07 ; WR_CLT_1
    0x71U, 0x89U, // WR: [reg_access            = 71] = 89 ; WR_CLT_1: [reg_temp_cfg          = 75] = 07 
    // ----------------------------------------------------------------
    0x6FU, 0x71U, // WR: [reg_addr              = 6F] = 71 ; WR_CLT_1
    0x70U, 0x01U, // WR: [reg_tx_data           = 70] = 01 ; WR_CLT_1
    0x71U, 0x89U, // WR: [reg_access            = 71] = 89 ; WR_CLT_1: [reg_pwr_sensor        = 71] = 01 
    // ----------------------------------------------------------------
    0x6FU, 0x71U, // WR: [reg_addr              = 6F] = 71 ; RD_CLT_1
    0x71U, 0x99U, // WR: [reg_access            = 71] = 99 ; RD_CLT_1
    0xF2U, 0x01U, // RD: [reg_rx_data           = 72] = 01 ; RD_CLT_1: [reg_pwr_sensor        = 71] = 01 
    // ----------------------------------------------------------------
    0x70U, 0x11U, // WR: [reg_tx_data           = 70] = 11 ; WR_CLT_1_SENS_0
    0x71U, 0x81U, // WR: [reg_access            = 71] = 81 ; WR_CLT_1_SENS_0: 11
    // ----------------------------------------------------------------
    0x70U, 0x4CU, // WR: [reg_tx_data           = 70] = 4C ; WR_CLT_1_SENS_0
    0x71U, 0x81U, // WR: [reg_access            = 71] = 81 ; WR_CLT_1_SENS_0: 4C
    // ----------------------------------------------------------------
    0x71U, 0xC1U, // WR: [reg_access            = 71] = C1 ; WR_CLT_1_SENS_0: EoT
    // ----------------------------------------------------------------
    0x70U, 0x91U, // WR: [reg_tx_data           = 70] = 91 ; WR_CLT_1_SENS_0
    0x71U, 0x81U, // WR: [reg_access            = 71] = 81 ; WR_CLT_1_SENS_0: 91
    // ----------------------------------------------------------------
    0x71U, 0xB1U, // WR: [reg_access            = 71] = B1 ; RD_CLT_1_SENS_0
    0xF2U, 0x4CU, // RD: [reg_rx_data           = 72] = 4C ; RD_CLT_1_SENS_0: 4C
    // ----------------------------------------------------------------
    0x71U, 0xD1U, // WR: [reg_access            = 71] = D1 ; RD_CLT_1_SENS_0: EoT
    // ----------------------------------------------------------------
    0x70U, 0x10U, // WR: [reg_tx_data           = 70] = 10 ; WR_CLT_1_SENS_0
    0x71U, 0x81U, // WR: [reg_access            = 71] = 81 ; WR_CLT_1_SENS_0: 10
    // ----------------------------------------------------------------
    0x70U, 0x48U, // WR: [reg_tx_data           = 70] = 48 ; WR_CLT_1_SENS_0
    0x71U, 0x81U, // WR: [reg_access            = 71] = 81 ; WR_CLT_1_SENS_0: 48
    // ----------------------------------------------------------------
    0x71U, 0xC1U, // WR: [reg_access            = 71] = C1 ; WR_CLT_1_SENS_0: EoT
    // ----------------------------------------------------------------
    0x70U, 0x90U, // WR: [reg_tx_data           = 70] = 90 ; WR_CLT_1_SENS_0
    0x71U, 0x81U, // WR: [reg_access            = 71] = 81 ; WR_CLT_1_SENS_0: 90
    // ----------------------------------------------------------------
    0x71U, 0xB1U, // WR: [reg_access            = 71] = B1 ; RD_CLT_1_SENS_0
    0xF2U, 0x48U, // RD: [reg_rx_data           = 72] = 48 ; RD_CLT_1_SENS_0: 48
    // ----------------------------------------------------------------
    0x71U, 0xD1U, // WR: [reg_access            = 71] = D1 ; RD_CLT_1_SENS_0: EoT
    // ----------------------------------------------------------------
    0x6FU, 0x7EU, // WR: [reg_addr              = 6F] = 7E ; WR_CLT_2
    0x70U, 0x4BU, // WR: [reg_tx_data           = 70] = 4B ; WR_CLT_2
    0x71U, 0x8AU, // WR: [reg_access            = 71] = 8A ; WR_CLT_2: [reg_key               = 7E] = 4B 
    // ----------------------------------------------------------------
    0x6FU, 0x75U, // WR: [reg_addr              = 6F] = 75 ; WR_CLT_2
    0x70U, 0x07U, // WR: [reg_tx_data           = 70] = 07 ; WR_CLT_2
    0x71U, 0x8AU, // WR: [reg_access            = 71] = 8A ; WR_CLT_2: [reg_temp_cfg          = 75] = 07 
    // ----------------------------------------------------------------
    0x6FU, 0x71U, // WR: [reg_addr              = 6F] = 71 ; WR_CLT_2
    0x70U, 0x01U, // WR: [reg_tx_data           = 70] = 01 ; WR_CLT_2
    0x71U, 0x8AU, // WR: [reg_access            = 71] = 8A ; WR_CLT_2: [reg_pwr_sensor        = 71] = 01 
    // ----------------------------------------------------------------
    0x6FU, 0x71U, // WR: [reg_addr              = 6F] = 71 ; RD_CLT_2
    0x71U, 0x9AU, // WR: [reg_access            = 71] = 9A ; RD_CLT_2
    0xF2U, 0x01U, // RD: [reg_rx_data           = 72] = 01 ; RD_CLT_2: [reg_pwr_sensor        = 71] = 01 
    // ----------------------------------------------------------------
    0x70U, 0x11U, // WR: [reg_tx_data           = 70] = 11 ; WR_CLT_2_SENS_0
    0x71U, 0x82U, // WR: [reg_access            = 71] = 82 ; WR_CLT_2_SENS_0: 11
    // ----------------------------------------------------------------
    0x70U, 0x4CU, // WR: [reg_tx_data           = 70] = 4C ; WR_CLT_2_SENS_0
    0x71U, 0x82U, // WR: [reg_access            = 71] = 82 ; WR_CLT_2_SENS_0: 4C
    // ----------------------------------------------------------------
    0x71U, 0xC2U, // WR: [reg_access            = 71] = C2 ; WR_CLT_2_SENS_0: EoT
    // ----------------------------------------------------------------
    0x70U, 0x91U, // WR: [reg_tx_data           = 70] = 91 ; WR_CLT_2_SENS_0
    0x71U, 0x82U, // WR: [reg_access            = 71] = 82 ; WR_CLT_2_SENS_0: 91
    // ----------------------------------------------------------------
    0x71U, 0xB2U, // WR: [reg_access            = 71] = B2 ; RD_CLT_2_SENS_0
    0xF2U, 0x4CU, // RD: [reg_rx_data           = 72] = 4C ; RD_CLT_2_SENS_0: 4C
    // ----------------------------------------------------------------
    0x71U, 0xD2U, // WR: [reg_access            = 71] = D2 ; RD_CLT_2_SENS_0: EoT
    // ----------------------------------------------------------------
    0x70U, 0x10U, // WR: [reg_tx_data           = 70] = 10 ; WR_CLT_2_SENS_0
    0x71U, 0x82U, // WR: [reg_access            = 71] = 82 ; WR_CLT_2_SENS_0: 10
    // ----------------------------------------------------------------
    0x70U, 0x48U, // WR: [reg_tx_data           = 70] = 48 ; WR_CLT_2_SENS_0
    0x71U, 0x82U, // WR: [reg_access            = 71] = 82 ; WR_CLT_2_SENS_0: 48
    // ----------------------------------------------------------------
    0x71U, 0xC2U, // WR: [reg_access            = 71] = C2 ; WR_CLT_2_SENS_0: EoT
    // ----------------------------------------------------------------
    0x70U, 0x90U, // WR: [reg_tx_data           = 70] = 90 ; WR_CLT_2_SENS_0
    0x71U, 0x82U, // WR: [reg_access            = 71] = 82 ; WR_CLT_2_SENS_0: 90
    // ----------------------------------------------------------------
    0x71U, 0xB2U, // WR: [reg_access            = 71] = B2 ; RD_CLT_2_SENS_0
    0xF2U, 0x48U, // RD: [reg_rx_data           = 72] = 48 ; RD_CLT_2_SENS_0: 48
    // ----------------------------------------------------------------
    0x71U, 0xD2U, // WR: [reg_access            = 71] = D2 ; RD_CLT_2_SENS_0: EoT
    // ----------------------------------------------------------------
    0x6FU, 0x7EU, // WR: [reg_addr              = 6F] = 7E ; WR_CLT_3
    0x70U, 0x4BU, // WR: [reg_tx_data           = 70] = 4B ; WR_CLT_3
    0x71U, 0x8BU, // WR: [reg_access            = 71] = 8B ; WR_CLT_3: [reg_key               = 7E] = 4B 
    // ----------------------------------------------------------------
    0x6FU, 0x75U, // WR: [reg_addr              = 6F] = 75 ; WR_CLT_3
    0x70U, 0x07U, // WR: [reg_tx_data           = 70] = 07 ; WR_CLT_3
    0x71U, 0x8BU, // WR: [reg_access            = 71] = 8B ; WR_CLT_3: [reg_temp_cfg          = 75] = 07 
    // ----------------------------------------------------------------
    0x6FU, 0x71U, // WR: [reg_addr              = 6F] = 71 ; WR_CLT_3
    0x70U, 0x01U, // WR: [reg_tx_data           = 70] = 01 ; WR_CLT_3
    0x71U, 0x8BU, // WR: [reg_access            = 71] = 8B ; WR_CLT_3: [reg_pwr_sensor        = 71] = 01 
    // ----------------------------------------------------------------
    0x6FU, 0x71U, // WR: [reg_addr              = 6F] = 71 ; RD_CLT_3
    0x71U, 0x9BU, // WR: [reg_access            = 71] = 9B ; RD_CLT_3
    0xF2U, 0x01U, // RD: [reg_rx_data           = 72] = 01 ; RD_CLT_3: [reg_pwr_sensor        = 71] = 01 
    // ----------------------------------------------------------------
    0x70U, 0x11U, // WR: [reg_tx_data           = 70] = 11 ; WR_CLT_3_SENS_0
    0x71U, 0x83U, // WR: [reg_access            = 71] = 83 ; WR_CLT_3_SENS_0: 11
    // ----------------------------------------------------------------
    0x70U, 0x4CU, // WR: [reg_tx_data           = 70] = 4C ; WR_CLT_3_SENS_0
    0x71U, 0x83U, // WR: [reg_access            = 71] = 83 ; WR_CLT_3_SENS_0: 4C
    // ----------------------------------------------------------------
    0x71U, 0xC3U, // WR: [reg_access            = 71] = C3 ; WR_CLT_3_SENS_0: EoT
    // ----------------------------------------------------------------
    0x70U, 0x91U, // WR: [reg_tx_data           = 70] = 91 ; WR_CLT_3_SENS_0
    0x71U, 0x83U, // WR: [reg_access            = 71] = 83 ; WR_CLT_3_SENS_0: 91
    // ----------------------------------------------------------------
    0x71U, 0xB3U, // WR: [reg_access            = 71] = B3 ; RD_CLT_3_SENS_0
    0xF2U, 0x4CU, // RD: [reg_rx_data           = 72] = 4C ; RD_CLT_3_SENS_0: 4C
    // ----------------------------------------------------------------
    0x71U, 0xD3U, // WR: [reg_access            = 71] = D3 ; RD_CLT_3_SENS_0: EoT
    // ----------------------------------------------------------------
    0x70U, 0x10U, // WR: [reg_tx_data           = 70] = 10 ; WR_CLT_3_SENS_0
    0x71U, 0x83U, // WR: [reg_access            = 71] = 83 ; WR_CLT_3_SENS_0: 10
    // ----------------------------------------------------------------
    0x70U, 0x48U, // WR: [reg_tx_data           = 70] = 48 ; WR_CLT_3_SENS_0
    0x71U, 0x83U, // WR: [reg_access            = 71] = 83 ; WR_CLT_3_SENS_0: 48
    // ----------------------------------------------------------------
    0x71U, 0xC3U, // WR: [reg_access            = 71] = C3 ; WR_CLT_3_SENS_0: EoT
    // ----------------------------------------------------------------
    0x70U, 0x90U, // WR: [reg_tx_data           = 70] = 90 ; WR_CLT_3_SENS_0
    0x71U, 0x83U, // WR: [reg_access            = 71] = 83 ; WR_CLT_3_SENS_0: 90
    // ----------------------------------------------------------------
    0x71U, 0xB3U, // WR: [reg_access            = 71] = B3 ; RD_CLT_3_SENS_0
    0xF2U, 0x48U, // RD: [reg_rx_data           = 72] = 48 ; RD_CLT_3_SENS_0: 48
    // ----------------------------------------------------------------
    0x71U, 0xD3U, // WR: [reg_access            = 71] = D3 ; RD_CLT_3_SENS_0: EoT
    // ----------------------------------------------------------------

    // -------- Preparation of measurement loop: ------------------------------------------------------

    0x03U, 0x16U, // WR: [reg_tread_sensor      = 03] = 16 
    0x01U, 0x25U, // WR: [reg_tloop_period      = 01] = 25 
    0x02U, 0x0FU, // WR: [reg_en_client         = 02] = 0F 
    0x7EU, 0x4BU, // WR: [reg_key               = 7E] = 4B 
    0x7FU, 0x00U, // WR: [reg_page_select       = 7F] = 00 Normal_Page
    // ================================================================
    SSBAL_CFG_DAT_END  // Only once after the configs of all cfg_idx
    // ----------------------------------------------------------------
};

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

