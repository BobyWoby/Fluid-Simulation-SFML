//
// Created by bestd on 3/21/2024.
//

#ifndef FLUIDSIMSFML_SPH_H
#define FLUIDSIMSFML_SPH_H


#include "Particle.h"

class SPH {
public:
    SPH(Particle* particles, int numParticles, float kernelRadius, float restDensity, float viscosity);

private:
    Particle* particles;
    int numParticles;
    float kernelRadius;
    float restDensity;
    float mass;

    float viscosity;

};


#endif //FLUIDSIMSFML_SPH_H
