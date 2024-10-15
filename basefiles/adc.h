/*
 * adc.h
 *
 *  Created on: Mar 19, 2024
 */

#ifndef ADC_H_
#define ADC_H_

#include <stdint.h>

void adc_init(void);

uint16_t adc_read(void);

#endif /* ADC_H_ */
