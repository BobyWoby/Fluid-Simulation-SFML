//
// Created by bestd on 3/20/2024.
//
#include "Force.h"
#include <iostream>
#include <cmath>
using namespace F;

Force::Force(float dx, float my, bool polar){
    if(polar){
        direction = dx;
        magnitude = my;
    }
    else{
        direction = std::atan(my / dx);
        if(dx <= 0 && my <= 0 || dx <= 0 && my >= 0) {
            direction += M_PI;
        }
        magnitude = (float)std::sqrt(pow(dx, 2) + pow(my, 2));
    }
}

Force Force::add(Force f1) {
    float x = std::cos(f1.getDirection()) * f1.getMagnitude() + std::cos(direction) * magnitude;
    float y = std::sin(f1.getDirection()) * f1.getMagnitude() + std::sin(direction) * magnitude;
    return {x, y, false};
}

float Force::getDirection() {
    return direction;
}
float Force::getMagnitude() {
    return magnitude;
}

void Force::setMagnitude(float m) {
    magnitude = m;
}

void Force::setDirection(float d) {
    direction = d;
}

