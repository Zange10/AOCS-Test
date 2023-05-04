//
// Created by niklas on 04.05.23.
//

#include "connector.h"
#include "geometry.h"
#include <math.h>

double pitch = 0;
double yaw = 0;
double roll = 0;

double get_pitch() {
    pitch += M_PI/200;
    return pitch;
}

double get_yaw() {
    return yaw;
}

double get_roll() {
    roll -= M_PI/500;
    return roll;
}