/**
 * lab6_template.c
 *
 * Template file for CprE 288 Lab 6
 *
 * @author Diane Rover, 2/15/2020
 *
 */

#include "Timer.h"
#include "lcd.h"
#include "scan.h"  // For scan sensors
#include "uart-interrupt.h"
#include <stdbool.h>
#include "driverlib/interrupt.h"
#include "open_interface.h"
#include "movement.h"
#include "visual.h"


#define REPLACEME 0

int main(void) {
    timer_init(); // Must be called before lcd_init(), which uses timer functions
    lcd_init();
    uart_interrupt_init();
    servo_init();
    ping_init();
    adc_init();

    oi_t *sensor_data = oi_alloc();
    oi_init(sensor_data);

    int objectLocation[8];
    int objectWidth[8];
    int objectDistance[8];

    scan_t* currScan;

    servo_move(0);
    servo_move(180);

/*
    //TODO
    turn_left(sensor_data, 90);
    move_forward(sensor_data, 700);
    turn_right(sensor_data, 180);
    int sum = move_forward(sensor_data, 700);
    turn_right(sensor_data, 180);
    sum /= 2;
    move_forward(sensor_data, sum);
    turn_right(sensor_data, 90);
    */

    while(1){
        move_forward(sensor_data, 200);
        detectObjects(objectLocation, objectWidth, objectDistance, currScan);
        printf("%d", objectLocation[0]);
    }



    //Lab 7 code below

    oi_free (sensor_data);
};
