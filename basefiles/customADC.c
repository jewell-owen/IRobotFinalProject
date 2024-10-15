/*
 * customADC.c
 *
 *  Created on: Mar 21, 2024
 *      Author: trantk
 */

#include "customADC.h"

void init_ADC0_Seq3(void) {
    SYSCTL_RCGCADC_R |= 0x01;    //ADC0
    SYSCTL_RCGCGPIO_R |= 0x02;   //Enable ADC on port B
    while ((SYSCTL_PRGPIO_R & 0x02) != 0x02) {};
    GPIO_PORTB_DIR_R &= ~0x10;  //Set port 4 as input
    GPIO_PORTB_AFSEL_R |= 0x10; //Alternate function on port 4
    GPIO_PORTB_DEN_R &= ~0x10;  //Disable digital on port 4
    GPIO_PORTB_AMSEL_R |= 0x10; //Enable analog on port 4
    while((SYSCTL_PRADC_R & 0x1) != 0x1) {};

    ADC0_PC_R &= ~0xF;
    ADC0_PC_R |= 0x5;           //500K samples per second
    ADC0_SSPRI_R = 0x0123;      //Sequencer 3
    ADC0_ACTSS_R &= ~0x8;       //Disable sequencer 3 for configuration
    ADC0_EMUX_R &= ~0xF000;     //Software trigger
    ADC0_SSMUX3_R &= ~0xF;
    ADC0_SSMUX3_R |= 0xA;       //Input channel 10
    ADC0_SAC_R |= 0x4;          //Software averaging 16x
    ADC0_SSCTL3_R = 0x6;        //Temp sensor off, interrupt enable, end of sequence, disable differential input
    ADC0_IM_R &= ~0x8;          //Disable interrupts on sequencer 3
    ADC0_ACTSS_R |= 0x8;        //Enable sequencer 3
}

uint32_t ADC0_Seq3(void) {
    uint32_t result;
    ADC0_PSSI_R = 0x8;      //Trigger
    while ((ADC0_RIS_R & 0x8) == 0) {};
    result = ADC0_SSFIFO3_R & 0xFFFF;
    ADC0_ISC_R = 0x8;       //Finish
    return result;
}

uint32_t translateIR(uint32_t rawIR) {
    //edit equation here
    return 7000000*pow(rawIR,-1.703);
}
