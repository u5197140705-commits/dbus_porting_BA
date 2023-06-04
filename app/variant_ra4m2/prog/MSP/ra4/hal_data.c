/* generated HAL source file - do not edit */
#include "bsh_stdinc.h"
#include "hal_data.h"


/* Instance structure to use this module. */
const uart_instance_t g_uart0 =
{
    .p_ctrl = &g_uart0_ctrl,
    .p_cfg = &g_uart0_cfg,
    .p_api = &g_uart_on_sci
};

sci_uart_instance_ctrl_t g_uart0_ctrl;

baud_setting_t g_uart0_baud_setting =
{
    /* Baud rate calculated with 0.469% error. */
    {
        .semr_bits = {
                    .brme  = 0,
                    .abcse = 0,          //  bit of SEMR register
                    .abcs  = 0,          //  bit of SEMR register
                    .bgdm  = 1           //  bit of SEMR register
                     }
    },
    .cks = 0,           // PCLKA (64MHz) is not divided (input clock for SCI, n=0)
    .brr = 207U,        // calculated outside of MCU Framework to get 9600 bauds
    .mddr = (uint8_t) 256
};

/** UART extended configuration for UARTonSCI HAL driver */
const sci_uart_extended_cfg_t g_uart0_cfg_extend =
{
    .clock           = SCI_UART_CLOCK_INT,
    .rx_edge_start   = SCI_UART_START_BIT_FALLING_EDGE,
    .noise_cancel    = SCI_UART_NOISE_CANCELLATION_DISABLE,
    .rx_fifo_trigger = SCI_UART_RX_FIFO_TRIGGER_MAX,
    .p_baud_setting  = &g_uart0_baud_setting,
    .flow_control    = SCI_UART_FLOW_CONTROL_RTS,
    .flow_control_pin = BSP_IO_PORT_UNDEFINED
};

/** UART interface configuration */
const uart_cfg_t g_uart0_cfg =
{
    .channel = 0,
    .data_bits  = UART_DATA_BITS_8,
    .parity     = UART_PARITY_OFF,
    .stop_bits  = UART_STOP_BITS_1,
    .p_callback = user_uart_callback,
    .p_context  = NULL,
    .p_extend   = &g_uart0_cfg_extend,
    .p_transfer_tx = NULL,
    .p_transfer_rx = NULL,

    .rxi_ipl = (12),
    .txi_ipl = (12),
    .tei_ipl = (12),
    .eri_ipl = (12),
#if defined(VECTOR_NUMBER_SCI0_RXI)
    .rxi_irq = VECTOR_NUMBER_SCI0_RXI,
#else
    .rxi_irq = FSP_INVALID_VECTOR,
#endif

#if defined(VECTOR_NUMBER_SCI0_TXI)
    .txi_irq = VECTOR_NUMBER_SCI0_TXI,
#else
    .txi_irq = FSP_INVALID_VECTOR,
#endif

#if defined(VECTOR_NUMBER_SCI0_TEI)
    .tei_irq = VECTOR_NUMBER_SCI0_TEI,
#else
    .tei_irq = FSP_INVALID_VECTOR,
#endif

#if defined(VECTOR_NUMBER_SCI0_ERI)
    .eri_irq = VECTOR_NUMBER_SCI0_ERI,
#else
    .eri_irq = FSP_INVALID_VECTOR,
#endif
};


/* Instance structure to use this module. */
const uart_instance_t g_uart2 =
{
    .p_ctrl = &g_uart2_ctrl,
    .p_cfg = &g_uart2_cfg,
    .p_api = &g_uart_on_sci
};

sci_uart_instance_ctrl_t g_uart2_ctrl;

baud_setting_t g_uart2_baud_setting =
{
    /* Baud rate calculated with 0.469% error. */
    {
        .semr_bits = {
                    .brme  = 0,
                    .abcse = 0,         //  bit of SEMR register
                    .abcs  = 0,         //  bit of SEMR register
                    .bgdm  = 1          //  bit of SEMR register
                     }
    },
    .cks = 0,           // PCLKA (64MHz) is not divided (input clock for SCI, n=0)
    .brr = 207U,        // calculated outside of MCU Framework to get 9600 bauds
    .mddr = (uint8_t) 256
};

/** UART extended configuration for UARTonSCI HAL driver */
const sci_uart_extended_cfg_t g_uart2_cfg_extend =
{
    .clock           = SCI_UART_CLOCK_INT,
    .rx_edge_start   = SCI_UART_START_BIT_FALLING_EDGE,
    .noise_cancel    = SCI_UART_NOISE_CANCELLATION_DISABLE,
    .rx_fifo_trigger = SCI_UART_RX_FIFO_TRIGGER_MAX,
    .p_baud_setting  = &g_uart2_baud_setting,
    .flow_control    = SCI_UART_FLOW_CONTROL_RTS,
    .flow_control_pin = BSP_IO_PORT_UNDEFINED,
};

/** UART interface configuration */
const uart_cfg_t g_uart2_cfg =
{
    .channel = 2,
    .data_bits  = UART_DATA_BITS_8,
    .parity     = UART_PARITY_OFF,
    .stop_bits  = UART_STOP_BITS_1,
    .p_callback = user_uart_callback,
    .p_context  = NULL,
    .p_extend   = &g_uart2_cfg_extend,
    .p_transfer_tx = NULL,
    .p_transfer_rx = NULL,
    .rxi_ipl = (12),
    .txi_ipl = (12),
    .tei_ipl = (12),
    .eri_ipl = (12),
#if defined(VECTOR_NUMBER_SCI2_RXI)
    .rxi_irq = VECTOR_NUMBER_SCI2_RXI,
#else
    .rxi_irq = FSP_INVALID_VECTOR,
#endif

#if defined(VECTOR_NUMBER_SCI2_TXI)
    .txi_irq = VECTOR_NUMBER_SCI2_TXI,
#else
    .txi_irq = FSP_INVALID_VECTOR,
#endif

#if defined(VECTOR_NUMBER_SCI2_TEI)
    .tei_irq = VECTOR_NUMBER_SCI2_TEI,
#else
    .tei_irq = FSP_INVALID_VECTOR,
#endif

#if defined(VECTOR_NUMBER_SCI2_ERI)
    .eri_irq = VECTOR_NUMBER_SCI2_ERI,
#else
    .eri_irq = FSP_INVALID_VECTOR,
#endif
};

gpt_instance_ctrl_t g_timerGPT7_ctrl;
const gpt_extended_cfg_t g_timerGPT7_extend = {
    .gtioca = {
                .output_enabled = false,
                .stop_level = GPT_PIN_LEVEL_LOW
              },
    .gtiocb = {
                .output_enabled = false,
                .stop_level = GPT_PIN_LEVEL_LOW
              },
    .start_source = GPT_SOURCE_NONE,
    .stop_source = GPT_SOURCE_NONE,
    .clear_source = GPT_SOURCE_NONE,
    .count_up_source = GPT_SOURCE_GPT_A,
    .count_down_source = GPT_SOURCE_NONE,
    .capture_a_source = GPT_SOURCE_NONE,
    .capture_b_source = GPT_SOURCE_NONE,
    .capture_a_ipl = (BSP_IRQ_DISABLED),
    .capture_b_ipl = (BSP_IRQ_DISABLED),
    .capture_a_irq = Invalid_vector,
    .capture_b_irq = Invalid_vector,
    .capture_filter_gtioca = GPT_CAPTURE_FILTER_NONE,
    .capture_filter_gtiocb = GPT_CAPTURE_FILTER_NONE,
    .p_pwm_cfg = NULL,
    .gtior_setting = { {0U} },
};

const timer_cfg_t g_timerGPT7_cfg = {
    .mode = TIMER_MODE_PERIODIC,
    .period_counts = 0xFFFFU,
    .duty_cycle_counts = 0x18,
    .source_div = TIMER_SOURCE_DIV_64,
    .channel = 7,
    .p_callback = NULL,
    .p_context = NULL,
    .p_extend = &g_timerGPT7_extend,
    .cycle_end_ipl = (0),
    .cycle_end_irq = Invalid_vector,
};

/* AGT timer instance */
agt_instance_ctrl_t timerAGT0_ctrl;

const agt_extended_cfg_t timerAGT0_extend =
{
    .count_source     = AGT_CLOCK_PCLKB,
    /* Output pin settings. */
    {
    .agtoab_bits = {
                    .agtoa = (uint8_t)AGT_PIN_CFG_DISABLED,
                    .agtob = (uint8_t)AGT_PIN_CFG_DISABLED,
                   }
    },
    .agto = (uint8_t)AGT_PIN_CFG_DISABLED,
    /* Input pin settings. */
    .measurement_mode = AGT_MEASURE_DISABLED,
    .agtio_filter     = AGT_AGTIO_FILTER_NONE,
    .enable_pin       = AGT_ENABLE_PIN_NOT_USED,
    .trigger_edge     = AGT_TRIGGER_EDGE_RISING
};

#if defined (BOOTLOADER_VARIANT)
const timer_cfg_t timerAGT0_cfg =
#else
timer_cfg_t timerAGT0_cfg =
#endif //BOOTLOADER_VARIANT
{
    .mode              = TIMER_MODE_PERIODIC,
    .period_counts     = PCLOCKB,
    .source_div        = TIMER_SOURCE_DIV_1,
    .duty_cycle_counts = 0,
    .channel           = 0,
    .cycle_end_ipl     = 0,
    .cycle_end_irq     = Invalid_vector,
    .p_callback        = NULL,
    .p_context         = NULL,
    .p_extend          = &timerAGT0_extend
};


void g_hal_init(void)
{
    g_common_init ();
}

/*lint -save -e818 parameter 'p_args' (line 251) could be declared as pointing to const */
void user_uart_callback(uart_callback_args_t * p_args)
{
    (void)*p_args;
}
/*lint -restore -e818 */
