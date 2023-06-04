/* generated vector header file - do not edit */
#ifndef VECTOR_DATA_H
#define VECTOR_DATA_H
/* Number of interrupts allocated */
#ifndef VECTOR_DATA_IRQ_COUNT
#define VECTOR_DATA_IRQ_COUNT    (71)
#endif
/* ISR prototypes */
extern void sci_uart_rxi_isr(void);
extern void sci_uart_txi_isr(void);
extern void sci_uart_tei_isr(void);
extern void sci_uart_eri_isr(void);

/* UART interrupts */
#define VECTOR_NUMBER_SCI0_RXI      (Position_0_IRQn) /* SCI0 RXI (Receive data full), used in hal_data.c only */
#define SCI0_RXI_IRQn               (Position_0_IRQn) /* SCI0 RXI (Receive data full) */
#define VECTOR_NUMBER_SCI0_TXI      (Position_1_IRQn) /* SCI0 TXI (Transmit data empty), used in hal_data.c only */
#define SCI0_TXI_IRQn               (Position_1_IRQn) /* SCI0 TXI (Transmit data empty) */
#define VECTOR_NUMBER_SCI0_TEI      (Position_2_IRQn) /* SCI0 TEI (Transmit end) */
#define SCI0_TEI_IRQn               (Position_2_IRQn) /* SCI0 TEI (Transmit end) */
#define VECTOR_NUMBER_SCI0_ERI      (Position_3_IRQn) /* SCI0 ERI (Receive error), used in hal_data.c only */
#define SCI0_ERI_IRQn               (Position_3_IRQn) /* SCI0 ERI (Receive error) */

#define SCI1_RXI_IRQn               (Position_4_IRQn) /* SCI1 RXI (Receive data full) */
#define SCI1_TXI_IRQn               (Position_5_IRQn) /* SCI1 TXI (Transmit data empty) */
#define SCI1_TEI_IRQn               (Position_6_IRQn) /* SCI1 TEI (Transmit end) */
#define SCI1_ERI_IRQn               (Position_7_IRQn) /* SCI1 ERI (Receive error) */

#define VECTOR_NUMBER_SCI2_RXI      (Position_8_IRQn) /* SCI2 RXI (Receive data full) */
#define SCI2_RXI_IRQn               (Position_8_IRQn) /* SCI2 RXI (Receive data full) */
#define VECTOR_NUMBER_SCI2_TXI      (Position_9_IRQn) /* SCI2 TXI (Transmit data empty) */
#define SCI2_TXI_IRQn               (Position_9_IRQn) /* SCI2 TXI (Transmit data empty) */
#define VECTOR_NUMBER_SCI2_TEI      (Position_10_IRQn) /* SCI2 TEI (Transmit end) */
#define SCI2_TEI_IRQn               (Position_10_IRQn) /* SCI2 TEI (Transmit end) */
#define VECTOR_NUMBER_SCI2_ERI      (Position_11_IRQn) /* SCI2 ERI (Receive error) */
#define SCI2_ERI_IRQn               (Position_11_IRQn) /* SCI2 ERI (Receive error) */

#define SCI3_RXI_IRQn               (Position_12_IRQn) /* SCI3 RXI (Receive data full) */
#define SCI3_TXI_IRQn               (Position_13_IRQn) /* SCI3 TXI (Transmit data empty) */
#define SCI3_TEI_IRQn               (Position_14_IRQn) /* SCI3 TEI (Transmit end) */
#define SCI3_ERI_IRQn               (Position_15_IRQn) /* SCI3 ERI (Receive error) */

#define SCI9_RXI_IRQn               (Position_16_IRQn) /* SCI9 RXI (Receive data full) */
#define SCI9_TXI_IRQn               (Position_17_IRQn) /* SCI9 TXI (Transmit data empty) */
#define SCI9_TEI_IRQn               (Position_18_IRQn) /* SCI9 TEI (Transmit end) */
#define SCI9_ERI_IRQn               (Position_19_IRQn) /* SCI9 ERI (Receive error) */

/* ADC interrupt */
#define VECTOR_NUMBER_ADC0_SCAN_END (Position_20_IRQn) /* ADC0 SCAN END (A/D scan end interrupt), used in madc_mc.c only */
#define ADC0_SCAN_END_IRQn          (Position_20_IRQn) /* ADC0 SCAN END (A/D scan end interrupt) */

/* EXTI */
#define VECTOR_NUMBER_ICU_EXT_IRQ00 (Position_21_IRQn) /* EXT_IRQ00 interrupt) */
#define VECTOR_NUMBER_ICU_EXT_IRQ01 (Position_22_IRQn) /* EXT_IRQ01 interrupt) */
#define VECTOR_NUMBER_ICU_EXT_IRQ02 (Position_23_IRQn) /* EXT_IRQ02 interrupt) */
#define VECTOR_NUMBER_ICU_EXT_IRQ03 (Position_24_IRQn) /* EXT_IRQ03 interrupt) */
#define VECTOR_NUMBER_ICU_EXT_IRQ04 (Position_25_IRQn) /* EXT_IRQ04 interrupt) */
#define VECTOR_NUMBER_ICU_EXT_IRQ05 (Position_26_IRQn) /* EXT_IRQ05 interrupt) */
#define VECTOR_NUMBER_ICU_EXT_IRQ06 (Position_27_IRQn) /* EXT_IRQ06 interrupt) */
#define VECTOR_NUMBER_ICU_EXT_IRQ07 (Position_28_IRQn) /* EXT_IRQ07 interrupt) */
#define VECTOR_NUMBER_ICU_EXT_IRQ08 (Position_29_IRQn) /* EXT_IRQ08 interrupt) */
#define VECTOR_NUMBER_ICU_EXT_IRQ09 (Position_30_IRQn) /* EXT_IRQ09 interrupt) */
#define VECTOR_NUMBER_ICU_EXT_IRQ10 (Position_31_IRQn) /* EXT_IRQ10 interrupt) */
#define VECTOR_NUMBER_ICU_EXT_IRQ11 (Position_32_IRQn) /* EXT_IRQ11 interrupt) */
#define VECTOR_NUMBER_ICU_EXT_IRQ12 (Position_33_IRQn) /* EXT_IRQ12 interrupt) */
#define VECTOR_NUMBER_ICU_EXT_IRQ13 (Position_34_IRQn) /* EXT_IRQ13 interrupt) */
#define VECTOR_NUMBER_ICU_EXT_IRQ14 (Position_35_IRQn) /* EXT_IRQ14 interrupt) */
#define VECTOR_NUMBER_ICU_EXT_IRQ15 (Position_36_IRQn) /* EXT_IRQ15 interrupt) */

/* GPT0 interrupts */
#define GPT0_CCMPA_IRQn             (Position_37_IRQn) /* GPT0_CCMPA (GTCCRA capture/compare match) */
#define GPT0_CCMPB_IRQn             (Position_38_IRQn) /* GPT0_CCMPA (GTCCRB capture/compare match) */
#define GPT0_CMPC_IRQn              (Position_39_IRQn) /* GPT0_CMPC (GTCRC compare match) */
#define GPT0_CMPD_IRQn              (Position_40_IRQn) /* GPT0_CMPD (GTCRD compare match) */
#define GPT0_CMPE_IRQn              (Position_41_IRQn) /* GPT0_CMPE (GTCRE compare match) */
#define GPT0_CMPF_IRQn              (Position_42_IRQn) /* GPT0_CMPF (GTCRF compare match) */
#define GPT0_OVF_IRQn               (Position_43_IRQn) /* GPT0_OVF (GTCNT overflow) */
#define GPT0_UDF_IRQn               (Position_44_IRQn) /* GPT0_UDF (GTCNT underflow) */
#define GPT0_PC_IRQn                (Position_45_IRQn) /* GPT0_PC (Period count function finish) */
/* GPT1 interrupts */
#define GPT1_CCMPA_IRQn             (Position_46_IRQn) /* GPT1_CCMPA (GTCCRA capture/compare match) */
#define GPT1_CCMPB_IRQn             (Position_47_IRQn) /* GPT1_CCMPA (GTCCRB capture/compare match) */
#define GPT1_CMPC_IRQn              (Position_48_IRQn) /* GPT1_CMPC (GTCRC compare match) */
#define GPT1_CMPD_IRQn              (Position_49_IRQn) /* GPT1_CMPD (GTCRD compare match) */
#define GPT1_CMPE_IRQn              (Position_50_IRQn) /* GPT1_CMPE (GTCRE compare match) */
#define GPT1_CMPF_IRQn              (Position_51_IRQn) /* GPT1_CMPF (GTCRF compare match) */
#define GPT1_OVF_IRQn               (Position_52_IRQn) /* GPT1_OVF (GTCNT overflow) */
#define GPT1_UDF_IRQn               (Position_53_IRQn) /* GPT1_UDF (GTCNT underflow) */
#define GPT1_PC_IRQn                (Position_54_IRQn) /* GPT1_PC (Period count function finish) */
/* GPT2 interrupts */
#define GPT2_CCMPA_IRQn             (Position_55_IRQn) /* GPT2_CCMPA (GTCCRA capture/compare match) */
#define GPT2_CCMPB_IRQn             (Position_56_IRQn) /* GPT2_CCMPA (GTCCRB capture/compare match) */
#define GPT2_CMPC_IRQn              (Position_57_IRQn) /* GPT2_CMPC (GTCRC compare match) */
#define GPT2_CMPD_IRQn              (Position_58_IRQn) /* GPT2_CMPD (GTCRD compare match) */
#define GPT2_CMPE_IRQn              (Position_59_IRQn) /* GPT2_CMPE (GTCRE compare match) */
#define GPT2_CMPF_IRQn              (Position_60_IRQn) /* GPT2_CMPF (GTCRF compare match) */
#define GPT2_OVF_IRQn               (Position_61_IRQn) /* GPT2_OVF (GTCNT overflow) */
#define GPT2_UDF_IRQn               (Position_62_IRQn) /* GPT2_UDF (GTCNT underflow) */
/* GPT3 interrupts */
#define GPT3_CCMPA_IRQn             (Position_63_IRQn) /* GPT3_CCMPA (GTCCRA capture/compare match) */
#define GPT3_CCMPB_IRQn             (Position_64_IRQn) /* GPT3_CCMPA (GTCCRB capture/compare match) */
#define GPT3_CMPC_IRQn              (Position_65_IRQn) /* GPT3_CMPC (GTCRC compare match) */
#define GPT3_CMPD_IRQn              (Position_66_IRQn) /* GPT3_CMPD (GTCRD compare match) */
#define GPT3_CMPE_IRQn              (Position_67_IRQn) /* GPT3_CMPE (GTCRE compare match) */
#define GPT3_CMPF_IRQn              (Position_68_IRQn) /* GPT3_CMPF (GTCRF compare match) */
#define GPT3_OVF_IRQn               (Position_69_IRQn) /* GPT3_OVF (GTCNT overflow) */
#define GPT3_UDF_IRQn               (Position_70_IRQn) /* GPT3_UDF (GTCNT underflow) */

#endif /* VECTOR_DATA_H */
