//
// Created by bestd on 3/21/2024.
//

#include "SPH.h"

SPH::SPH(Particle *particles, int numParticles, float kernelRadius, float restDensity, float viscosity) {
    this->particles = particles;
    this->numParticles = numParticles;
    this->kernelRadius = kernelRadius;
    this->restDensity = restDensity;
    this->viscosity = viscosity;

}
