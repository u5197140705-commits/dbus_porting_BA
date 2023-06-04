/* generated pin source file - do not edit */
#include "bsh_stdinc.h"
#include "bsp_api.h"
#include "r_ioport_api.h"

const ioport_pin_cfg_t g_bsp_pin_cfg_data[] =
        {
            /* UART0  - SCI0 */
            #if defined (HUART0_RX_P100) && defined (HUART0_TX_P101)
            { .pin = BSP_IO_PORT_01_PIN_00, .pin_cfg = ((uint32_t) IOPORT_CFG_PERIPHERAL_PIN | (uint32_t) IOPORT_PERIPHERAL_SCI0_2_4_6_8) },
            { .pin = BSP_IO_PORT_01_PIN_01, .pin_cfg = ((uint32_t) IOPORT_CFG_PERIPHERAL_PIN | (uint32_t) IOPORT_PERIPHERAL_SCI0_2_4_6_8) },
            #endif
            /* UART2  - SCI2 */
            #if defined (HUART2_RX_P301) && defined (HUART2_TX_P302)
            { .pin = BSP_IO_PORT_03_PIN_01, .pin_cfg = ((uint32_t) IOPORT_CFG_PERIPHERAL_PIN | (uint32_t) IOPORT_PERIPHERAL_SCI0_2_4_6_8) },
            { .pin = BSP_IO_PORT_03_PIN_02, .pin_cfg = ((uint32_t) IOPORT_CFG_PERIPHERAL_PIN | (uint32_t) IOPORT_PERIPHERAL_SCI0_2_4_6_8) },
            #endif
            /* UART0  - SCI0 */
            #if defined (HUART0_RX_P410) && defined (HUART0_TX_P411)
            { .pin = BSP_IO_PORT_04_PIN_10, .pin_cfg = ((uint32_t) IOPORT_CFG_PERIPHERAL_PIN | (uint32_t) IOPORT_PERIPHERAL_SCI0_2_4_6_8) },
            { .pin = BSP_IO_PORT_04_PIN_11, .pin_cfg = ((uint32_t) IOPORT_CFG_PERIPHERAL_PIN | (uint32_t) IOPORT_PERIPHERAL_SCI0_2_4_6_8) }
            #endif
        };

const ioport_cfg_t g_bsp_pin_cfg =
{ .number_of_pins = (uint16_t)(sizeof(g_bsp_pin_cfg_data) / sizeof(ioport_pin_cfg_t)), .p_pin_cfg_data = &g_bsp_pin_cfg_data[0], };

#if BSP_TZ_SECURE_BUILD

void R_BSP_PinCfgSecurityInit(void);

/* Initialize SAR registers for secure pins. */
void R_BSP_PinCfgSecurityInit(void)
{
 #if (2U == BSP_FEATURE_IOPORT_VERSION)
    uint32_t pmsar[BSP_FEATURE_BSP_NUM_PMSAR];
 #else
    uint16_t pmsar[BSP_FEATURE_BSP_NUM_PMSAR];
 #endif
    memset(pmsar, 0xFF, BSP_FEATURE_BSP_NUM_PMSAR * sizeof(R_PMISC->PMSAR[0]));


    for(uint32_t i = 0; i < g_bsp_pin_cfg.number_of_pins; i++)
    {
        uint32_t port_pin = g_bsp_pin_cfg.p_pin_cfg_data[i].pin;
        uint32_t port = port_pin >> 8U;
        uint32_t pin = port_pin & 0xFFU;
        pmsar[port] &= (uint16_t) ~(1U << pin);
    }

    for(uint32_t i = 0; i < BSP_FEATURE_BSP_NUM_PMSAR; i++)
    {
 #if (2U == BSP_FEATURE_IOPORT_VERSION)
        R_PMISC->PMSAR[i].PMSAR = (uint16_t) pmsar[i];
 #else
        R_PMISC->PMSAR[i].PMSAR = pmsar[i];
 #endif
    }

}
#endif
