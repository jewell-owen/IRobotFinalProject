/*
 * scan.h
 *
 *  Created on: Apr 25, 2024
 *      Author: sidrao
 */

#ifndef SCAN_H_
#define SCAN_H_

#include "Timer.h"
#include "button.h"
#include "servo.h"
#include "lcd.h"
#include "ping.h"
#include "adc.h"
#include "driverlib/interrupt.h"
#include <stdint.h>
#include <stdbool.h>
#include <inc/tm4c123gh6pm.h>

typedef struct{
    float sound_dist;  // Distance in cm from PING sensor (cyBOT_Scan returns -1.0 if PING is not enabled)
    int IR_raw_val;    // Raw ADC value from IR sensor (cyBOT_Scan returns -1 if IR is not enabled)
} scan_t;



void cyBOT_Scan(int angle, scan_t* getScan);

#endif /* SCAN_H_ */
