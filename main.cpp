#include <iostream>
#include <vector>
#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <cmath>
#include "Particle.h"
#include <random>

int main() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, 800);
    std::uniform_int_distribution<> dist1(0, 300);
    std::uniform_int_distribution<> forceDist(0, 10);
    sf::RenderWindow window(sf::VideoMode(800, 600), "Fluid Sim");
    window.setPosition(sf::Vector2i(sf::VideoMode::getDesktopMode().width / 2 - 400,
                                    sf::VideoMode::getDesktopMode().height / 2 - 320));

    Particle particle(5, dist(gen), dist1(gen), 1, sf::Vector2f(0, 9.8));

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
        //draw stuff here
        particle.update();
        window.draw(particle.getCircle());
        t = clock.restart();
        window.display();
    }
    return 0;
}
