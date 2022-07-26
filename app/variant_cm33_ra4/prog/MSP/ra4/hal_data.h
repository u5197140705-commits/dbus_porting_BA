/* generated HAL header file - do not edit */
#ifndef HAL_DATA_H_
#define HAL_DATA_H_
#include <stdint.h>
#include "bsp_api.h"
#include "common_data.h"
#include "r_sci_uart.h"
#include "r_uart_api.h"
#include "r_gpt.h"
#include "r_agt.h"

FSP_HEADER
/** UART on SCI Instance. */
extern const uart_instance_t g_uart0;
extern const uart_instance_t g_uart2;

/** Access the UART instance using these structures when calling API functions directly (::p_api is not used). */
extern sci_uart_instance_ctrl_t g_uart0_ctrl;
extern sci_uart_instance_ctrl_t g_uart2_ctrl;
extern const uart_cfg_t g_uart0_cfg;
extern const uart_cfg_t g_uart2_cfg;
extern const sci_uart_extended_cfg_t g_uart0_cfg_extend;
extern const sci_uart_extended_cfg_t g_uart2_cfg_extend;
extern baud_setting_t g_uart0_baud_setting;
extern baud_setting_t g_uart2_baud_setting;

/** Access the GPT timer instance */
extern gpt_instance_ctrl_t g_timerGPT7_ctrl;
extern const gpt_extended_cfg_t g_timerGPT7_extend;
extern const timer_cfg_t g_timerGPT7_cfg;

/** Access the AGT timer instance */
extern agt_instance_ctrl_t timerAGT0_ctrl;
#if defined (VARIANT_ROMLOADER) || defined (VARIANT_LROMLOADER)
extern const timer_cfg_t timerAGT0_cfg;
#else
extern timer_cfg_t timerAGT0_cfg;
#endif //VARIANT_ROMLOADER or VARIANT_LROMLOADER 

#ifndef user_uart_callback
void user_uart_callback(uart_callback_args_t *p_args);
#endif
void r_sci_uart_basic_example (void);

void hal_entry(void);
void g_hal_init(void);
FSP_FOOTER
#endif /* HAL_DATA_H_ */
