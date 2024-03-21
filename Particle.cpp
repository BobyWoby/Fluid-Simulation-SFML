//
// Created by bestd on 3/20/2024.
//
#include <cmath>
#include <iostream>
#include "Particle.h"

Particle::Particle(float r, float x, float y, float m, float g, sf::Color c) {
    color = c;
    texture.setOrigin(r,r);
    texture.setRadius(r);
    // Ensure the particles are always initialized within the window dimensions
    x = std::min(std::max(x, r), 800.0f - r);
    y = std::min(std::max(y, r), 600.0f - r);
    texture.setPosition(x, y);
    texture.setFillColor(c);
    mass = m;
    gravity = g;
    force = F::Force(M_PI * 3/2, gravity);
}


void Particle::update(int width, int height) {
    wallCollision(width, height);
    updateVelocity();
    texture.setPosition(texture.getPosition().x + velocity.x, texture.getPosition().y + velocity.y);
}

void Particle::updateVelocity() {
    float ax = force.getMagnitude() * std::cos(force.getDirection());
    float ay = force.getMagnitude() * -std::sin(force.getDirection());
    velocity.x += ax / 60;
    velocity.y += ay / 60;
}
void Particle::wallCollision(int width, int height) {
    if(texture.getPosition().x < 0 || texture.getPosition().x > (float)width - texture.getRadius()) {
        if(texture.getPosition().x < 0){
            texture.setPosition(0, texture.getPosition().y);
        }
        else{
            texture.setPosition((float)width - texture.getRadius(), texture.getPosition().y);
        }
        force = F::Force(M_PI * 3/2, 9.8);
        velocity.x *= -.9;
    }
    if((texture.getPosition().y) < 0 || texture.getPosition().y > (float)height - texture.getRadius()) {
        if(texture.getPosition().y < 0){
            texture.setPosition(texture.getPosition().x, 0);
        }
        else{
            texture.setPosition(texture.getPosition().x, (float)height - texture.getRadius());
        }

        force = F::Force(M_PI * 3/2, 9.8);
        velocity.y *= -.9;
    }
}
void Particle::particleCollsiion(Particle &body) {

    float r1 = texture.getRadius();
    float r2 = body.texture.getRadius();
    float dx = body.texture.getPosition().x - texture.getPosition().x;
    float dy = body.texture.getPosition().y - texture.getPosition().y;
    float buffer = 1.0f;
    float distance = std::sqrt(pow(dx, 2) + pow(dy, 2));
    float damping = 0.9;
    // Ball is colliding
    if(distance < r2 + r1 - buffer){
        force = F::Force(M_PI * 3/2, 9.8);
        body.force = F::Force(M_PI * 3/2, 9.8);
        sf::Vector2f v1 = velocity;
        sf::Vector2f v2 = body.velocity;
        float m1 = mass;
        float m2 = body.mass;

        float v1x = (m1 - m2)/ (m1 + m2) * v1.x + 2 * m2 / (m1 + m2) * v2.x;
        float v1y = (m1 - m2)/ (m1 + m2) * v1.y + 2 * m2 / (m1 + m2) * v2.y;

        float v2x = 2 * m1 / (m1 + m2) * v1.x + (m2 - m1)/ (m1 + m2) * v2.x;
        float v2y = 2 * m1 / (m1 + m2) * v1.y + (m2 - m1)/ (m1 + m2) * v2.y;

        // Calculate the overlap distance and add a small buffer to prevent sticking
        float overlap = r1 + r2 - distance + 1.0f;

        // Calculate the separation vector
        sf::Vector2f separation(dx / distance * overlap, dy / distance * overlap);

        // Separate the particles
        texture.move(-separation / 2.0f);
        body.texture.move(separation / 2.0f);

        velocity.x = v1x * damping;
        velocity.y = v1y * damping;
        body.velocity.x = v2x * damping;
        body.velocity.y = v2y * damping;
        return;
    }
}

float Particle::dotProduct(const sf::Vector2f& a, const sf::Vector2f& b) {
    return a.x * b.x + a.y * b.y;
}
Particle::Particle() {

}

