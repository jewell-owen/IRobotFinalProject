#include "lcd.h"
#include <math.h>
#include "servo.h"
#include "ping.h"
#include "adc.h"
#include "open_interface.h"
#include "Timer.h"
#include "uart_extra_help.h"
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <inc/tm4c123gh6pm.h>
#include "driverlib/interrupt.h"
#include "music.h"
#include "button.h"

extern volatile enum {LOW, HIGH, DONE} state;  //Set by ISR
extern volatile unsigned int rising_time;  //Pulse start time
extern volatile unsigned int falling_time; //Pulse end time
extern volatile char uart_data;
extern volatile char flag;
extern volatile int deg;
extern volatile int direction;
extern volatile float pcv;
extern volatile int right_calibration_val = 313200;
extern volatile int left_calibration_val = 287172;

const int IR_THRESHOLD = 700; //bot 12

const int LEFT_WHEEL_FIX = 0;
const int RIGHT_WHEEL_FIX = 2;

static int CURRENT_ANGLE = 0;

//void turn_counterclockwise(oi_t *sensor, short degrees);
//void turn_clockwise(oi_t *sensor, short degrees);
int oneEightyScan();
void move_backwards(oi_t *sensor);
void move_forward(oi_t *sensor, float millimeters);

/*
section 10
 */



void turn_clockwise(oi_t *sensor, short degrees) {
    char output[50];
    double sumTurnRight = 0.0;
    oi_setWheels(-1 * (30) , 30 + LEFT_WHEEL_FIX);
    oi_update(sensor);

    while ((short) sumTurnRight < degrees - 2) {
        oi_update(sensor);
        sumTurnRight -= sensor->angle;
    }

    oi_setWheels(0, 0);
    sprintf(output, "\n\rTurned right %d degrees", degrees);
    uart_sendStr(output);
    CURRENT_ANGLE += degrees;
}

void turn_counterclockwise(oi_t *sensor, short degrees) {
    char output[50];
    double sumTurnLeft = 0;
    oi_setWheels(30, -1 * (30 + LEFT_WHEEL_FIX));
    oi_update(sensor);

    while ((short) sumTurnLeft < degrees - 2) {
        oi_update(sensor);
        sumTurnLeft += sensor->angle;
    }

    oi_setWheels(0, 0);
    sprintf(output, "\n\rTurned left %d degrees", degrees);
    uart_sendStr(output);
    CURRENT_ANGLE -= degrees;

}


//Stores all of the information about each object scanned
typedef struct {
    short object;
    short start_angle;
    short end_angle;
    short mid_angle;
    double ping_distance;
    short adc_distance;
    double linear_width;
    char type;
} objects;

int main(void) {
    //Initializations
    oi_t *sensor_data = oi_alloc();
    oi_init(sensor_data);

    timer_init();
    lcd_init();
    adc_init();
    uart_init(115200);
    uart_interrupt_init();
    ping_init();
    button_init();
    servo_init();

    SERVO_LOW = 22043;
    SERVO_HIGH = 52214;

    sound_load_marchTheme();

    //
    //servo_cal();
    //

    /*      to adjust IR_THRESHOLD based on bot
    while(1){
    char output1[40];
    sprintf(output1, "%d", adc_read());
    lcd_printf(output1);
    timer_waitMillis(100);
    }
    */

/*
         // to test servo calibration
    servo_move(0);
    servo_move(180);
    servo_move(90);
    servo_move(45);
    servo_move(135);
    servo_move(90);
    servo_move(0);
    */

    //move_forward(sensor_data, 1000);
    //while(1);

    /*char output[40];
    while(1){
        oi_update(sensor_data);
        sprintf(output, "%d", sensor_data -> cliffLeftSignal);
        lcd_printf(output);
    }*/


    while (1) {
        oi_update(sensor_data);

        //Conditions to override manual control: hole, boundary, or small object
        //If there is a hole, then avoid falling into it
        if (sensor_data->cliffFrontLeftSignal < 500 ||  sensor_data->cliffFrontRightSignal < 500||  sensor_data->cliffLeftSignal < 500 || sensor_data->cliffRightSignal < 500) {
            uart_sendStr("\n\rOVERRIDE: Hole in the way!");
            oi_setWheels(0, 0);

            //If hole is on left side of the bot, turn right
           if (sensor_data->cliffLeftSignal < sensor_data->cliffRightSignal || sensor_data->cliffFrontLeftSignal < sensor_data->cliffFrontRightSignal) {
               move_backwards(sensor_data);
               uart_sendStr("\n\rHit Hole Left");
               //turn_clockwise(sensor_data, 90);
           }

           //Otherwise, turn left
           else {
               move_backwards(sensor_data);
               uart_sendStr("Hit Hole Right");
               //turn_counterclockwise(sensor_data, 90);
           }
        }

        //If there is a boundary, move backwards and stay in the course


        if (sensor_data->cliffFrontLeftSignal > 2700 || sensor_data->cliffFrontRightSignal > 2700 || sensor_data->cliffLeftSignal > 2700 || sensor_data->cliffRightSignal > 2700) {


           if (sensor_data->cliffLeftSignal < sensor_data->cliffRightSignal || sensor_data->cliffFrontLeftSignal < sensor_data->cliffFrontRightSignal) {
               move_backwards(sensor_data);
               uart_sendStr("\n\rOVERRIDE: Hit the boundary on the right!");
               //turn_counterclockwise(sensor_data, 90);

           }
           else {
               move_backwards(sensor_data);
               uart_sendStr("\n\rOVERRIDE: Hit the boundary on the left!");
               //turn_clockwise(sensor_data, 90);

           }
        }

        //If the bot bumps into a small object on the left, move around it to the right
        if (sensor_data->bumpLeft) {
            uart_sendStr("\n\rOVERRIDE: Bumped into object on the left side!");
            move_backwards(sensor_data);
            oi_update(sensor_data);
            turn_clockwise(sensor_data, 90);
        }

        //If the bot bumps into a small object on the right, move around it to the left
        else if (sensor_data->bumpRight) {
            uart_sendStr("\n\rOVERRIDE: Bumped into object on the right side!");
            move_backwards(sensor_data);
            oi_update(sensor_data);
            turn_counterclockwise(sensor_data, 90);
        }

        //If a key was pressed in Putty, respond accordingly
        if (flag) {
            //Send character received to the Putty
            //uart_sendChar(uart_data);

            //If the character is 'w', drive forward
            if (uart_data == 'w') {
                //oi_setWheels(150, 150);
                move_forward(sensor_data, 200);

            }

            if(uart_data == 'e'){
                move_forward(sensor_data, 1);
            }

            //If the character is 'c', drive backwards
            else if (uart_data == 's') {
                //oi_setWheels(-150, -150);
                oi_update(sensor_data);
                timer_waitMillis(500);
                move_backwards(sensor_data);

            }

            //If the character is 'a', turn left
            else if (uart_data == 'a') {

                turn_counterclockwise(sensor_data, 15);
            }

            //If the character is 'd', turn right
            else if (uart_data == 'd') {
                turn_clockwise(sensor_data, 15);

            }


            //If the character is 'f', turn left 90
            else if (uart_data == 'f') {

                turn_counterclockwise(sensor_data, 90);
                //oi_setWheels(400, -400);
                //timer_waitMillis(500);
               // oi_setWheels(0, 0);
            }

            //If the character is 'g', turn right 90
            else if (uart_data == 'g') {
//                oi_setWheels(-400, 400);
//                timer_waitMillis(500);
//                oi_setWheels(0, 0);
                turn_clockwise(sensor_data, 90);
            }

            else if (uart_data == 'l') {
               sound_play_marchTheme();
            }

            //If the character is 'm', stop the bot and do a 180 degree scan
            else if (uart_data == 'm') {
                oi_setWheels(0, 0);
                int objs = oneEightyScan(sensor_data);

                //If objs is 1, a objective is found, if 2, first object was found
                if (objs) {
                    lcd_printf("OBJECTIVE FOUND!");
                    lcd_home();
                    uart_sendStr("\n\rLocated objective");
                }
                else if (objs == 2) {
                    lcd_printf("Located object.");
                    lcd_home();
                    uart_sendStr("\n\rLocated object");
                }
            }else if(uart_data == 't'){
                servo_move(90);
                timer_waitMillis(500);
            }

            else if(uart_data == 'n'){
                char output[10];
                sprintf(output, "%d", CURRENT_ANGLE);
                uart_sendStr(output);
            }

//            else if (uart_data == 'o') {
//                  oi_loadSong(MARIO_UNDERWATER, 65, 65, 65);
//                  oi_play_song(MARIO_UNDERWATER);
//
//                lcd_printf("EVIL CYBOT!");
//                lcd_home();
//               uart_sendStr("\n\rSong Playing");
//            }

            //If any other key is pressed, stop the bot and do nothing
            else {
                oi_setWheels(0, 0);

            }

            flag = 0;
        }
    }

}



void move_forward(oi_t *sensor, float millimeters) {
    char output[50];
    double sum = 0.0;
    servo_move(90);
    timer_waitMillis(100);
    oi_update(sensor);
    oi_setWheels(100 + RIGHT_WHEEL_FIX, 100 + LEFT_WHEEL_FIX);

    //Move given distance (used in override for hole, boundary, and small object)
    while (sum < millimeters && ping_read() > 10) {


        if (sensor->cliffFrontLeftSignal < 500 ||  sensor->cliffFrontRightSignal < 500||  sensor->cliffLeftSignal < 500 || sensor->cliffRightSignal < 500) {
                uart_sendStr("\n\rOVERRIDE: Hole in the way!");
                oi_setWheels(0, 0);

                //If hole is on left side of the bot, turn right
               if (sensor->cliffLeftSignal < sensor->cliffRightSignal || sensor->cliffFrontLeftSignal < sensor->cliffFrontRightSignal) {
                   move_backwards(sensor);
                   uart_sendStr("\n\rHit Hole Left");
                   //turn_clockwise(sensor_data, 90);
               }

               //Otherwise, turn left
               else {
                   move_backwards(sensor);
                   uart_sendStr("Hit Hole Right");
                   //turn_counterclockwise(sensor_data, 90);
               }
            }

            //If there is a boundary, move backwards and stay in the course


            if (sensor->cliffFrontLeftSignal > 2700 || sensor->cliffFrontRightSignal > 2700 || sensor->cliffLeftSignal > 2700 || sensor->cliffRightSignal > 2700) {


               if (sensor->cliffLeftSignal < sensor->cliffRightSignal || sensor->cliffFrontLeftSignal < sensor->cliffFrontRightSignal) {
                   move_backwards(sensor);
                   uart_sendStr("\n\rOVERRIDE: Hit the boundary on the right!");
                   //turn_counterclockwise(sensor, 90);

               }
               else {
                   move_backwards(sensor);
                   uart_sendStr("\n\rOVERRIDE: Hit the boundary on the left!");
                   //turn_clockwise(sensor, 90);

               }
            }

            //If the bot bumps into a small object on the left, move around it to the right
            if (sensor->bumpLeft) {
                uart_sendStr("\n\rOVERRIDE: Bumped into object on the left side!");
                move_backwards(sensor);
                oi_update(sensor);
                turn_clockwise(sensor, 90);
            }

            //If the bot bumps into a small object on the right, move around it to the left
            else if (sensor->bumpRight) {
                uart_sendStr("\n\rOVERRIDE: Bumped into object on the right side!");
                move_backwards(sensor);
                oi_update(sensor);
                turn_counterclockwise(sensor, 90);
            }



       oi_update(sensor);
       sum += sensor->distance;
    }

    //Stop after moving the specified distance
    oi_setWheels(0, 0);
    sprintf(output, "\n\rMoved forward %.0f cm", sum/10.0);
    uart_sendStr(output);
}

void move_backwards(oi_t *sensor) {
    char output[50];
    double sumBack = 0.0;
    oi_update(sensor);

    //Move backwards 75mm
    oi_setWheels(-100 - RIGHT_WHEEL_FIX,-100 - LEFT_WHEEL_FIX);
    while (sumBack > -75) {
        oi_update(sensor);
        sumBack += sensor->distance;
    }
    oi_setWheels(0,0);
    sprintf(output, "\n\rMoved backwards 7.5 cm");
    uart_sendStr(output);
}

int oneEightyScan(oi_t* sensor_data) {
    int i;
    int objectTypes = 0; //0 is no object, 1 is single object, 2 is a group of objects
    int objectCounts = 0;
    char python[100];

    //Values for displaying data
    char start[] = "Degrees\t\tDistance (cm)\n\r";
    char end[] = "Object#\t\tAngle\t\tPing Distance\tADC Distance\tWidth\tType\n\r";

    uart_sendStr("\n\r");
    uart_sendStr(start);

    //Values to store data collected from the scan
    char layout[100];
    short objectCount = 0;
    objects objectArr[10];

    //Values to note whether and object is being scanned
    int previousVal = 0;
    int previousAngle = 0;
    float curVal;

    //Scan loop for 180 degrees
    for (i = 0; i <= 180; i ++) {
        servo_move(i);
        timer_waitMillis(50);

        //Give bot time to turn
        if (i == 0) {
            timer_waitMillis(500);
        }

        //Store current IR distance in cm
        curVal = 0;
        curVal += adc_read();
        curVal += adc_read();
        curVal += adc_read();
        curVal += adc_read();
        curVal += adc_read();
        curVal += adc_read();
        curVal += adc_read();
        curVal += adc_read();
        curVal += adc_read();
        curVal += adc_read();

        curVal /= 10.0;
        //curVal = 2 * pow(10, 8)* pow(curVal, -2.115) / 4; // cybot 8

        //timer_waitMillis(50);

        //If an object is scanned, store its values
        if (curVal > IR_THRESHOLD && previousVal < IR_THRESHOLD) {
            objectArr[objectCount].object = objectCount + 1;
            objectArr[objectCount].start_angle = i;
            objectArr[objectCount].adc_distance = curVal;
            objectCount++;
        }

        //If object is still being scanned, continue to update width
        else if (curVal < IR_THRESHOLD && previousVal > IR_THRESHOLD) {
            objectArr[objectCount - 1].end_angle = i-1;
        }
       /*  if (curVal > IR_THRESHOLD && i == 180) {
           uart_sendStr("Object Cut Off to the Left");
       }
         if(curVal > IR_THRESHOLD && i == 0){
            uart_sendStr("Object Cut Off to the Right");
        }
        */

        sprintf(layout, "%d\t\t%.3f\n\r", i, curVal);
        uart_sendStr(layout);
        previousVal = curVal;
        previousAngle = i;
    }

    //Find the middle angle, then turn to it and scan the ping distance
    for (i = 0; i < objectCount; i++) {
        objectArr[i].mid_angle = (objectArr[i].start_angle + objectArr[i].end_angle) / 2.0;
        servo_move(objectArr[i].mid_angle);
        timer_waitMillis(500); //Give scanner time to move over
        objectArr[i].ping_distance = 0;
        objectArr[i].ping_distance += ping_read();
        timer_waitMillis(1000);
        //objectArr[i].ping_distance += ping_read();
        //timer_waitMillis(1000);
        objectArr[i].ping_distance /= 1.0;

        if (objectArr[i].ping_distance > 60)
            continue;

        //Calculate the linear width with trig
        //objectArr[i].linear_width = objectArr[i].ping_distance * sin((objectArr[i].end_angle - objectArr[i].start_angle) * (3.14 / 180.0));
        objectArr[i].linear_width = objectArr[i].ping_distance / sin(90-(objectArr[i].end_angle - objectArr[i].mid_angle) * M_PI / 180.0) * sin((objectArr[i].end_angle - objectArr[i].mid_angle) * M_PI / 180.0);
        objectArr[i].linear_width *= 2;

        //Figure out what type of object was scanned (S is small, M is medium, L is large object)
        if (objectArr[i].linear_width <= 8 && objectArr[i].linear_width > 4) {
            objectArr[i].type = 'S';
            objectCounts++;
            /*uart_sendStr("Turned\n");
            if(objectArr[i].mid_angle < 90){
                turn_clockwise(sensor_data, 90-objectArr[i].mid_angle);
            }
            else if(objectArr[i].mid_angle > 90){
                turn_counterclockwise(sensor_data, objectArr[i].mid_angle-90);
            }
            servo_move(90);
            timer_waitMillis(500);
            continue;
            */
            //move_forward(sensor_data, objectArr[i].ping_distance-4); // goes to small object

        }
        else if (objectArr[i].linear_width <= 14.0 && objectArr[i].linear_width > 8) {
            objectArr[i].type = 'M';
            objectCounts++;
        }
        else if (objectArr[i].linear_width > 14.0) {
            objectArr[i].type = 'L';
            objectCounts++;
        }
        else{
            objectArr[i].type = 'N';
            objectCounts++;
            continue;
        }
    }

    //Figure out if it is a single object or the group of objects
    if (objectCounts >= 3) {
        objectTypes = 2;  //Group of objects
    }
    else if (objectCounts < 3 && objectCounts > 0) {
        objectTypes = 1;  //Single object
    }

    //Formatting to print out the objects and their values to Putty
    uart_sendStr("\n\r");
    uart_sendStr(end);
    for (i = 0; i < objectCount; i++) {
        if (objectArr[i].ping_distance > 60)
            continue;
        timer_waitMillis(30);
        sprintf(layout, "%d\t\t%d\t\t%.2f\t\t%d\t\t%.2f\t%c\n\r", objectArr[i].object, objectArr[i].mid_angle, objectArr[i].ping_distance, objectArr[i].adc_distance, objectArr[i].linear_width, objectArr[i].type);
        uart_sendStr(layout);
    }

    for (i = 0; i < objectCount; i++) {
        if (objectArr[i].ping_distance > 60)
            continue;
        sprintf(python,"\n!%d!%0.2f!%c\n\r", objectArr[i].mid_angle, objectArr[i].ping_distance, objectArr[i].type);
        timer_waitMillis(10);
        uart_sendStr(python);
    }

    servo_move(90);

    return objectTypes;
}

/**
 * turn last, after pinging all objects
 * face the servo forward after turning to the small object so we ensure it doesn't crash into the small obj
 * switch vals for light and dark
 */
