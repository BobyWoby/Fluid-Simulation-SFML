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
    sf::Vector2f position;
    float mass; // in Kg
    float viscosity;
    int id;

public:
    Particle();
    Particle(int id, float r, float x, float y, float m, float vis, sf::Vector2f v, sf::Color c = sf::Color(50, 50, 255));
    sf::CircleShape getCircle(){ return texture; }
    int getId(){ return id; }
    float getMass(){ return mass; }
    float getViscosity() { return viscosity; }
    sf::Vector2f getVelocity(){ return velocity; }
    sf::Vector2f getPosition(){ return texture.getPosition(); }
    void setVelocity(sf::Vector2f v){ velocity = v; }
    void setColor(sf::Color c){ color = c; texture.setFillColor(c);}
    void update();
    void wallCollision(int width, int height);
    void setPosition(float x, float y);
};

#endif //FLUIDSIMSFML_PARTICLE_H
