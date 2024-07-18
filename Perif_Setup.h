/*
 * Perif_Setup.h
 *
 *  Created on: 24 de jun de 2024
 *      Author: emanu
 */

#ifndef PERIF_SETUP_H_
#define PERIF_SETUP_H_

#include "F28x_Project.h"

// Setup Routines

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

// Protections Routines

void Off_ePWMs(void);

// Control Routines

int Float_Control (uint16_t Current, uint16_t Voltage);
uint16_t Filter_ADC1 (uint16_t Scan_SOC1);
uint16_t Filter_ADC2 (uint16_t Scan_SOC2);

#endif /* PERIF_SETUP_H_ */
