//
// Created by bestd on 3/20/2024.
//

#ifndef FLUIDSIMSFML_PARTICLE_H
#define FLUIDSIMSFML_PARTICLE_H

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include "Force.h"



class Particle{
private:
    sf::Color color;
    sf::CircleShape texture; // radius is in m
    sf::Vector2f velocity; // in m/s
    float mass; // in Kg
    float gravity = 9.8;
public:
    Particle();
    F::Force force = F::Force(M_PI * 3/2, gravity);
    Particle(float r, float x, float y, float m, float g, sf::Color c = sf::Color(50, 50, 255));
    sf::CircleShape getCircle(){ return texture; }
    float getMass(){ return mass; }
    void particleCollsiion(Particle& body);
    void updateVelocity();
    void wallCollision(int width, int height);
    void update(int width, int height);
//    static float SmoothingKernel(float kernelRadius, float distance);
    float dotProduct(const sf::Vector2f& v1,const sf::Vector2f& v2);
};

#endif //FLUIDSIMSFML_PARTICLE_H
