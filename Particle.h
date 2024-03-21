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

public:
    Particle();
    Particle(float r, float x, float y, float m, sf::Vector2f v, sf::Color c = sf::Color(50, 50, 255));
    sf::CircleShape getCircle(){ return texture; }
    float getMass(){ return mass; }
    void update();
    void wallCollision(int width, int height);
};

#endif //FLUIDSIMSFML_PARTICLE_H
