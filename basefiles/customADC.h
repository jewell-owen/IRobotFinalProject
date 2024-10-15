/*
 * customADC.h
 *
 *  Created on: Mar 21, 2024
 *      Author: trantk
 */

#ifndef CUSTOMADC_H_
#define CUSTOMADC_H_

#include <stdio.h>
#include <stdint.h>
#include <inc/tm4c123gh6pm.h>
#include <math.h>

void init_ADC0_Seq3(void);

uint32_t ADC0_Seq3(void);

uint32_t translateIR(uint32_t rawIR);

#endif /* CUSTOMADC_H_ */
