/*
 * visual.c
 *
 *  Created on: Apr 25, 2024
 *      Author: trantk
 */

#include "visual.h"

static int START_DEGREE = 45;
static int END_DEGREE = 135;
static int DEGREE_STEP = 2;
static int IR_THRESHOLD = 900;

//Takes empty arrays and fills them with data for width, location, and distance
void detectObjects(int objectLocation[], int objectWidth[], int objectDistance[], scan_t* currentScan) {
    cyBOT_Scan(0, currentScan); //move the scanner to 0
    timer_waitMillis(2000);

    int objectEdge[10] =
    {   0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    int i = 0, j = 0, objectDetected = 0;
    int IR_raws[3];
    int IR_raw;
    int width;
    for (width = START_DEGREE; width <= END_DEGREE; width+=DEGREE_STEP)
    {
        cyBOT_Scan(width, currentScan);
        IR_raws[0] = currentScan->IR_raw_val;
        cyBOT_Scan(width, currentScan);
        IR_raws[1] = currentScan-> IR_raw_val;

        cyBOT_Scan(width, currentScan);
        IR_raws[2] = currentScan -> IR_raw_val;

        IR_raw = (IR_raws[0] + IR_raws[1] + IR_raws[2]) / 3;

        //edge detection
        if (!objectDetected)
        {
            if (IR_raw > IR_THRESHOLD && j == 0)
            {
                objectDetected = 1;
                objectEdge[0] = width;
                j++;
            }
            else if (j != 0 && IR_raw > IR_THRESHOLD)
            {
                objectDetected = 1;
                objectEdge[j] = width;
                j++;
            }
        }
        if (objectDetected)
        {
            if (IR_raw < IR_THRESHOLD)
            {
                objectDetected = 0;
                objectEdge[j++] = width;
            }
        }
        if (objectDetected && width == 180) objectEdge[j] = 180;
    }

    //finding object width and location
    j = 0;
    for (i = 0; i < 10; i += 2)
    {
        objectLocation[j] = (objectEdge[i] + objectEdge[i + 1]) / 2;
        objectWidth[j] = objectEdge[i + 1] - objectEdge[i];
        j++;
    }

    //finding object distance
    for (i = 0; i < 5; i++)
    {
        if (objectLocation[i] != 0)
        {
            cyBOT_Scan(objectLocation[i], currentScan);
            timer_waitMillis(2000);
            cyBOT_Scan(objectLocation[i], currentScan);

            objectDistance[i] = currentScan -> sound_dist;
        }
    }

    //calculating real width
    for (i = 0; i < 5; i++)
    {
        if (objectWidth[i] != 0)
        {
            objectWidth[i] = objectWidth[i] * 2 * 3.14159 * objectDistance[i] / 360.0;
        }
    }
    return;
}

//Using object data, can the bot drive forward without hitting an object?
int clearPath(int objectLocation[], int objectWidth[], int objectDistance[]) {
    int i = 0;
    while (objectLocation[i] != 0 && i < sizeof(objectLocation)) {
        if (objectLocation[i] <= 90) {
            if (objectDistance[i] * cos(objectLocation[i] * 3.14159 / 180.0) - objectWidth[i] / 2 > 20) return 0;
        } else {
            if (objectDistance[i] * cos((180 - objectLocation[i]) * 3.14159 / 180.0) - objectWidth[i] / 2 > 20) return 0;
        }
    }
    return 1;
}

//Returns the angle for a thin object, -1 if no thin object
int targetWire(int objectLocation[], int objectWidth[]) {
    int i = 0;
    while (objectWidth[i] != 0 && i < sizeof(objectWidth)) {
        if (objectWidth[i] < 9) return objectLocation[i];
        i++;
    }
    return -1;
}
