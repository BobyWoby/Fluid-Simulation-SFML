//
// Created by bestd on 3/20/2024.
//
#include <cmath>
#include <iostream>
#include "Particle.h"

Particle::Particle(int id, float r, float x, float y, float m, sf::Vector2f v, sf::Color c) {
    this->id = id;
    color = c;
    texture.setOrigin(r,r);
    texture.setRadius(r);
    texture.setFillColor(c);
    texture.setPosition(x, y);
    mass = m;
    velocity = v;
}


void Particle::wallCollision(int width, int height) {
    if(texture.getPosition().x < 0 || texture.getPosition().x > (float)width - texture.getRadius()) {
        if(texture.getPosition().x < 0){
            texture.setPosition(0, texture.getPosition().y);
        }
        else{
            texture.setPosition((float)width - texture.getRadius(), texture.getPosition().y);
        }
        velocity.x *= -.9;
    }
    if((texture.getPosition().y) < 0 || texture.getPosition().y > (float)height - texture.getRadius()) {
        if(texture.getPosition().y < 0){
            texture.setPosition(texture.getPosition().x, 0);
        }
        else{
            texture.setPosition(texture.getPosition().x, (float)height - texture.getRadius());
        }

        velocity.y *= -.9;
    }
}
void Particle::setPosition(float x, float y) {
    texture.setPosition(x, y);
}
Particle::Particle() {

}

void Particle::update() {
    wallCollision(800, 600);
    texture.move(velocity);

}

