/**
 * main.c
 *
 * Bancada do Coversor Boost CCTE - AGT - Malha Fechada
 * Autores: Emanuel Mota e Pedro Herysson
 * Data: 24/06/24
 *
 * Legenda:
 * PORTA -> LABEL       (Localizacao - Funcao)
 *
 * Pinos utilizados:
 *
 * GPIO34 -> LED        (LaunchPad - LED de indicacao)
 *
 * GPIO0 -> PWM1A       (J4 40 - PWM)
 * GPIO1 -> PWM1B       (J4 39 - PWM)
 * GPIO2 -> PWM2A       (J4 38 - PWM)
 * GPIO3 -> PWM2B       (J4 37 - PWM)
 * GPIO4 -> PWM3A       (J4 36 - PWM)
 * GPIO5 -> PWM3B       (J4 35 - PWM)
 * GPIO24 -> RDY1_CREE  (J4 34 - o que faz???)
 * GPIO16 -> FLT1_CREE  (J4 33 - pino de falha do modulo)
 *
 * GPIO6 -> PWM4A       (J8 80 - PWM)
 * GPIO7 -> PWM4B       (J8 79 - PWM)
 * GPIO8 -> PWM5A       (J8 78 - PWM)
 * GPIO9 -> PWM5B       (J8 77 - PWM)
 * GPIO10 -> PWM6A      (J8 76 - PWM)
 * GPIO11 -> PWM6B      (J8 75 - PWM)
 * GPIO14 -> RDY2_CREE  (J8 74 - o que faz???)
 * GPIO15 -> FLT2_CREE  (J8 73 - pino de falha do modulo)
 *
 * ADCINC3 -> Isec3     (DevKit J3 24 - ADC)
 * ADCINB3 -> Isec2     (DevKit J3 25 - ADC)
 * ADCINA3 -> Isec1     (DevKit J3 26 - ADC)
 *
 * ADCINB4 -> Vdcsec    (DevKit J7 68 - ADC)
 * ADCINA4 -> Vdcprim   (DevKit J7 69 - ADC)
 *
 *
 * TODO:
 *  Colocar CMPA como saida do controle
 *  Verificar se 105kHz gerado com triangular
 *  DAC com triangular
 *  Ponto fixo
 *
 */

#include "Perif_Setup.h"

// Definitions

// Functions
__interrupt void isr_adca(void);
__interrupt void isr_adcb(void);
__interrupt void isr_adcc(void);

// Variaveis

uint16_t count_led = 0;

uint16_t Isec1_SOC = 0;             // Variaveis que recebem SOC
uint16_t Isec2_SOC = 0;
uint16_t Isec3_SOC = 0;
uint16_t Vdcsec_SOC = 0;
uint16_t Vdcprim_SOC = 0;

int main(void)
{
    InitSysCtrl();
    // InitPeripheralClocks() desabilitados

    DINT;

    InitPieCtrl();

    IER = 0x0000;       // Desabilita interrupcao
    IFR = 0x0000;

    InitPieVectTable();

    EALLOW;

    PieVectTable.ADCA1_INT = &isr_adca;                      // Att interrupcao ADCA
    PieVectTable.ADCB1_INT = &isr_adcb;                      // Att interrupcao
    PieVectTable.ADCC1_INT = &isr_adcc;                      // Att interrupcao

    PieCtrlRegs.PIEIER1.bit.INTx1 = 1;                      // Habilita coluna ADC

    EDIS;
    IER |= M_INT1;                                          // Habilita linha do ADC

    // Setup GPIO
    Setup_GPIO();

    // Configura PWM
    Setup_ePWM1();
    Setup_ePWM2();
    Setup_ePWM3();
    Setup_ePWM4();
    Setup_ePWM5();
    Setup_ePWM6();
    Setup_ePWM10();

    // Configura ADC
    Setup_ADC();

    EINT;
    ERTM;               // Com real time


    GpioDataRegs.GPBDAT.bit.GPIO34 = 0;

    while(1)
    {
        for (count_led = 0; count_led < 0x00FFFFFF; count_led++)
        {}
        GpioDataRegs.GPBTOGGLE.bit.GPIO34 = 1;
    }
}

// Entra na primeira interrupcao e nao vem mais, erro deve estar na aquisicao
__interrupt void isr_adca(void)
{
    GpioDataRegs.GPBDAT.bit.GPIO32 = 0;
    Isec1_SOC = AdcaResultRegs.ADCRESULT0;                  // SOC0 - ADCINA3
    Vdcprim_SOC = AdcaResultRegs.ADCRESULT1;                // SOC1 - ADCINA4
    Float_Control (Isec1_SOC, Vdcprim_SOC);

    AdcaRegs.ADCINTFLGCLR.bit.ADCINT1 = 1;                  // Limpa flat
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;

    GpioDataRegs.GPBTOGGLE.bit.GPIO32 = 1;
}

// Nao chega na interrupcao
__interrupt void isr_adcb(void)
{
    Isec2_SOC = AdcbResultRegs.ADCRESULT0;                  // SOC0 - ADCINB3
    Vdcsec_SOC = AdcbResultRegs.ADCRESULT1;                 // SOC1 - ADCINB4

    AdcbRegs.ADCINTFLGCLR.bit.ADCINT1 = 1;                  // Limpa flat
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
}

// Nao chega na interrupcao
__interrupt void isr_adcc(void)
{
    Isec3_SOC = AdccResultRegs.ADCRESULT0;                  // SOC0 - ADCINC3

    AdcbRegs.ADCINTFLGCLR.bit.ADCINT1 = 1;                  // Limpa flat
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
}






