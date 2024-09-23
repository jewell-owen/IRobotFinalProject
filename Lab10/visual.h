/*
 * visual.h
 *
 *  Created on: Apr 25, 2024
 *      Author: trantk
 */

#ifndef VISUAL_H_
#define VISUAL_H_

#include "scan.h"
#include <stdio.h>
#include "Timer.h"
#include <math.h>

void detectObjects(int objectLocation[], int objectWidth[], int objectDistance[], scan_t* currentScan);
int clearPath(int objectLocation[], int objectWidth[], int objectDistance[]);
int targetWire(int objectLocation[], int objectWidth[]);


#endif /* VISUAL_H_ */
