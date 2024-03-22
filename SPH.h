//
// Created by bestd on 3/21/2024.
//

#ifndef FLUIDSIMSFML_SPH_H
#define FLUIDSIMSFML_SPH_H


#include "Particle.h"

class SPH {
public:
    SPH(int numParticles, float kernelRadius, float restDensity, float gasConstant, float viscosity);
    float Poly6SmoothingKernel(float r, float h); // for density
    float Poly6SmoothingKernelGradient(float r, float h);
    float SpikyKernelGradient(float r, float h); // for pressure force
    void updateDensities(Particle *particles);
    float getDensity(int i){ return densities[i]; }
    float getDensity(int mouseX, int mouseY, Particle *particles);
    float *getDensity(){ return densities; }
    sf::Vector2f CalculatePressureForce(Particle *p, Particle *particles);
    float CalculateSharedPressure(float densityA, float densityB);
    float CalculateProperty(float (*kernel)(float, float), Particle *p, Particle *particles, float *particleProperties);
    sf::Vector2f CalculatePropertyGradient(float (*kernel)(float, float), Particle *p, Particle *particles, float *particleProperties);

    void setKernelRadius(float r){ kernelRadius = r; }
    void updateVelocities(Particle *particles);
private:
    int numParticles;
    float kernelRadius;
    float restDensity;
    float *densities;
    float gasConstant;
    float viscosity;
    float distance(sf::Vector2f a, sf::Vector2f b);

    float CalculatePressure(float density);
};


#endif //FLUIDSIMSFML_SPH_H
