#include <iostream>
#include <vector>
#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <cmath>
#include "Particle.h"
#include <random>
#define NUM_PARTICLES 500
#define KERNEL_RADIUS 10
//float SmoothingKernel(float kernelRadius, float distance) {
//    float sigma1d = 4 / 3 / kernelRadius;
//    float sigma2d = 40 / (7 * M_PI * kernelRadius * kernelRadius);
//    float sigma3d = 8 / (M_PI * kernelRadius * kernelRadius * kernelRadius);
//
//    float q = std::abs(distance) / kernelRadius;
//    if (0 <= q && q <= .5)
//        return 6.0f * (pow(q, 3) - pow(q, 2)) + 1;
//
//    else if (.5 < q && q <= 1)
//        return 2.0 * pow(1 - q, 3);
//    else
//        return 0;
//}
float SmoothingKernel(float kernelRadius, float distance) {
    float integral = M_PI * std::pow(kernelRadius, 8) / 4;
    float value = std::max(0.0f, kernelRadius * kernelRadius - std::abs(distance) * std::abs(distance));
    return value * value * value / integral;
}
float MassDensityEstimation(Particle* particles, int numParticles, int particleIndex, float kernelRadius) {
    float density = 0;
    float distance = 0;
    for (int i = 0; i < numParticles; i++) {
        distance = std::sqrt(pow(particles[particleIndex].getCircle().getPosition().x - particles[i].getCircle().getPosition().x, 2) +
                             pow(particles[particleIndex].getCircle().getPosition().y - particles[i].getCircle().getPosition().y, 2));
        density += particles[i].getMass() * SmoothingKernel(kernelRadius, distance);
    }
    return density;
}
static float SmoothingKernelDerivative(float kernelRadius, float distance) {
    if(distance >= kernelRadius)
        return 0;
    float f = kernelRadius * kernelRadius - distance * distance;
    float scale = -24 / (M_PI * std::pow(kernelRadius, 8));
    return scale * f * f;

}
float densities[NUM_PARTICLES];
void UpdateDensities(Particle *particles){
    for(int i = 0; i < NUM_PARTICLES; i++){
        densities[i] = MassDensityEstimation(particles, NUM_PARTICLES, i, KERNEL_RADIUS);
    }
}
sf::Vector2f CalculateGradient(Particle* particles, int numParticles, int particleIndex, float kernelRadius) {
    sf::Vector2f gradient = {0, 0};
    float distance = 0;
    float slope = 0;
    float dirScalerX, dirScalerY;
    float density;
    for (int i = 0; i < numParticles; i++) {
        distance = std::sqrt(pow(particles[particleIndex].getCircle().getPosition().x - particles[i].getCircle().getPosition().x, 2) +
                             pow(particles[particleIndex].getCircle().getPosition().y - particles[i].getCircle().getPosition().y, 2));
        dirScalerX = particles[particleIndex].getCircle().getPosition().x - particles[i].getCircle().getPosition().x;
        slope = SmoothingKernelDerivative(kernelRadius, distance);
        density = densities[i];
        gradient += -sf::Vector2f(slope * std::cos(dir), slope * std::sin(dir));
    }
    return gradient;
}

int main() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, 800);
    std::uniform_int_distribution<> dist1(0, 300);
    std::uniform_int_distribution<> forceDist(0, 10);
    sf::RenderWindow window(sf::VideoMode(800, 600), "Fluid Sim");
    window.setPosition(sf::Vector2i(sf::VideoMode::getDesktopMode().width / 2 - 400,
                                    sf::VideoMode::getDesktopMode().height / 2 - 320));

    Particle Particles[NUM_PARTICLES];
    for(int i = 0; i < NUM_PARTICLES / 10; i++){
        for(int j = 0; j < 10; j++){
            Particles[i * 10 + j] = Particle(10, 80 * j, 30 * i, 1, 0);
            Particles[i * 10 + j].force = F::Force(forceDist(gen), forceDist(gen));
//            Particles[i * 10 + j] = Particle(10, dist(gen), dist1(gen), 1, 9.8);
//            Particles[i * 10 + j].force = F::Force(M_PI, 2);
//            Particles[i].force = Particles[i].force.add(F::Force(M_PI, 2));
        }
    }


    sf::Clock clock;
    window.setFramerateLimit(60);
    sf::Time t;

    while(window.isOpen()){
        sf::Event e;
        while(window.pollEvent(e)){
            switch(e.type){
                case sf::Event::Closed:
                    window.close();
                    break;
                case sf::Event::KeyPressed:
                    switch(e.key.code){
                        case sf::Keyboard::Escape:
                            window.close();
                            break;
                        default:
                            break;
                    }
                default:
                    break;
            }
            if(e.type == sf::Event::Closed)
                window.close();
        }
        window.clear(sf::Color::Black);

        for(int i = 0; i < NUM_PARTICLES; i++){
            for(int j = 0; j < NUM_PARTICLES; j++){
                if(i != j){
                    Particles[i].particleCollsiion(Particles[j]);
                }
            }
            float density = MassDensityEstimation(Particles, NUM_PARTICLES, i, KERNEL_RADIUS);
            Particles[i].force.setMagnitude(density * 9.8);

            Particles[i].update(800, 600);
            window.draw(Particles[i].getCircle());
        }
        window.display();
    }
    return 0;
}
