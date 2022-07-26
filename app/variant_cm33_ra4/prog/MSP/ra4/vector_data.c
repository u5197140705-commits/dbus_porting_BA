/* generated vector source file - do not edit */
#include "bsp_api.h"

/* Do not build these data structures if no interrupts are currently allocated because IAR will have build errors. */
#if VECTOR_DATA_IRQ_COUNT > 0
        /* used */
        const bsp_interrupt_event_t g_interrupt_event_link_select[BSP_ICU_VECTOR_MAX_ENTRIES] =
        {
            /* SCI0 */
            [0]  = ELC_EVENT_SCI0_RXI, /* SCI0 RXI (Receive data full) */
            [1]  = ELC_EVENT_SCI0_TXI, /* SCI0 TXI (Transmit data empty) */
            [2]  = ELC_EVENT_SCI0_TEI, /* SCI0 TEI (Transmit end) */
            [3]  = ELC_EVENT_SCI0_ERI, /* SCI0 ERI (Receive error) */
            /* SCI1 */
            [4]  = ELC_EVENT_SCI1_RXI, /* SCI1 RXI (Receive data full) */
            [5]  = ELC_EVENT_SCI1_TXI, /* SCI1 TXI (Transmit data empty) */
            [6]  = ELC_EVENT_SCI1_TEI, /* SCI1 TEI (Transmit end) */
            [7]  = ELC_EVENT_SCI1_ERI, /* SCI1 ERI (Receive error) */
            /* SCI2 */
            [8]   = ELC_EVENT_SCI2_RXI, /* SCI2 RXI (Receive data full) */
            [9]   = ELC_EVENT_SCI2_TXI, /* SCI2 TXI (Transmit data empty) */
            [10]  = ELC_EVENT_SCI2_TEI, /* SCI2 TEI (Transmit end) */
            [11]  = ELC_EVENT_SCI2_ERI, /* SCI2 ERI (Receive error) */
            /* SCI3 */
            [12]  = ELC_EVENT_SCI3_RXI, /* SCI3 RXI (Receive data full) */
            [13]  = ELC_EVENT_SCI3_TXI, /* SCI3 TXI (Transmit data empty) */
            [14]  = ELC_EVENT_SCI3_TEI, /* SCI3 TEI (Transmit end) */
            [15]  = ELC_EVENT_SCI3_ERI, /* SCI3 ERI (Receive error) */
            /* SCI9 */
            [16] = ELC_EVENT_SCI9_RXI, /* SCI9 RXI (Receive data full) */
            [17] = ELC_EVENT_SCI9_TXI, /* SCI9 TXI (Transmit data empty) */
            [18] = ELC_EVENT_SCI9_TEI, /* SCI9 TEI (Transmit end) */
            [19] = ELC_EVENT_SCI9_ERI, /* SCI9 ERI (Receive error) */
            /* ADC0 */
            [20] = ELC_EVENT_ADC0_SCAN_END, /* A/D scan end interrupt */
            /* EXTI */
            [21] = ELC_EVENT_ICU_IRQ0 , /* External pin interrupt 00 */
            [22] = ELC_EVENT_ICU_IRQ1 , /* External pin interrupt 01 */
            [23] = ELC_EVENT_ICU_IRQ2 , /* External pin interrupt 02 */
            [24] = ELC_EVENT_ICU_IRQ3 , /* External pin interrupt 03 */
            [25] = ELC_EVENT_ICU_IRQ4 , /* External pin interrupt 04 */
            [26] = ELC_EVENT_ICU_IRQ5 , /* External pin interrupt 05 */
            [27] = ELC_EVENT_ICU_IRQ6 , /* External pin interrupt 06 */
            [28] = ELC_EVENT_ICU_IRQ7 , /* External pin interrupt 07 */
            [29] = ELC_EVENT_ICU_IRQ8 , /* External pin interrupt 08 */
            [30] = ELC_EVENT_ICU_IRQ9 , /* External pin interrupt 09 */
            [31] = ELC_EVENT_ICU_IRQ10, /* External pin interrupt 10 */
            [32] = ELC_EVENT_ICU_IRQ11, /* External pin interrupt 11 */
            [33] = ELC_EVENT_ICU_IRQ12, /* External pin interrupt 12 */
            [34] = ELC_EVENT_ICU_IRQ13, /* External pin interrupt 13 */
            [35] = ELC_EVENT_ICU_IRQ14, /* External pin interrupt 14 */
            [36] = ELC_EVENT_ICU_IRQ15, /* External pin interrupt 15 */
            /* GPT0 */
            [37] = ELC_EVENT_GPT0_CAPTURE_COMPARE_A, // Compare match A
            [38] = ELC_EVENT_GPT0_CAPTURE_COMPARE_B, // Compare match B
            [39] = ELC_EVENT_GPT0_COMPARE_C,         // Compare match C
            [40] = ELC_EVENT_GPT0_COMPARE_D,         // Compare match D
            [41] = ELC_EVENT_GPT0_COMPARE_E,         // Compare match E
            [42] = ELC_EVENT_GPT0_COMPARE_F,         // Compare match F
            [43] = ELC_EVENT_GPT0_COUNTER_OVERFLOW,  // Overflow
            [44] = ELC_EVENT_GPT0_COUNTER_UNDERFLOW, // Underflow
            [45] = ELC_EVENT_GPT0_PC,                // Period count function finish
            /* GPT1 */
            [46] = ELC_EVENT_GPT1_CAPTURE_COMPARE_A, // Compare match A
            [47] = ELC_EVENT_GPT1_CAPTURE_COMPARE_B, // Compare match B
            [48] = ELC_EVENT_GPT1_COMPARE_C,         // Compare match C
            [49] = ELC_EVENT_GPT1_COMPARE_D,         // Compare match D
            [50] = ELC_EVENT_GPT1_COMPARE_E,         // Compare match E
            [51] = ELC_EVENT_GPT1_COMPARE_F,         // Compare match F
            [52] = ELC_EVENT_GPT1_COUNTER_OVERFLOW,  // Overflow
            [53] = ELC_EVENT_GPT1_COUNTER_UNDERFLOW, // Underflow
            [54] = ELC_EVENT_GPT1_PC,                // Period count function finish
            /* GPT2 */
            [55] = ELC_EVENT_GPT2_CAPTURE_COMPARE_A, // Compare match A
            [56] = ELC_EVENT_GPT2_CAPTURE_COMPARE_B, // Compare match B
            [57] = ELC_EVENT_GPT2_COMPARE_C,         // Compare match C
            [58] = ELC_EVENT_GPT2_COMPARE_D,         // Compare match D
            [59] = ELC_EVENT_GPT2_COMPARE_E,         // Compare match E
            [60] = ELC_EVENT_GPT2_COMPARE_F,         // Compare match F
            [61] = ELC_EVENT_GPT2_COUNTER_OVERFLOW,  // Overflow
            [62] = ELC_EVENT_GPT2_COUNTER_UNDERFLOW, // Underflow
            /* GPT3 */
            [63] = ELC_EVENT_GPT3_CAPTURE_COMPARE_A, // Compare match A
            [64] = ELC_EVENT_GPT3_CAPTURE_COMPARE_B, // Compare match B
            [65] = ELC_EVENT_GPT3_COMPARE_C,         // Compare match C
            [66] = ELC_EVENT_GPT3_COMPARE_D,         // Compare match D
            [67] = ELC_EVENT_GPT3_COMPARE_E,         // Compare match E
            [68] = ELC_EVENT_GPT3_COMPARE_F,         // Compare match F
            [69] = ELC_EVENT_GPT3_COUNTER_OVERFLOW,  // Overflow
            [70] = ELC_EVENT_GPT3_COUNTER_UNDERFLOW,  // Underflow
            /* unused */
            [71] = ELC_EVENT_NONE,
            [72] = ELC_EVENT_NONE,
            [73] = ELC_EVENT_NONE,
            [74] = ELC_EVENT_NONE,
            [75] = ELC_EVENT_NONE,
            [76] = ELC_EVENT_NONE,
            [77] = ELC_EVENT_NONE,
            [78] = ELC_EVENT_NONE,
            [79] = ELC_EVENT_NONE,
            [80] = ELC_EVENT_NONE,
            [81] = ELC_EVENT_NONE,
            [82] = ELC_EVENT_NONE,
            [83] = ELC_EVENT_NONE,
            [84] = ELC_EVENT_NONE,
            [85] = ELC_EVENT_NONE,
            [86] = ELC_EVENT_NONE,
            [87] = ELC_EVENT_NONE,
            [88] = ELC_EVENT_NONE,
            [89] = ELC_EVENT_NONE,
            [90] = ELC_EVENT_NONE,
            [91] = ELC_EVENT_NONE,
            [92] = ELC_EVENT_NONE,
            [93] = ELC_EVENT_NONE,
            [94] = ELC_EVENT_NONE,
            [95] = ELC_EVENT_NONE
        };
#endif
