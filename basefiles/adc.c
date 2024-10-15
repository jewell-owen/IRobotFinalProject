/*
 * adc.c
 *
 *  Created on: Mar 19, 2024
 *
 */

#include <stdint.h>
#include <inc/tm4c123gh6pm.h>
#include "adc.h"
#include "lcd.h"

void adc_init(void){
    SYSCTL_RCGCADC_R |= 0x0001; // activate ADC0
    SYSCTL_RCGCGPIO_R |= 0b00000010; //activate GPIO B
    while((SYSCTL_PRGPIO_R & 0b00000010) != 0b00000010){}; //wait until GPIO ready
    GPIO_PORTB_DIR_R &= ~0b00001000; //PB4 input
    GPIO_PORTB_AFSEL_R |= 0b00001000; //alternate function PB4 (ADC10)
    GPIO_PORTB_DEN_R &= ~0b00001000; //disable digital IO on PB4
    GPIO_PORTB_AMSEL_R |= 0b00001000; //enable analog PB4
    while((SYSCTL_PRADC_R & 0x0001) != 0x0001){}; //wait until ADC ready
    //ADC0_PC_R &= ~0xF;
    //ADC0_PC_R |= 0x1; //125k samples per sec
    ADC0_SSPRI_R = 0x0123; //sequencer 3 highest priority
    ADC0_ACTSS_R &= ~0x0008; //disable sequencer 3
    ADC0_EMUX_R &= ~0xF000; //seq3 is software trigger
    ADC0_SSMUX3_R &= ~0x000F;
    ADC0_SSMUX3_R += 10; //set channel
    ADC0_SSCTL3_R = 0x0006; // no TS0 D0, yes IE0 END0
    ADC0_IM_R &= ~0x0008; //disable SS3 interupts
    ADC0_ACTSS_R |= 0x0008; //enable sequencer 3
}

uint16_t adc_read(void){
    //wait until active then return
    ADC0_PSSI_R = 0x0008;
    while((ADC0_RIS_R & 0x0008) == 0){
        lcd_printf("waiting");
    }
    return ADC0_SSFIFO3_R & 0xFFF;
}
