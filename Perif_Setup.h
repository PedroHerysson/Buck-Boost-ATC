/*
 * Perif_Setup.h
 *
 *  Created on: 24 de jun de 2024
 *      Author: emanu
 */

#ifndef PERIF_SETUP_H_
#define PERIF_SETUP_H_

#include "F28x_Project.h"

void Setup_GPIO(void);
void Setup_ADC(void);
void Setup_DAC(void);

void Setup_ePWM10(void);
void Setup_ePWM1(void);
void Setup_ePWM2(void);
void Setup_ePWM3(void);
void Setup_ePWM4(void);
void Setup_ePWM5(void);
void Setup_ePWM6(void);

void Off_ePWMs(void);
void On_ePWMs(void);

void Float_Control (uint16_t Isec1_SOC, uint16_t Vdcprim_SOC);

#endif /* PERIF_SETUP_H_ */
