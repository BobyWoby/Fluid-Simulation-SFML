//
// Created by bestd on 3/21/2024.
//

#ifndef FLUIDSIMSFML_SPH_H
#define FLUIDSIMSFML_SPH_H


#include "Particle.h"

class SPH {
public:
    SPH(int numParticles, float kernelRadius, float restDensity, float gasConstant, float viscosity);

    void updateDensities(Particle *particles);
    float getDensity(int mouseX, int mouseY, Particle *particles);
    float *getDensity(){ return densities; }

    void setGasConstant(float g){ gasConstant = g; }
    void setKernelRadius(float r){ kernelRadius = r; }
    void updateVelocities(Particle *particles);
    float CalculateSharedPressure(float densityA, float densityB);
    float CalculatePressure(float density);
    sf::Vector2f CalculatePressureForce(Particle *p, Particle *particles);
    float CalculateProperty(float (*kernel)(float, float), Particle *p, Particle *particles, float *particleProperties);
    sf::Vector2f CalculatePropertyGradient(float (*kernel)(float, float), Particle *p, Particle *particles, float *particleProperties);
private:
    int numParticles;
    float kernelRadius;
    float restDensity;
    float *densities;
    float gasConstant;
    float viscosity;
    float distance(sf::Vector2f a, sf::Vector2f b);

    static float Poly6SmoothingKernel(float r, float h); // for density
    static float Poly6SmoothingKernelGradient(float r, float h);
    static float SpikyKernelGradient(float r, float h); // for pressure force
    static float SpikyKernel(float r, float h);
    static float SpikyCubicKernelGradient(float r, float h);
    static float SpikyCubicKernel(float r, float h);
};


#endif //FLUIDSIMSFML_SPH_H
