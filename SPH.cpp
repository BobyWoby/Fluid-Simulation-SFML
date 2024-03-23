//
// Created by bestd on 3/21/2024.
//

#include <cmath>
#include "SPH.h"
#include <random>
SPH::SPH(int numParticles, float kernelRadius, float restDensity, float gasConstant, float viscosity) {
//    this->particles = &particles;
    this->numParticles = numParticles;
    this->kernelRadius = kernelRadius;
    this->restDensity = restDensity;
    this->gasConstant = gasConstant;
    this->viscosity = viscosity;
    this->densities = new float[numParticles];
}

float SPH::Poly6SmoothingKernel(float r, float h) {
    if(r > h){
        return 0;
    }
    float volume = (float)((M_PI) * 0.000119999999984 * (float)(pow(h, 8))) / 4;
    return std::max(0.0,  pow((pow(h, 2) - pow(r, 2)), 3)) / volume;
}

//grad((315 (s^2 - x^2)^3)/(64 π s^9)) = (-(945 x (s^2 - x^2)^2)/(32 * M_PI * s^9), 0)
float SPH::Poly6SmoothingKernelGradient(float r, float h) {
    if(r > h){
        return 0;
    }
    float f = (float)std::pow(h, 2) - (float)pow(r, 2);
    float scale = -945 * r * (float)pow(f, 2) / (32 * (float)M_PI * (float)pow(h, 9));
    return scale * r * f * f;
}
//Spiky Kernel: std::max(0.0, 15/(pi * h^6) * pow(h - r, 3));
float SPH::SpikyKernelGradient(float r, float h) {
    if(r > h){
        return 0;
    }
    return -(66666.7f * (h-r) /(float)(pow(h, 4)) / (float)(1.0f));
}
float SPH::SpikyKernel(float r, float h){
    if(r > h){
        return 0;
    }
    float volume = .00003 * std::pow(h,4);
    return std::max(0.0, std::pow(h-r, 2) / volume);

}

float SPH::SpikyCubicKernelGradient(float r, float h) {
    if(r > h){
        return 0;
    }
    return -(4000 * (float)pow(h-r, 2) / (float)pow(h, 4));
}
float SPH::SpikyCubicKernel(float r, float h) {
    if(r > h){
        return 0;
    }
    float volume = 0.00075 * (float)pow(h, 4);
    return pow(h-r, 3) / volume;
}

float SPH::distance(sf::Vector2f a, sf::Vector2f b) {
    return sqrt(pow(a.x - b.x, 2) + pow(a.y - b.y, 2));
}

void SPH::updateDensities(Particle *particles) {
    for(int i = 0; i <= numParticles; i++){
        float density = 0;
        for(int j = 0; j <= numParticles; j++){
            float r = distance(particles[i].getPosition(), particles[j].getPosition());
//            density += particles[j].getMass() * Poly6SmoothingKernel(r, kernelRadius);
            density += particles[j].getMass() * SpikyKernel(r, kernelRadius);
        }
        densities[i] = density;
    }

}

void SPH::updateVelocities(Particle *particles) {
    for(int i = 0; i < numParticles; i++){
        sf::Vector2f pressureForce(0, 0);
        sf::Vector2f viscosityForce(0, 0);
        for(int j = 0; j < numParticles; j++){
            if(i != j){
                sf::Vector2f r = particles[i].getPosition() - particles[j].getPosition();
                float rMag = sqrt(pow(r.x, 2) + pow(r.y, 2));
                pressureForce += (particles[i].getMass() / densities[i]) * (particles[i].getMass() / densities[j]) * (gasConstant * (densities[i] - restDensity) + gasConstant * (densities[j] - restDensity)) *
                        SpikyKernelGradient(rMag, kernelRadius) * r;
                viscosityForce += (particles[j].getMass() / densities[j]) * (particles[j].getVelocity() - particles[i].getVelocity()) *
                        SpikyKernelGradient(rMag, kernelRadius);
            }
        }
        particles[i].setVelocity(particles[i].getVelocity() + pressureForce + viscosityForce);
    }
}

float SPH::getDensity(int mouseX, int mouseY, Particle *particles) {
    float density = 0;
    for(int j = 0; j <= numParticles; j++){
        float r = distance(sf::Vector2f(mouseX, mouseY), particles[j].getPosition());
//        density += particles[j].getMass() * Poly6SmoothingKernel(r, kernelRadius);
        density += particles[j].getMass() * SpikyKernel(r, kernelRadius);
    }
    return density;
}

float SPH::CalculatePressure(float density) {
    return gasConstant * (density - restDensity);
}

sf::Vector2f SPH::CalculatePressureForce(Particle *p, Particle *particles) {
    sf::Vector2f pressureForce(0, 0);
    std::default_random_engine rd;
    std::uniform_real_distribution<> dist(0, gasConstant);
    sf::Vector2f dir;
    float rMag;
    sf::Vector2f r;
    float sharedPressure;
    for(int i = 0; i <= numParticles; i++){
        if(i != p->getId()){

            r = p->getPosition() - particles[i].getPosition();
            rMag = sqrt(pow(r.x, 2) + pow(r.y, 2));
            dir = (rMag != 0)? r / rMag : sf::Vector2f(dist(rd), dist(rd));
            sharedPressure = CalculateSharedPressure(densities[i], densities[p->getId()]);
            pressureForce += particles[i].getMass() * (CalculatePressure(densities[i]) + sharedPressure) / (2 * densities[i]) * SpikyKernelGradient(rMag, kernelRadius) * dir;
        }
    }
    return -pressureForce;
}

float SPH::CalculateProperty(float (*kernel)(float, float), Particle *p, Particle *particles, float* particleProperties) {
    float property = 0;
    for(int i = 0; i <= numParticles; i++){
        float r = distance(p->getPosition(), particles[i].getPosition());
        property += particleProperties[i] * particles[i].getMass() / densities[i] * kernel(r, kernelRadius) ;
    }
    return property;
}
sf::Vector2f SPH::CalculatePropertyGradient(float (*kernel)(float, float), Particle *p, Particle *particles, float* particleProperties) {
    sf::Vector2f gradient(0, 0);
    float rMag;
    float dirX;
    float dirY;
    for(int i = 0; i <= numParticles; i++){
        sf::Vector2f r = p->getPosition() - particles[i].getPosition();
        rMag = sqrt(pow(r.x, 2) + pow(r.y, 2));
        dirX = r.x / rMag;
        dirY = r.y / rMag;
        gradient += -particleProperties[i] * particles[i].getMass() / densities[i] * kernel(rMag, kernelRadius) * sf::Vector2f(dirX, dirY);
    }

    return gradient;
}

float SPH::CalculateSharedPressure(float densityA, float densityB) {
    return (CalculatePressure(densityA) + CalculatePressure(densityB)) / 2;
}
