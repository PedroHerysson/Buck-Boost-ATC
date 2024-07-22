/*
 * Perif_Setup.c
 *
 *  Created on: 24 de jun de 2024
 *      Author: emanu
 */

#include "Perif_Setup.h"


#define OL_DC 814U                  // Open Loop Signal Control

// Gain Curve Parameters, Vomax = 1260 V
#define GC_DC1 114U
#define GC_DC2 243U
#define GC_DC3 357U
#define GC_DC4 471U
#define GC_DC5 600U
#define GC_DC6 714U
#define GC_DC7 828U
#define GC_DC8 957U
#define GC_DC9 1071U
#define GC_DC10 1185U
#define GC_DC11 1314U

//#define ref 2520U                   // Voltage Reference
#define ref 1260U                   // Voltage Reference


#define points 4                    // Points of Move Mean Filter

float u[3] = {0,0,0};
float e[3] = {0,0,0};
float k[3] = {0,0,0};
float er[3] = {0,0,0};

uint16_t Aux_points;
uint16_t Aux_points2;

uint16_t Mean[points];
uint16_t Mean2[points];

uint16_t Filt;
uint16_t Filt2;
uint16_t Aux_filt;
uint16_t Aux_filt2;

float aux;

/*
 * @brief Funï¿½ï¿½o para configurar pino como GPIO
 * Pino GPIO, com resistor de pull-up e controlado via CPU1
 * @return none*/
void Setup_GPIO(void)
{
    EALLOW;

    GpioCtrlRegs.GPBGMUX1.bit.GPIO32 = 0;               // GPIO
    GpioCtrlRegs.GPBGMUX1.bit.GPIO32 = 0;               // GPIO
    GpioCtrlRegs.GPBPUD.bit.GPIO32 = 1;                 // Resistor pull-up
    GpioCtrlRegs.GPBDIR.bit.GPIO32 = 1;                 // Only GPIO
    GpioCtrlRegs.GPBCSEL1.bit.GPIO32 = GPIO_MUX_CPU1;   // CPU1
    // GPIO34 - LED
    GpioCtrlRegs.GPBGMUX1.bit.GPIO34 = 0;               // GPIO
    GpioCtrlRegs.GPBGMUX1.bit.GPIO34 = 0;               // GPIO
    GpioCtrlRegs.GPBPUD.bit.GPIO34 = 1;                 // Resistor pull-up
    GpioCtrlRegs.GPBDIR.bit.GPIO34 = 1;                 // Only GPIO
    GpioCtrlRegs.GPBCSEL1.bit.GPIO34 = GPIO_MUX_CPU1;   // CPU1

    // GPIO0 - ePWM1A (J4 - 40)
    GpioCtrlRegs.GPAGMUX1.bit.GPIO0 = 0;
    GpioCtrlRegs.GPAMUX1.bit.GPIO0 = 1;
    GpioCtrlRegs.GPAPUD.bit.GPIO0 = 1;
    GpioCtrlRegs.GPACSEL1.bit.GPIO0 = GPIO_MUX_CPU1;

    // GPIO1 - ePWM1B (J4 - 39)
    GpioCtrlRegs.GPAGMUX1.bit.GPIO1 = 0;
    GpioCtrlRegs.GPAMUX1.bit.GPIO1 = 1;
    GpioCtrlRegs.GPAPUD.bit.GPIO1 = 1;
    GpioCtrlRegs.GPACSEL1.bit.GPIO1 = GPIO_MUX_CPU1;

    // GPIO2 - ePWM2A (J4 - 38)
    GpioCtrlRegs.GPAGMUX1.bit.GPIO2 = 0;
    GpioCtrlRegs.GPAMUX1.bit.GPIO2 = 1;
    GpioCtrlRegs.GPAPUD.bit.GPIO2 = 1;
    GpioCtrlRegs.GPACSEL1.bit.GPIO2 = GPIO_MUX_CPU1;

    // GPIO3 - ePWM2B (J4 - 37)
    GpioCtrlRegs.GPAGMUX1.bit.GPIO3 = 0;
    GpioCtrlRegs.GPAMUX1.bit.GPIO3 = 1;
    GpioCtrlRegs.GPAPUD.bit.GPIO3 = 1;
    GpioCtrlRegs.GPACSEL1.bit.GPIO3 = GPIO_MUX_CPU1;

    // GPIO4 - ePWM3A (J4 - 36)
    GpioCtrlRegs.GPAGMUX1.bit.GPIO4 = 0;
    GpioCtrlRegs.GPAMUX1.bit.GPIO4 = 1;
    GpioCtrlRegs.GPAPUD.bit.GPIO4 = 1;
    GpioCtrlRegs.GPACSEL1.bit.GPIO4 = GPIO_MUX_CPU1;

    // GPIO5 - ePWM3B (J4 - 35)
    GpioCtrlRegs.GPAGMUX1.bit.GPIO5 = 0;
    GpioCtrlRegs.GPAMUX1.bit.GPIO5 = 1;
    GpioCtrlRegs.GPAPUD.bit.GPIO5 = 1;
    GpioCtrlRegs.GPACSEL1.bit.GPIO5 = GPIO_MUX_CPU1;

    // GPIO6 - ePWM4A (J8 - 80)
    GpioCtrlRegs.GPAGMUX1.bit.GPIO6 = 0;
    GpioCtrlRegs.GPAMUX1.bit.GPIO6 = 1;
    GpioCtrlRegs.GPAPUD.bit.GPIO6 = 1;
    GpioCtrlRegs.GPACSEL1.bit.GPIO6 = GPIO_MUX_CPU1;

    // GPIO7 - ePWM4B (J8 - 79)
    GpioCtrlRegs.GPAGMUX1.bit.GPIO7 = 0;
    GpioCtrlRegs.GPAMUX1.bit.GPIO7 = 1;
    GpioCtrlRegs.GPAPUD.bit.GPIO7 = 1;
    GpioCtrlRegs.GPACSEL1.bit.GPIO7 = GPIO_MUX_CPU1;

    // GPIO8 - ePWM5A (J8 - 78)
    GpioCtrlRegs.GPAGMUX1.bit.GPIO8 = 0;
    GpioCtrlRegs.GPAMUX1.bit.GPIO8 = 1;
    GpioCtrlRegs.GPAPUD.bit.GPIO8 = 1;
    GpioCtrlRegs.GPACSEL2.bit.GPIO8 = GPIO_MUX_CPU1;

    // GPIO9 - ePWM5B (J8 - 77)
    GpioCtrlRegs.GPAGMUX1.bit.GPIO9 = 0;
    GpioCtrlRegs.GPAMUX1.bit.GPIO9 = 1;
    GpioCtrlRegs.GPAPUD.bit.GPIO9 = 1;
    GpioCtrlRegs.GPACSEL2.bit.GPIO9 = GPIO_MUX_CPU1;

    // GPIO10 - ePWM6A (J8 - 76)
    GpioCtrlRegs.GPAGMUX1.bit.GPIO10 = 0;
    GpioCtrlRegs.GPAMUX1.bit.GPIO10 = 1;
    GpioCtrlRegs.GPAPUD.bit.GPIO10 = 1;
    GpioCtrlRegs.GPACSEL2.bit.GPIO10 = GPIO_MUX_CPU1;

    // GPIO8 - ePWM6B (J8 - 75)
    GpioCtrlRegs.GPAGMUX1.bit.GPIO11 = 0;
    GpioCtrlRegs.GPAMUX1.bit.GPIO11 = 1;
    GpioCtrlRegs.GPAPUD.bit.GPIO11 = 1;
    GpioCtrlRegs.GPACSEL2.bit.GPIO11 = GPIO_MUX_CPU1;

    EDIS;
}

/*
 * @brief Setup ePWM1 (CREE MODULE 1)
 * PWM Configuration
 * @return none
 * */
void Setup_ePWM1(void)
{
    EALLOW;
    CpuSysRegs.PCLKCR2.bit.EPWM1 = 1;                   // Off module clock
    CpuSysRegs.PCLKCR0.bit.TBCLKSYNC = 0;               // Off module count

    EPwm1Regs.TBPRD = 1428;                             // 35 KHz frequency
    EPwm1Regs.TBPHS.bit.TBPHS = 0;                      // 0 phase
    EPwm1Regs.TBCTR = 0;                                // Clear count
    EPwm1Regs.TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN;      // Triangule
    EPwm1Regs.TBCTL.bit.PHSEN = TB_DISABLE;             // No phase
    EPwm1Regs.TBCTL.bit.HSPCLKDIV = TB_DIV1;            // Prescale - 200 MHz clock
    EPwm1Regs.TBCTL.bit.CLKDIV = TB_DIV1;

    EPwm1Regs.CMPA.bit.CMPA = EPwm1Regs.TBPRD >> 1;     // 0,5

    EPwm1Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;         // Load after event, between 0 and PRD
    EPwm1Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO_PRD;
    EPwm1Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;
    EPwm1Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO_PRD;

    EPwm1Regs.AQCTLA.bit.CAU = AQ_CLEAR;                // Action for UP and DOWN ePWM1A - DIODE
    EPwm1Regs.AQCTLA.bit.CAD = AQ_CLEAR;
    EPwm1Regs.AQCTLA.bit.PRD = AQ_CLEAR;
    EPwm1Regs.AQCTLA.bit.ZRO = AQ_CLEAR;

    EPwm1Regs.AQCTLB.bit.CAU = AQ_CLEAR;                // Action for UP and DOWN ePWM1B - DIODE
    EPwm1Regs.AQCTLB.bit.CAD = AQ_CLEAR;
    EPwm1Regs.AQCTLB.bit.PRD = AQ_CLEAR;
    EPwm1Regs.AQCTLB.bit.ZRO = AQ_CLEAR;

    // DBCTL not used

    CpuSysRegs.PCLKCR0.bit.TBCLKSYNC = 1;               // On module count

    EDIS;
}

/*
 * @brief Setup ePWM2 (CREE MODULE 1)
 * PWM Configuration
 * @return none
 * */
void Setup_ePWM2(void)
{
    EALLOW;
    CpuSysRegs.PCLKCR2.bit.EPWM2 = 1;                   // Off module clock
    CpuSysRegs.PCLKCR0.bit.TBCLKSYNC = 0;               // Off module count

    EPwm2Regs.TBPRD = 1428;                             // 35 KHz frequency
    EPwm2Regs.TBPHS.bit.TBPHS = 0;                      // 0 phase
    EPwm2Regs.TBCTR = 0;                                // Clear count
    EPwm2Regs.TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN;      // Triangule
    EPwm2Regs.TBCTL.bit.PHSEN = TB_DISABLE;             // No phase
    EPwm2Regs.TBCTL.bit.HSPCLKDIV = TB_DIV1;            // Prescale - 200 MHz clock
    EPwm2Regs.TBCTL.bit.CLKDIV = TB_DIV1;

    EPwm2Regs.CMPA.bit.CMPA = EPwm2Regs.TBPRD >> 1;     // 0,5

    EPwm2Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;         // Load after event, between 0 and PRD
    EPwm2Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO_PRD;
    EPwm2Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;
    EPwm2Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO_PRD;

    EPwm2Regs.AQCTLA.bit.CAU = AQ_CLEAR;                // Action for UP and DOWN ePWM2A - DIODE
    EPwm2Regs.AQCTLA.bit.CAD = AQ_CLEAR;
    EPwm2Regs.AQCTLA.bit.PRD = AQ_CLEAR;
    EPwm2Regs.AQCTLA.bit.ZRO = AQ_CLEAR;

    EPwm2Regs.AQCTLB.bit.CAU = AQ_CLEAR;                // Action for UP and DOWN ePWM2B - DIODE
    EPwm2Regs.AQCTLB.bit.CAD = AQ_CLEAR;
    EPwm2Regs.AQCTLB.bit.PRD = AQ_CLEAR;
    EPwm2Regs.AQCTLB.bit.ZRO = AQ_CLEAR;

    // DBCTL not used

    CpuSysRegs.PCLKCR0.bit.TBCLKSYNC = 1;               // On module count

    EDIS;
}

/*
 * @brief Setup ePWM3 (CREE MODULE 1)
 * PWM Configuration
 * @return none
 * */
void Setup_ePWM3(void)
{
    EALLOW;
    CpuSysRegs.PCLKCR2.bit.EPWM3 = 1;                   // Off module clock
    CpuSysRegs.PCLKCR0.bit.TBCLKSYNC = 0;               // Off module count

    EPwm3Regs.TBPRD = 1428;                             // 35 KHz frequency
    EPwm3Regs.TBPHS.bit.TBPHS = 0;                      // 0 phase
    EPwm3Regs.TBCTR = 0;                                // Clear count
    EPwm3Regs.TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN;      // Triangule
    EPwm3Regs.TBCTL.bit.PHSEN = TB_DISABLE;             // No phase
    EPwm3Regs.TBCTL.bit.HSPCLKDIV = TB_DIV1;            // Prescale - 200 MHz clock
    EPwm3Regs.TBCTL.bit.CLKDIV = TB_DIV1;

    EPwm3Regs.CMPA.bit.CMPA = EPwm3Regs.TBPRD >> 1;     // 0,5

    EPwm3Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;         // Load after event, between 0 and PRD
    EPwm3Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO_PRD;
    EPwm3Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;
    EPwm3Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO_PRD;

    EPwm3Regs.AQCTLA.bit.CAU = AQ_CLEAR;                // Action for UP and DOWN ePWM3A - DIODE
    EPwm3Regs.AQCTLA.bit.CAD = AQ_CLEAR;
    EPwm3Regs.AQCTLA.bit.PRD = AQ_CLEAR;
    EPwm3Regs.AQCTLA.bit.ZRO = AQ_CLEAR;

    EPwm3Regs.AQCTLB.bit.CAU = AQ_CLEAR;                // Action for UP and DOWN ePWM3B - DIODE
    EPwm3Regs.AQCTLB.bit.CAD = AQ_CLEAR;
    EPwm3Regs.AQCTLB.bit.PRD = AQ_CLEAR;
    EPwm3Regs.AQCTLB.bit.ZRO = AQ_CLEAR;

    // DBCTL not used

    CpuSysRegs.PCLKCR0.bit.TBCLKSYNC = 1;               // On module count

    EDIS;
}

/*
 * @brief Setup ePWM4 (CREE MODULE 2)
 * PWM Configuration
 * @return none
 * */
void Setup_ePWM4(void)
{
    EALLOW;
    CpuSysRegs.PCLKCR2.bit.EPWM4 = 1;                   // Off module clock
    CpuSysRegs.PCLKCR0.bit.TBCLKSYNC = 0;               // Off module count

    EPwm4Regs.TBPRD = 1428;                             // 35 KHz frequency
    EPwm4Regs.TBPHS.bit.TBPHS = 0;                      // 0 phase
    EPwm4Regs.TBCTR = 0;                                // Clear count
    EPwm4Regs.TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN;      // Triangule

    EPwm4Regs.TBCTL.bit.SYNCOSEL = TB_CTR_ZERO;

    EPwm4Regs.TBCTL.bit.PHSEN = TB_DISABLE;             // No phase
    EPwm4Regs.TBCTL.bit.HSPCLKDIV = TB_DIV1;            // Prescale - 200 MHz clock
    EPwm4Regs.TBCTL.bit.CLKDIV = TB_DIV1;

    EPwm4Regs.CMPA.bit.CMPA = 1;                      // 0,57 (814)
//    EPwm4Regs.CMPA.bit.CMPA = OL_DC;                      // 0,57 (814)

    EPwm4Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;         // Load after event, between 0 and PRD
    EPwm4Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO_PRD;
    EPwm4Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;
    EPwm4Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO_PRD;

    EPwm4Regs.AQCTLA.bit.CAU = AQ_CLEAR;                // Action for UP and DOWN ePWM4A - DIODE
    EPwm4Regs.AQCTLA.bit.CAD = AQ_CLEAR;
    EPwm4Regs.AQCTLA.bit.PRD = AQ_CLEAR;
    EPwm4Regs.AQCTLA.bit.ZRO = AQ_CLEAR;

    EPwm4Regs.AQCTLB.bit.CAU = AQ_CLEAR;                // Action for UP and DOWN ePWM4B - SWITCH
    EPwm4Regs.AQCTLB.bit.CAD = AQ_SET;
    EPwm4Regs.AQCTLB.bit.PRD = AQ_NO_ACTION;
    EPwm4Regs.AQCTLB.bit.ZRO = AQ_NO_ACTION;

    // DBCTL not used

    CpuSysRegs.PCLKCR0.bit.TBCLKSYNC = 1;               // On module count

    EDIS;
}

/*
 * @brief Setup ePWM5 (CREE MODULE 2)
 * PWM Configuration
 * @return none
 * */
void Setup_ePWM5(void)
{
    EALLOW;
    CpuSysRegs.PCLKCR2.bit.EPWM5 = 1;                   // Off module clock
    CpuSysRegs.PCLKCR0.bit.TBCLKSYNC = 0;               // Off module count

    EPwm5Regs.TBPRD = 1428;                             // 35 KHz frequency
    EPwm5Regs.TBCTR = 0;                                // Clear count
    EPwm5Regs.TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN;      // Triangule

    EPwm5Regs.TBPHS.bit.TBPHS = 952;                    // 120 phase (0-TBPRD)(0-180)
    EPwm5Regs.TBCTL.bit.SYNCOSEL = TB_SYNC_IN;
    EPwm5Regs.TBCTL.bit.PHSEN = TB_ENABLE;              // Phase
    EPwm5Regs.TBCTL.bit.PHSDIR = TB_DOWN;               // Direction down

    EPwm5Regs.TBCTL.bit.HSPCLKDIV = TB_DIV1;            // Prescale - 200 MHz clock
    EPwm5Regs.TBCTL.bit.CLKDIV = TB_DIV1;

    EPwm5Regs.CMPA.bit.CMPA = 1;                      // 0,5 (814)
//    EPwm5Regs.CMPA.bit.CMPA = OL_DC;                      // 0,5 (814)

    EPwm5Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;         // Load after event, between 0 and PRD
    EPwm5Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO_PRD;
    EPwm5Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;
    EPwm5Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO_PRD;

    EPwm5Regs.AQCTLA.bit.CAU = AQ_CLEAR;                // Action for UP and DOWN ePWM5A - DIODE
    EPwm5Regs.AQCTLA.bit.CAD = AQ_CLEAR;
    EPwm5Regs.AQCTLA.bit.PRD = AQ_CLEAR;
    EPwm5Regs.AQCTLA.bit.ZRO = AQ_CLEAR;

    EPwm5Regs.AQCTLB.bit.CAU = AQ_CLEAR;                // Action for UP and DOWN ePWM5B - SWITCH
    EPwm5Regs.AQCTLB.bit.CAD = AQ_SET;
    EPwm5Regs.AQCTLB.bit.PRD = AQ_NO_ACTION;
    EPwm5Regs.AQCTLB.bit.ZRO = AQ_NO_ACTION;

    // DBCTL not used

    CpuSysRegs.PCLKCR0.bit.TBCLKSYNC = 1;               // On module count

    EDIS;
}

/*
 * @brief Setup ePWM6 (CREE MODULE 2)
 * PWM Configuration
 * @return none
 * */
void Setup_ePWM6(void)
{
    EALLOW;
    CpuSysRegs.PCLKCR2.bit.EPWM6 = 1;                   // Off module clock
    CpuSysRegs.PCLKCR0.bit.TBCLKSYNC = 0;               // Off module count

    EPwm6Regs.TBPRD = 1428;                             // 35 KHz frequency
    EPwm6Regs.TBCTR = 0;                                // Clear count
    EPwm6Regs.TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN;      // Triangule

    EPwm6Regs.TBPHS.bit.TBPHS = 952;                    // 0 phase (0-TBPRD)
    EPwm6Regs.TBCTL.bit.SYNCOSEL = TB_SYNC_IN;
    EPwm6Regs.TBCTL.bit.PHSEN = TB_ENABLE;              // Phase
    EPwm6Regs.TBCTL.bit.PHSDIR = TB_UP;                 // Direction up

    EPwm6Regs.TBCTL.bit.HSPCLKDIV = TB_DIV1;            // Prescale - 200 MHz clock
    EPwm6Regs.TBCTL.bit.CLKDIV = TB_DIV1;

    EPwm6Regs.CMPA.bit.CMPA = 1;                      // 0,5 (814)
//    EPwm6Regs.CMPA.bit.CMPA = OL_DC;                      // 0,5 (814)

    EPwm6Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;         // Load after event, between 0 and PRD
    EPwm6Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO_PRD;
    EPwm6Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;
    EPwm6Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO_PRD;

    EPwm6Regs.AQCTLA.bit.CAU = AQ_CLEAR;                // Action for UP and DOWN ePWM6A - DIODE
    EPwm6Regs.AQCTLA.bit.CAD = AQ_CLEAR;
    EPwm6Regs.AQCTLA.bit.PRD = AQ_CLEAR;
    EPwm6Regs.AQCTLA.bit.ZRO = AQ_CLEAR;

    EPwm6Regs.AQCTLB.bit.CAU = AQ_CLEAR;                // Action for UP and DOWN ePWM6B - SWITCH
    EPwm6Regs.AQCTLB.bit.CAD = AQ_SET;
    EPwm6Regs.AQCTLB.bit.PRD = AQ_NO_ACTION;
    EPwm6Regs.AQCTLB.bit.ZRO = AQ_NO_ACTION;

    // DBCTL not used

    CpuSysRegs.PCLKCR0.bit.TBCLKSYNC = 1;               // On module count

    EDIS;
}


/*
 * @brief Funï¿½ï¿½o para configurar pulso para start do ADC
 *
 * @return none
 */
void Setup_ePWM10(void){
    EALLOW;
    CpuSysRegs.PCLKCR2.bit.EPWM10 = 1;           //habilitar clock
    CpuSysRegs.PCLKCR0.bit.TBCLKSYNC = 0;

    EPwm10Regs.TBPRD = 952;                          //periodo (up/down) 105Khz Clock/2/fpwm LAB PWM
    EPwm10Regs.TBPHS.bit.TBPHS = 0;                  // Phase is 0
    EPwm10Regs.TBCTR = 0x0000;                       // Clear counter
    EPwm10Regs.TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN;  // Count up/down

    EPwm10Regs.TBCTL.bit.SYNCOSEL = TB_SYNC_IN;
    EPwm10Regs.TBCTL.bit.PHSEN = TB_ENABLE;              // Phase
    EPwm10Regs.TBCTL.bit.PHSDIR = TB_UP;

    EPwm10Regs.TBCTL.bit.HSPCLKDIV = TB_DIV1;        // Clock ratio to SYSCLKOUT
    EPwm10Regs.TBCTL.bit.CLKDIV = TB_DIV1;

    EPwm10Regs.CMPA.bit.CMPA = EPwm10Regs.TBPRD >> 1;

    EPwm10Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;     // Load registers every ZERO
    EPwm10Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO_PRD;
    EPwm10Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;     // Load registers every ZERO
    EPwm10Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO_PRD;

    EPwm10Regs.AQCTLA.bit.PRD = AQ_CLEAR;
    EPwm10Regs.AQCTLA.bit.ZRO = AQ_SET;
    EPwm10Regs.AQCTLA.bit.CAU = AQ_NO_ACTION;            // set actions for EPWM10A
    EPwm10Regs.AQCTLA.bit.CAD = AQ_NO_ACTION;

    //Trigger ADC
    EPwm10Regs.ETSEL.bit.SOCAEN = 1;                 // Enable SOC on A group
    EPwm10Regs.ETSEL.bit.SOCASEL = ET_CTR_PRDZERO;   // Dispara ADC no topo
    EPwm10Regs.ETPS.bit.SOCAPRD = ET_1ST;            // Trigger on every event

    CpuSysRegs.PCLKCR0.bit.TBCLKSYNC = 1;
    EDIS;
}

/*
 * @brief Funcao para configurar ADC
 * Start do ADCA, ADCB, ADCC no ePWM10
 * @return none
 */
void Setup_ADC()
{
    Uint16 acqps;

    // (12 bits - minimo 75 ns) tSH = (ACQPS+1)*SYSCLK = (14+1)*5 = 75 ns
    // (16 bits - minimo 320 ns) tSH = (63+1)*5 = 320 ns
    if(ADC_RESOLUTION_12BIT == AdcaRegs.ADCCTL2.bit.RESOLUTION) acqps = 14; else acqps = 63;
    if(ADC_RESOLUTION_12BIT == AdcbRegs.ADCCTL2.bit.RESOLUTION) acqps = 14; else acqps = 63;
    if(ADC_RESOLUTION_12BIT == AdccRegs.ADCCTL2.bit.RESOLUTION) acqps = 14; else acqps = 63;

    EALLOW;

    CpuSysRegs.PCLKCR13.bit.ADC_A = 1;              // SysRegs do ADCA
    CpuSysRegs.PCLKCR13.bit.ADC_B = 1;              // SysRegs do ADCB
    CpuSysRegs.PCLKCR13.bit.ADC_C = 1;              // SysRegs do ADCC
    AdcaRegs.ADCCTL2.bit.PRESCALE = 6;              // ADC funciona com no mï¿½ximo 50 MHz (prescale = 6: 200/4)
    AdcbRegs.ADCCTL2.bit.PRESCALE = 6;              // ADC funciona com no mï¿½ximo 50 MHz (prescale = 6: 200/4)
    AdccRegs.ADCCTL2.bit.PRESCALE = 6;              // ADC funciona com no mï¿½ximo 50 MHz (prescale = 6: 200/4)

    AdcSetMode(ADC_ADCA, ADC_RESOLUTION_12BIT, ADC_SIGNALMODE_SINGLE);
    AdcSetMode(ADC_ADCB, ADC_RESOLUTION_12BIT, ADC_SIGNALMODE_SINGLE);
    AdcSetMode(ADC_ADCC, ADC_RESOLUTION_12BIT, ADC_SIGNALMODE_SINGLE);

    AdcaRegs.ADCCTL1.bit.INTPULSEPOS = 1;           // Flag ao final da conversao (1), amostragem (0)
    AdcaRegs.ADCCTL1.bit.ADCPWDNZ = 1;              // Power ADCA
    AdcbRegs.ADCCTL1.bit.INTPULSEPOS = 1;           // Flag ao final da conversao (1), amostragem (0)
    AdcbRegs.ADCCTL1.bit.ADCPWDNZ = 1;              // Power ADCB
    AdccRegs.ADCCTL1.bit.INTPULSEPOS = 1;           // Flag ao final da conversao (1), amostragem (0)
    AdccRegs.ADCCTL1.bit.ADCPWDNZ = 1;              // Power ADCC
    DELAY_US(1000);

    // 1 SOC (Start of Conversion) to each conversion

    // SOC0 com sinais de corrente

    AdcaRegs.ADCSOC0CTL.bit.CHSEL = 3;              // ADCINA3 - Isec1
    AdcaRegs.ADCSOC0CTL.bit.ACQPS = acqps;
    AdcaRegs.ADCSOC0CTL.bit.TRIGSEL = 0x17;         // Start ePWM10

    AdcaRegs.ADCSOC1CTL.bit.CHSEL = 4;              // ADCINA4 - Vdcprim
    AdcaRegs.ADCSOC1CTL.bit.ACQPS = acqps;
    AdcaRegs.ADCSOC1CTL.bit.TRIGSEL = 0x17;         // Start ePWM10

    AdcbRegs.ADCSOC0CTL.bit.CHSEL = 3;              // ADCINB3 - Isec2
    AdcbRegs.ADCSOC0CTL.bit.ACQPS = acqps;
    AdcbRegs.ADCSOC0CTL.bit.TRIGSEL = 0x17;         // Start ePWM10

    AdcbRegs.ADCSOC1CTL.bit.CHSEL = 4;              // ADCINB4 - Vdcsec
    AdcbRegs.ADCSOC1CTL.bit.ACQPS = acqps;
    AdcbRegs.ADCSOC1CTL.bit.TRIGSEL = 0x17;         // Start ePWM10

    AdccRegs.ADCSOC0CTL.bit.CHSEL = 3;              // ADCINC3 - Isec3
    AdccRegs.ADCSOC0CTL.bit.ACQPS = acqps;
    AdccRegs.ADCSOC0CTL.bit.TRIGSEL = 0x17;         // Start ePWM10

    // SOC0 maior prioridade por default, gera flag EOC (End of Conversion)
    // Cada SOC gera um RESULT, comeca RESULT0

    // Interrupcao pï¿½s SOC0 do ADCA

    AdcaRegs.ADCINTSEL1N2.bit.INT1SEL = 0x01;       // InterrupcaoSOC1 gera INT1E (ADCA)
    AdcaRegs.ADCINTSEL1N2.bit.INT1E = 1;            // Habilita flag INT1
    AdcaRegs.ADCINTFLGCLR.bit.ADCINT1 = 1;          // Limpa flag INT1

//    AdcbRegs.ADCINTSEL1N2.bit.INT1SEL = 0x01;       // Interrupcao SOC1 gera INT1E (ADCB)
//    AdcbRegs.ADCINTSEL1N2.bit.INT1E = 1;            // Habilita flag INT1
//    AdcbRegs.ADCINTFLGCLR.bit.ADCINT1 = 1;          // Limpa flag INT1
//
//    AdccRegs.ADCINTSEL1N2.bit.INT1SEL = 0x00;       // Interrupcao SOC0 gera INT1E (ADCC)
//    AdccRegs.ADCINTSEL1N2.bit.INT1E = 1;            // Habilita flag INT1
//    AdccRegs.ADCINTFLGCLR.bit.ADCINT1 = 1;          // Limpa flag INT1

    EDIS;
}

/*
 * @brief Closed loop function
 *
 * Isec_SOC: current measure from interruption
 * Vdcprim_SOC: voltage measure from interruption
 *
 * @return u[0] -> Control Signal
 */
uint16_t Float_Control (uint16_t Current, uint16_t Voltage)
{
    er[0] = ref - (float) Voltage; // casting para tratar variável sem sinak
    k[0] = (1.9052*k[1])-(0.9052*k[2])+(0.001224*er[0])+(0.00004326*er[1])-(0.001181*er[2]);

    if(k[0]>200) k[0] = 200; //limitação de corrente, depende do ponto de operação

//    e[0] = k[0] - Current;        // retirado int(k[0])
    e[0] = 153 - (float) Current;        // casting para tratar variável sem sinak
    u[0] = ((0.5016*u[1])+(0.4984*u[2])+(0.7596*e[0])+(0.04858*e[1])-(0.71097*e[2])); //nova eq a diferenças
    //u[0] = ((0.8654*u[1])+(0.1346*u[2])+(0.3856*e[0])+(0.02497*e[1])-(0.3606*e[2]));
    aux =u[0];

    if(u[0] <= 0) u[0] = 0;
    if(u[0] >= 400) u[0] = 275; //saturação depende do ponto de operação

    u[2]=u[1];
    u[1]=u[0];
    e[2]=e[1];
    e[1]=e[0];
    k[2]=k[1];
    k[1]=k[0];
    er[2]=er[1];
    er[1]=er[0];
    return (uint16_t)u[0];
}

/*
 * @brief ADC Filter
 *
 * Move mean filter with "points" variable
 *
 * @return Signal filtered
 */
uint16_t Filter_ADC1 (uint16_t Scan_SOC1)
{
    for (Aux_points = points-1; Aux_points>0; Aux_points--) Mean[Aux_points] = Mean[Aux_points-1]; //Desloca para esquerda
    Mean[0] = Scan_SOC1; // Novo valor para posicao 0
    Aux_filt = 0;
    for (Aux_points = 0; Aux_points<points; Aux_points++){
        Aux_filt = Aux_filt+Mean[Aux_points]; // Soma
    }
    Filt = Aux_filt/4; // Media
    return Filt;
//    if(Aux_points == 4)
//    {
//        Filt = (int) Aux_filt >> 2;
//        Aux_points = 0;
//    }
//    else
//    {
//        Mean[Aux_points] = Scan_SOC1;                       // Preenche valores do result
//        Aux_filt = Mean[Aux_points] + Aux_filt;     // Faz a soma ponto a ponto
//        Aux_points++;
//    }
//    return Filt;
}
uint16_t Filter_ADC2 (uint16_t Scan_SOC2)
{
    for (Aux_points2 = points-1; Aux_points2>0; Aux_points--) Mean2[Aux_points2] = Mean2[Aux_points2-1]; //Desloca para esquerda
    Mean2[0] = Scan_SOC2; // Novo valor para posicao 0
    Aux_filt2 = 0;
    for (Aux_points2 = 0; Aux_points2<points; Aux_points2++){
        Aux_filt2 = Aux_filt2+Mean[Aux_points]; // Soma
    }
    Filt2 = Aux_filt2/4; // Media
    return Filt2;
}

/*
 * @brief DAC function
 *
 * @return none
 */
void Setup_DAC()
{
    EALLOW;

    DacbRegs.DACCTL.all = 0x0001;               // DACREFSEL in ADC VREF
    DacaRegs.DACCTL.all = 0x0001;               // DACREFSEL in ADC VREF

    DacbRegs.DACVALS.all = 0;                   // DACVALS bits 11-0 (mid-rang)
    DacaRegs.DACVALS.all = 0;                   // DACVALS bits 11-0 (mid-rang)

    DacbRegs.DACOUTEN.bit.DACOUTEN = 1;         // DAC enable
    DacaRegs.DACOUTEN.bit.DACOUTEN = 1;         // DAC enable

    DacbRegs.DACLOCK.all = 0x0000;              // DAC unlock, 1 lock
    DacaRegs.DACLOCK.all = 0x0000;              // DAC unlock, 1 lock

    DELAY_US(10);                               // Delay DAC Power

    EDIS;
}
