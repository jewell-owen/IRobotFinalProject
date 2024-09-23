/*
 * servo.c
 *
 *  Created on: Apr 4, 2024
 *      Author: sidrao
 */

#include <servo.h>
#include "Timer.h"
#include "button.h"
#include "driverlib/interrupt.h"

volatile uint32_t SERVO_LOW = 50500;
volatile uint32_t SERVO_HIGH = 80000;

void servo_init(void)
{

    SYSCTL_RCGCGPIO_R |= 0x02;
    SYSCTL_RCGCTIMER_R |= 0x02;
    while ((SYSCTL_PRGPIO_R & 0x02) == 0)
        ;
    while ((SYSCTL_PRTIMER_R & 0x02) == 0)
        ;

    GPIO_PORTB_DEN_R |= 0x20;

    GPIO_PORTB_DIR_R |= 0x20;

    GPIO_PORTB_AFSEL_R |= 0x20;

    GPIO_PORTB_PCTL_R &= ~0xF00000;
    GPIO_PORTB_PCTL_R |= 0x700000;

    TIMER1_CFG_R &= ~0x7;
    TIMER1_CTL_R &= ~0x100;
    TIMER1_CFG_R |= 0x4;
    TIMER1_TBMR_R &= ~0x10;
    TIMER1_TBMR_R |= 0xA;

    TIMER1_TBPR_R = 0x04; //
    TIMER1_TBILR_R = 0xE200; //

    //TIMER1_CTL_R |= 0xC00;

    TIMER1_TBMATCHR_R &= ~0xFFFF;

    TIMER1_TBPMR_R &= ~0xFF;

    TIMER1_CTL_R |= 0x100;

}

void servo_move(uint16_t degrees)
{

    //float millis = (degrees / 180.0 + 1) / 1000;
    //int deg = 320000 - (16000000 * (millis)) - 262144;
    //float deg = ((19-(degrees/180))/(1000*16000000))-262144;

    uint32_t servo_range = SERVO_HIGH - SERVO_LOW;

    TIMER1_TBPMR_R = 0x04;
    TIMER1_TBMATCHR_R = (int)(SERVO_LOW - (degrees * (servo_range / 180.0)));

    timer_waitMillis(1000);

    //while((TIMER1_TBPR_R != TIMER1_TBPMR_R && TIMER1_TBILR_R != TIMER1_TBMATCHR_R));

}

void push_button(int number, int *leftright)
{

    switch (number)
    {
    case 1:
        if (*leftright == 0)
        {
            if (TIMER1_TBMATCHR_R < 41856 - 89)
                TIMER1_TBMATCHR_R += 89;
        }
        else
        {
            if (TIMER1_TBMATCHR_R > 25856 + 89)
                TIMER1_TBMATCHR_R -= 89;
        }
        break;
    case 2:
        if (*leftright == 0)
        {
            if (TIMER1_TBMATCHR_R < 41856 - 445)
                TIMER1_TBMATCHR_R += 445;
        }
        else
        {
            if (TIMER1_TBMATCHR_R > 25856 + 445)
                TIMER1_TBMATCHR_R -= 445;
        }
        break;
    case 3:
        if (*leftright == 0)
        {
            *leftright = 1;
        }
        else
            *leftright = 0;
        break;
    case 4:
        if (*leftright == 1)
        {
            servo_move(175);
        }
        else
            servo_move(5);
    case 0:
        break;
    }

}

