/*
 * Perif_Setup.c
 *
 *  Created on: 24 de jun de 2024
 *      Author: emanu
 */

#include "Perif_Setup.h"


/*
 * @brief Função para configurar pino como GPIO
 * Pino GPIO, com resistor de pull-up e controlado via CPU1
 * @return none*/
void Setup_GPIO(void)
{
    EALLOW;

    // GPIO34
    GpioCtrlRegs.GPBGMUX1.bit.GPIO34 = 0;               // GPIO
    GpioCtrlRegs.GPBGMUX1.bit.GPIO34 = 0;               // GPIO
    GpioCtrlRegs.GPBPUD.bit.GPIO34 = 1;                 // Resistor pull-up
    GpioCtrlRegs.GPBDIR.bit.GPIO34 = 1;
    GpioCtrlRegs.GPBCSEL1.bit.GPIO34 = GPIO_MUX_CPU1;   // CPU1

    EDIS;
}


/*
 * @brief Função para configurar pulso para start do ADC
 *
 * @return none
 */
void Setup_ePWM10(void){
    EALLOW;
    CpuSysRegs.PCLKCR2.bit.EPWM10 = 1;           //habilitar clock

    CpuSysRegs.PCLKCR0.bit.TBCLKSYNC = 0;

    EPwm10Regs.TBPRD = 5000;                         //periodo (up/down) 10Khz Clock/4/fpwm LAB PWM

    //Largura do pulso 50%
    EPwm10Regs.CMPA.bit.CMPA = EPwm10Regs.TBPRD >> 1;

    EPwm10Regs.TBPHS.bit.TBPHS = 0;                  // Phase is 0
    EPwm10Regs.TBCTL.bit.SYNCOSEL = TB_CTR_ZERO;
    EPwm10Regs.TBCTR = 0x0000;                       // Clear counter

    EPwm10Regs.TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN;  // Count up/down
    EPwm10Regs.TBCTL.bit.PHSEN = TB_DISABLE;         // Disable phase loading

    EPwm10Regs.TBCTL.bit.HSPCLKDIV = TB_DIV1;        // Clock ratio to SYSCLKOUT
    EPwm10Regs.TBCTL.bit.CLKDIV = TB_DIV1;

    EPwm10Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;     // Load registers every ZERO
    EPwm10Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO_PRD;
    EPwm10Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;     // Load registers every ZERO
    EPwm10Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO_PRD;

    EPwm10Regs.AQCTLA.bit.PRD = AQ_NO_ACTION;
    EPwm10Regs.AQCTLA.bit.ZRO = AQ_NO_ACTION;
    EPwm10Regs.AQCTLA.bit.CAU = AQ_CLEAR;            // set actions for EPWM10A
    EPwm10Regs.AQCTLA.bit.CAD = AQ_SET;

    //Trigger ADC
    EPwm10Regs.ETSEL.bit.SOCAEN = 1;                 // Enable SOC on A group
    EPwm10Regs.ETSEL.bit.SOCASEL = ET_CTR_PRDZERO;   // Dispara ADC no topo
    EPwm10Regs.ETPS.bit.SOCAPRD = ET_1ST;            // Trigger on every event

    CpuSysRegs.PCLKCR0.bit.TBCLKSYNC = 1;
    EDIS;
}

/*
 * @brief Função para configurar ADC
 * Start do ADCA, ADCB, ADCC no ePWM10
 * @return none
 */
void Setup_ADC()
{
    Uint16 acqps;

    // (12 bits - mínimo 75 ns) tSH = (ACQPS+1)*SYSCLK = (14+1)*5 = 75 ns
    // (16 bits - mínimo 320 ns) tSH = (63+1)*5 = 320 ns
    if(ADC_RESOLUTION_12BIT == AdcaRegs.ADCCTL2.bit.RESOLUTION) acqps = 14; else acqps = 63;
    if(ADC_RESOLUTION_12BIT == AdcbRegs.ADCCTL2.bit.RESOLUTION) acqps = 14; else acqps = 63;
    if(ADC_RESOLUTION_12BIT == AdccRegs.ADCCTL2.bit.RESOLUTION) acqps = 14; else acqps = 63;

    EALLOW;

    CpuSysRegs.PCLKCR13.bit.ADC_A = 1;              // SysRegs do ADCA
    CpuSysRegs.PCLKCR13.bit.ADC_B = 1;              // SysRegs do ADCB
    CpuSysRegs.PCLKCR13.bit.ADC_C = 1;              // SysRegs do ADCC
    AdcaRegs.ADCCTL2.bit.PRESCALE = 6;              // ADC funciona com no máximo 50 MHz (prescale = 6: 200/4)
    AdcbRegs.ADCCTL2.bit.PRESCALE = 6;              // ADC funciona com no máximo 50 MHz (prescale = 6: 200/4)
    AdccRegs.ADCCTL2.bit.PRESCALE = 6;              // ADC funciona com no máximo 50 MHz (prescale = 6: 200/4)

    AdcSetMode(ADC_ADCA, ADC_RESOLUTION_12BIT, ADC_SIGNALMODE_SINGLE);
    AdcSetMode(ADC_ADCB, ADC_RESOLUTION_12BIT, ADC_SIGNALMODE_SINGLE);
    AdcSetMode(ADC_ADCC, ADC_RESOLUTION_12BIT, ADC_SIGNALMODE_SINGLE);

    AdcaRegs.ADCCTL1.bit.INTPULSEPOS = 1;           // Flag ao final da conversão (1), amostragem (0)
    AdcaRegs.ADCCTL1.bit.ADCPWDNZ = 1;              // Power ADCA
    AdcbRegs.ADCCTL1.bit.INTPULSEPOS = 1;           // Flag ao final da conversão (1), amostragem (0)
    AdcbRegs.ADCCTL1.bit.ADCPWDNZ = 1;              // Power ADCB
    AdccRegs.ADCCTL1.bit.INTPULSEPOS = 1;           // Flag ao final da conversão (1), amostragem (0)
    AdccRegs.ADCCTL1.bit.ADCPWDNZ = 1;              // Power ADCC
    DELAY_US(1000);

    // 1 SOC (Start of Conversion) para cada conversão

    // SOC0 com sinais de corrente

    AdcaRegs.ADCSOC0CTL.bit.CHSEL = 3;              // ADCINA3
    AdcaRegs.ADCSOC0CTL.bit.ACQPS = acqps;
    AdcaRegs.ADCSOC0CTL.bit.TRIGSEL = 0x17;         // Start ePWM10

    AdcaRegs.ADCSOC1CTL.bit.CHSEL = 4;              // ADCINA4
    AdcaRegs.ADCSOC1CTL.bit.ACQPS = acqps;
    AdcaRegs.ADCSOC1CTL.bit.TRIGSEL = 0x17;         // Start ePWM10

    AdcbRegs.ADCSOC0CTL.bit.CHSEL = 3;              // ADCINB3
    AdcbRegs.ADCSOC0CTL.bit.ACQPS = acqps;
    AdcbRegs.ADCSOC0CTL.bit.TRIGSEL = 0x17;         // Start ePWM10

    AdcbRegs.ADCSOC1CTL.bit.CHSEL = 4;              // ADCINB4
    AdcbRegs.ADCSOC1CTL.bit.ACQPS = acqps;
    AdcbRegs.ADCSOC1CTL.bit.TRIGSEL = 0x17;         // Start ePWM10

    AdccRegs.ADCSOC0CTL.bit.CHSEL = 3;              // ADCINC3
    AdccRegs.ADCSOC0CTL.bit.ACQPS = acqps;
    AdccRegs.ADCSOC0CTL.bit.TRIGSEL = 0x17;         // Start ePWM10

    // SOC0 maior prioridade por default, gera flag EOC (End of Conversion)
    // Cada SOC gera um RESULT, começa RESULT0

    // Interrupção pós SOC1 do ADCA
    AdcaRegs.ADCINTSEL1N2.bit.INT1SEL = 0x01;       // Interrupação pós SOC1 gera INT1E (ADCA)
    AdcaRegs.ADCINTSEL1N2.bit.INT1E = 1;            // Habilita flag INT1
    AdcaRegs.ADCINTFLGCLR.bit.ADCINT1 = 1;          // Limpa flag INT1

    // Interrupção pós SOC1 do ADCB
    AdcbRegs.ADCINTSEL1N2.bit.INT1SEL = 0x01;       // Interrupação pós SOC1 gera INT1E (ADCB)
    AdcbRegs.ADCINTSEL1N2.bit.INT1E = 1;            // Habilita flag INT1
    AdcbRegs.ADCINTFLGCLR.bit.ADCINT1 = 1;          // Limpa flag INT1

    // Interrupção pós SOC0 do ADCC
    AdccRegs.ADCINTSEL1N2.bit.INT1SEL = 0x00;       // Interrupação pós SOC1 gera INT1E (ADCC)
    AdccRegs.ADCINTSEL1N2.bit.INT1E = 1;            // Habilita flag INT1
    AdccRegs.ADCINTFLGCLR.bit.ADCINT1 = 1;          // Limpa flag INT1

    EDIS;
}
