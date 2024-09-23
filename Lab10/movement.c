/*
 * movement.c
 *
 *  Created on: Feb 1, 2024
 *      Author: trantk
 */

#include "movement.h"
#include "open_interface.h"
#include "scan.h"

double move_forward(oi_t *sensor_data, double distance_mm) {
    double sum = 0; //distance member in oi_t struct is type double

    oi_setWheels(200,200);  //move forward at full speed
    oi_update(sensor_data);

    while (sum < distance_mm) {
        oi_update(sensor_data);
        sum += sensor_data -> distance;
        if(sensor_data -> cliffFrontLeftSignal > 2600 || sensor_data -> cliffFrontRightSignal > 2600 || sensor_data -> cliffLeftSignal > 2600 || sensor_data -> cliffRightSignal > 2600){
            break;
        }
    }

    oi_setWheels(0,0);  //stop

    return sum;

}

double move_backward(oi_t *sensor_data, double distance_mm) {
    double sum = 0; //distance member in oi_t struct is type double

    oi_setWheels(-200,-200);  //move backward at full speed
    oi_update(sensor_data);

    while (sum < distance_mm) {
        oi_update(sensor_data);
        sum -= sensor_data -> distance;
    }

    oi_setWheels(0,0);  //stop

    return sum;

}

double turn_right(oi_t *sensor_data, double degrees) {
    double sum = 0;

    oi_setWheels(-15, 15); //rotate to the right
    oi_update(sensor_data); //zero the sensor

    while (sum < degrees) {
        oi_update(sensor_data);
        sum -= sensor_data -> angle;    //subtracting because clockwise turns are negative
    }

    oi_setWheels(0,0);  //stop

    return sum;
}

double turn_left(oi_t *sensor_data, double degrees) {
    double sum = 0;

    oi_setWheels(15, -15); //rotate to the left
    oi_update(sensor_data);

    while (sum < degrees) {
         oi_update(sensor_data);
         sum += sensor_data -> angle;       //counter-clockwise turns are positive
    }

     oi_setWheels(0,0);  //stop

     return sum;
}

double move_forward_avoid_obstacles(oi_t *sensor_data, double distance_mm) {
    double sum = 0; //distance member in oi_t struct is type double
    int left = 0, right = 0;

    oi_update(sensor_data); //zero the sensor
    while (sum < distance_mm) {
        oi_setWheels(100, 100);       //move forward at 100mm per second

        while(!sensor_data -> bumpLeft && !sensor_data -> bumpRight && sum < distance_mm) {
            oi_update(sensor_data);
            sum += sensor_data -> distance;
        }
        //iRobot has to stop for some reason
        oi_setWheels(0, 0);

        if(sensor_data -> bumpLeft && sum < distance_mm) {     //checking left
            move_backward(sensor_data, 150);
            turn_right(sensor_data, 90);
            move_forward_avoid_obstacles(sensor_data, 250);
            turn_left(sensor_data, 90);
            right = 1;
        }
        else if(sensor_data -> bumpRight && sum < distance_mm) {   //wasn't left so check right, else is to bias the robot to turning right if the center is bumped
            move_backward(sensor_data, 150);
            turn_left(sensor_data, 90);
            move_forward_avoid_obstacles(sensor_data, 250);
            turn_right(sensor_data, 90);
            left = 1;
        }

        //correct line of travel after avoiding object, infinite loops possible beware
        if (right) {
            turn_left(sensor_data, 90);
            move_forward_avoid_obstacles(sensor_data, 250);
            turn_right(sensor_data, 90);
        }
        if (left) {
            turn_right(sensor_data, 90);
            move_forward_avoid_obstacles(sensor_data, 250);
            turn_left(sensor_data, 90);
        }
    }
    return sum;
}

double drive_forward(oi_t *sensor_data, double distance_mm) {
    double sum = 0; //distance member in oi_t struct is type double
    int left = 0, right = 0;

    oi_update(sensor_data); //zeroing the sensors

    oi_setWheels(100, 100);
    while(!sensor_data -> bumpLeft && !sensor_data -> bumpRight && !sensor_data -> cliffFrontLeft && !sensor_data -> cliffFrontRight && sum < distance_mm) {
        oi_update(sensor_data);
        sum += sensor_data -> distance;
    }

    //iRobot has to stop for some reason
}
