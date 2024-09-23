/*
 * scan.c
 *
 *  Created on: Apr 4, 2024
 *      Author: sidrao
 */

#include <scan.h>

struct scant_t{
    float sound_dist;  // Distance in cm from PING sensor (cyBOT_Scan returns -1.0 if PING is not enabled)
    int IR_raw_val;    // Raw ADC value from IR sensor (cyBOT_Scan returns -1 if IR is not enabled)
};


void cyBOT_Scan(int angle, scan_t* getScan){
    servo_move(angle);
    getScan -> sound_dist = ping_getDistance();
    getScan -> IR_raw_val = adc_read();
}
