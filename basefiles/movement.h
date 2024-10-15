/*
 * movement.h
 *
 *  Created on: Feb 1, 2024
 *      Author: trantk
 */

#ifndef MOVEMENT_H_
#define MOVEMENT_H_

#include "open_interface.h"

double move_forward(oi_t *sensor_data, double distance_mm);

double move_backward(oi_t *sensor_data, double distance_mm);

double turn_right(oi_t *sensor_data, double degrees);

double turn_left(oi_t *sensor_data, double degrees);

double move_forward_avoid_obstacles(oi_t *sensor_data, double distance_mm);

#endif /* MOVEMENT_H_ */
