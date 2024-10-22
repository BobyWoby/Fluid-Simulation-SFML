//
// Created by bestd on 3/21/2024.
//

#include <cmath>
#include "SPH.h"
#include <random>
#include <iostream>

SPH::SPH(int numParticles, float kernelRadius, float restDensity, float gasConstant, float viscosity) {

    this->numParticles = numParticles;
    this->kernelRadius = kernelRadius;
    this->restDensity = restDensity;
    this->gasConstant = gasConstant;
    this->viscosity = viscosity;
    this->densities = new float[numParticles];;
}

void SPH::updateParticleHash(Particle *particles, int numParticles , std::pair<int, int> *particleHash, int *startIndices, std::pair<int, int>){
    startIndices = new int[numParticles];
    std::fill(startIndices, startIndices + numParticles, -1);
    for(int i = 0; i < numParticles; i++){
        int x = (int)particles[i].getPosition().x / (800 / GRID_COLS);
        int y = (int)particles[i].getPosition().y / (600 / GRID_ROWS);
        int cellIndex = y * GRID_COLS + x;
        particleHash[i] = std::make_pair(i, cellIndex);
    }
    //sort particleHash by particle cell index
    for(int i = 0; i < numParticles; i++){
        for(int j = 0; j < numParticles; j++){
            if(particleHash[j].first > particleHash[i].first){
                std::swap(particleHash[j], particleHash[i]);
            }
        }
    }
    //find start indices
    for(int i = 0; i < numParticles; i++){
        if(particleHash[i].first != -1 && startIndices[particleHash[i].second] != i){
            startIndices[particleHash[i].second] = i;
        }
    }
}

float SPH::Poly6SmoothingKernel(float r, float h) {
    if(r >= h || r < 0){
        return 0;
    }
    float volume = M_PI * pow(h, 8) / 8;
//    float scalar =  315 / (64 * M_PI * pow(h, 9));
    float scalar =  2 / (1 * M_PI * pow(h, 8));
    return pow((h * h) - (r * r), 3) / volume;
//    return scalar * pow((h * h) - (r * r), 3);
}

//grad((315 (s^2 - x^2)^3)/(64 π s^9)) = (-(945 x (s^2 - x^2)^2)/(32 * M_PI * s^9), 0)
float SPH::Poly6SmoothingKernelDerivative(float r, float h) {
    if(r > h){
        return 0;
    }
    float f = (float)std::pow(h, 2) - (float)pow(r, 2);
    float scale = -945 * r * (float)pow(f, 2) / (32 * (float)M_PI * (float)pow(h, 9));
    return scale * r * f * f;
}
//Spiky Kernel: std::max(0.0, 15/(pi * h^6) * pow(h - r, 3));
float SPH::SpikyKernelDerivative(float r, float h) {
    if(r >= h || r <= 0){
        return 0;
    }
    float scalar = -45 / (pow(h,6) * M_PI);
    return scalar * pow((h-r), 2);
//    return -(666666.7f * (h-r) /(float)(pow(h, 4)) / (float)(1.0f));
}
float SPH::SpikyKernelGradient(float r, float h) {
    if(r > h || r <= 0){
        return 0;
    }
    float scalar = -90 / (pow(h,6) * M_PI);
    float volume = pow(h, 5) * M_PI / -60;
    return pow(h-r, 2) / volume;
//    return scalar * pow((h-r), 5);
//    return -(666666.7f * (h-r) /(float)(pow(h, 4)) / (float)(1.0f));
}
float SPH::SpikyKernel(float r, float h){
    if(r >= h || r < 0){
        return 0;
    }
    float scalar = 15.0 / (M_PI * pow(h, 6));
    float volume = M_PI * pow(h, 5);
    return pow((h-r), 3) / volume;
//    return scalar * pow((h-r), 3);
}

float SPH::SpikyCubicKernelDerivative(float r, float h) {
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

float SPH::ViscosityLaplace(float r, float h) {
    if(r >= h || r <= 0){
        return 0;
    }
    return 20 * (-(r * r * r / 2 / h / h / h) + (r * r / h / h )+ (h / r / 2) - 1) / 3 / M_PI / h / h;
}

float SPH::ViscosityKernel(float r, float h) {
    if(r >= h ){
        return 0;
    }
    return (-r * r * r / 2 / h / h / h + r * r / h / h + h / 2 / r - 1);
}



float SPH::distance(sf::Vector2f a, sf::Vector2f b) {
    return sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
}

void SPH::updateDensities(Particle *particles) {

    for(int i = 0; i < numParticles; i++){
        float density = 0;
        for(int j = 0; j < numParticles; j++){
            float r = distance(particles[i].getPosition(), particles[j].getPosition());
            density += particles[j].getMass() * Poly6SmoothingKernel(r, kernelRadius);
//            density += particles[j].getMass() * SpikyKernel(r, kernelRadius);
        }
        if(!density){
            std::cout << density  << i << std::endl;
        }
        densities[i] = density;
    }

}

float SPH::getDensity(int mouseX, int mouseY, Particle *particles) {
    float density = 0;
    for(int j = 0; j <= numParticles; j++){
        float r = distance(sf::Vector2f(mouseX, mouseY), particles[j].getPosition());
//        density += particles[j].getMass() * Poly6SmoothingKernel(r, kernelRadius);
        density += particles[j].getMass() * Poly6SmoothingKernel(r, kernelRadius);
    }
    return density;
}

float SPH::CalculatePressure(float density) {
    return gasConstant * (density - restDensity);
}

sf::Vector2f SPH::CalculatePressureForce(Particle *p, Particle *particles) {
    sf::Vector2f pressureForce(0, 0);
    std::default_random_engine rd;
    std::uniform_real_distribution<> dist(0, 360);
    sf::Vector2f dir;
    float rMag;
    sf::Vector2f r;
    float sharedPressure;
    for(int i = 0; i < numParticles; i++){
        if(i != p->getId()){
            r = p->getPosition() - particles[i].getPosition();
            rMag = sqrt(r.x * r.x + r.y * r.y);
            dir = (rMag != 0)? r / rMag : sf::Vector2f(dist(rd), dist(rd));

            sharedPressure = CalculateSharedPressure(densities[i], densities[p->getId()]);
            pressureForce += particles[i].getMass() * (sharedPressure) / (2 * (densities[i])?densities[i]:1) * SpikyKernelGradient(rMag, kernelRadius) * dir;
        }
    }
    return pressureForce;
}

sf::Vector2f SPH::CalculateViscosityForce(Particle *p, Particle *particles) {
    sf::Vector2f viscosityForce(0, 0);
    std::default_random_engine rd;
    std::uniform_real_distribution<> dist(0, gasConstant);
    sf::Vector2f dir;
    float rMag;
    sf::Vector2f r, deltaV;

    for(int i = 0; i < numParticles; i++){
        if(i != p->getId()){
            deltaV = particles[i].getVelocity() - p->getVelocity();
            r = p->getPosition() - particles[i].getPosition();
            rMag = sqrt(r.x * r.x + r.y * r.y);
            dir = (rMag != 0)? r / rMag : sf::Vector2f(dist(rd), dist(rd));
            viscosityForce += particles[i].getMass() * deltaV / densities[i] *  ViscosityLaplace(rMag, kernelRadius);
            viscosityForce.x = viscosityForce.x * dir.x;
            viscosityForce.y = viscosityForce.y * dir.x;
        }
        viscosityForce *= p->getViscosity();
    }


}

sf::Vector2f SPH::CalculateParticlePressure(Particle& particle, Particle& actingParticle, float kR){
    sf::Vector2f r = particle.getPosition() - actingParticle.getPosition();
    float rMag = sqrt(r.x * r.x + r.y * r.y);
    std::default_random_engine rd;
    std::uniform_real_distribution<> dist(0, gasConstant);
    sf::Vector2f dir;
    dir = (rMag != 0)? r / rMag : sf::Vector2f(dist(rd), dist(rd));
    return actingParticle.getMass() * (CalculatePressure(densities[particle.getId()])) / (2 * densities[particle.getId()]) * SpikyKernelGradient(rMag, kR) * dir;
}
float SPH::CalculateProperty(float (*kernel)(float, float), Particle *p, Particle *particles, float* particleProperties) {
    float property = 0;
    for(int i = 0; i <= numParticles; i++){
        float r = distance(p->getPosition(), particles[i].getPosition());
        property += particleProperties[i] * particles[i].getMass() / densities[i] * kernel(r, 100) ;
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
