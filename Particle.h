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
    int id;

public:
    Particle();
    Particle(int id, float r, float x, float y, float m, sf::Vector2f v, sf::Color c = sf::Color(50, 50, 255));
    sf::CircleShape getCircle(){ return texture; }
    int getId(){ return id; }
    float getMass(){ return mass; }
    sf::Vector2f getVelocity(){ return velocity; }
    sf::Vector2f getPosition(){ return texture.getPosition(); }
    void setVelocity(sf::Vector2f v){ velocity = v; }
    void update();
    void wallCollision(int width, int height);
};

#endif //FLUIDSIMSFML_PARTICLE_H
