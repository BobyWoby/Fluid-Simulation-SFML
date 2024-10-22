#include <iostream>
#include <vector>
#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <cmath>
#include "Particle.h"
#include "SPH.h"
#include <random>
#define GRID_COLS 40
#define GRID_ROWS 30

void velocityToColor(Particle *p, sf::Vector2f v){
    sf::Color c;
    float vMag = sqrt(pow(v.x, 2) + pow(v.y, 2));
    c.r =  std::min(200.0, vMag / 5.0 * 255.0);
    c.g = 50;
    c.b = 200.0 - vMag / 5.0 * 255.0;
    p->setColor(c);
}



int main() {
    const int numParticles = 400;
    const int numCols = 20;
    const int radius = 5;
    float kernelRadius = 25;
    const int framerate = 60;
    float gasConstant = 1.5;


    // particleId, gridIndex
    std::pair<int, int> particleHash[numParticles];
    int startIndices[numParticles];

    sf::Font font;
    if(!font.loadFromFile("UbuntuMono-R.ttf")){
        std::cout << "Couldn't load font";
        exit(1);
    }
    sf::Text str("Density at Mouse: ", font, 18);
    str.setPosition(0, 0);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist(400, 410);
    std::uniform_real_distribution<float> dist1(350, 360);
//    std::uniform_int_distribution<> dist(0, 800);
//    std::uniform_int_distribution<> dist1(0, 600);
    sf::RenderWindow window(sf::VideoMode(800, 600), "Fluid Sim");
    window.setPosition(sf::Vector2i(sf::VideoMode::getDesktopMode().width / 2 - 400,
                                    sf::VideoMode::getDesktopMode().height / 2 - 320));

    sf::Vector2i MousePos;
    Particle mouse = Particle(-1, 0, (float)MousePos.x, (float)MousePos.y, 1000, 0, sf::Vector2f(0, 0), sf::Color(0,0,0,0));

    sf::CircleShape circle(kernelRadius);
    circle.setOutlineThickness(1);
    circle.setOutlineColor(sf::Color::White);
    circle.setFillColor(sf::Color::Transparent);
    circle.setOrigin(kernelRadius, kernelRadius);
    float density;

    Particle particles[numParticles];
//    for(int i = 0; i < numParticles; i++){
//        particles[i] = Particle(i, radius,  dist(gen), dist1(gen), 10, sf::Vector2f(0, gravity));
//    }
    for(int i = 0; i < numParticles / sqrt(numParticles); i++){
        for(int j = 0; j <sqrt(numParticles); j++){
//            std::cout << "Particle " << i << "," << j<< ": " << i * 2 * radius  << "," <<  j * 2 * radius << std::endl;
            particles[i * (int)sqrt(numParticles) + j] = Particle(i, radius,  i * 2 * radius + 100, j * 2 * radius + 100, 10, 1, sf::Vector2f(0, 0));
        }
    }
    for(int i = 0; i < numParticles - (int)sqrt(numParticles) * (int)sqrt(numParticles); i++){
        particles[(int)sqrt(numParticles) * (int)sqrt(numParticles) + i] = Particle((int)sqrt(numParticles) * (int)sqrt(numParticles) + i, radius,  i * 2 * radius + 100, ((int)sqrt(numParticles)+1) * 2 * radius + 100, 10, 1, sf::Vector2f(0, 0));
    }

    SPH sph(numParticles, kernelRadius, 1, gasConstant, 1);

    //    Particle particle(5, dist(gen), dist1(gen), 1, sf::Vector2f(0, 9.8));

    sf::Clock clock;
    window.setFramerateLimit(framerate);
    sf::Time t;
    float mouseSwitch = 1;
    float mouseScalar = 10;
    sf::Vector2f mousePressure;
    while(window.isOpen()){
        sf::Event e;
        while(window.pollEvent(e)){
            switch(e.type){
                case sf::Event::Closed:
                    window.close();
                    break;
                case sf::Event::MouseWheelScrolled:
//                    gasConstant += e.mouseWheelScroll.delta;
//                    sph.setGasConstant(gasConstant);
                    kernelRadius += e.mouseWheelScroll.delta;
                    sph.setKernelRadius(kernelRadius);
                    circle.setRadius(kernelRadius);
                    circle.setOrigin(kernelRadius, kernelRadius);
                    break;
                case sf::Event::KeyPressed:
                    switch(e.key.code){
                        case sf::Keyboard::Escape:
                            window.close();
                            break;
                        case sf::Keyboard::V:
                            for(auto & particle : particles){
                                particle.setVelocity(sf::Vector2f(0, 0));
                            }
                            break;
                        case sf::Keyboard::R:
//                            for(int i = 0; i < numParticles / numCols; i++){
//                                for(int j = 0; j < numCols; j++){
//                                    particles[i * numCols + j] = Particle(i * numCols + j, radius,  (float )dist(gen), (float)dist1(gen), 10, sf::Vector2f(0, gravity));
//                                }
//                            }
//                            for(int i = 0; i < numParticles; i++){
//                                    particles[i] = Particle(i, radius,  (float )dist(gen), (float)dist1(gen), 10, sf::Vector2f(0, gravity));
//                            }
                            for(int i = 0; i < numParticles / sqrt(numParticles); i++){
                                for(int j = 0; j <sqrt(numParticles); j++){
                                    particles[i * (int)sqrt(numParticles) + j] = Particle(i, radius,  i * 2 * radius + 100, j * 2 * radius + 100, 10, 1, sf::Vector2f(0, 0));
                                }
                            }
                            for(int i = 0; i < numParticles - (int)sqrt(numParticles) * (int)sqrt(numParticles); i++){
                                particles[(int)sqrt(numParticles) * (int)sqrt(numParticles) + i] = Particle((int)sqrt(numParticles) * (int)sqrt(numParticles) + i, radius,  i * 2 * radius + 100, ((int)sqrt(numParticles)+1) * 2 * radius + 100, 10, 1, sf::Vector2f(0, 0));
                            }
                            break;
                        default:
                            break;
                    }
                    break;
                case sf::Event::MouseButtonPressed:
                    if(e.mouseButton.button == sf::Mouse::Left){
                        // apply a pressure towards the mouse
                        mouseSwitch = 1;
                    }
                    else if (e.mouseButton.button == sf::Mouse::Right){
                        mouseSwitch = -1;
                    }

                    break;
                default:
                    break;
            }
            if(e.type == sf::Event::Closed)
                window.close();
        }
        window.clear(sf::Color::Black);
        //draw stuff here
        MousePos = sf::Mouse::getPosition(window);
        circle.setPosition(MousePos.x, MousePos.y);
        density = sph.getDensity(MousePos.x, MousePos.y, particles);
        str.setString("Density at Mouse: " + std::to_string(density) + "\nPressure Scaler: " + std::to_string(gasConstant));
        sph.updateDensities(particles);
        mouse.setPosition(MousePos.x, MousePos.y);

        for(auto & particle : particles){
//            std::cout << "theres a particle" << std::endl;
            //calculate pressure and add the acceleration to the velocity
            sf::Vector2f pressureForce = sph.CalculatePressureForce(&particle, particles);
            sf::Vector2f viscosityForce = sph.CalculateViscosityForce(&particle, particles);
            mousePressure = sph.CalculateParticlePressure(particle, mouse);
            if(sph.getDensity()[particle.getId()]){
                particle.setVelocity(particle.getVelocity() +  (( pressureForce) / ((sph.getDensity()[particle.getId()]) * framerate)) + (viscosityForce / (sph.getDensity()[particle.getId()] * framerate)));
            }
            particle.setVelocity(particle.getVelocity() + sf::Vector2f(mousePressure.x / framerate, mousePressure.y / framerate));
//
//            particle.setVelocity(particle.getVelocity() + sf::Vector2f(0, gravity / framerate ));

            velocityToColor(&particle, particle.getVelocity());
            particle.update();
            window.draw(particle.getCircle());
        }
        t = clock.restart();
        window.draw(str);
        window.draw(circle);
        window.display();
//        mouseSwitch = 0;
    }
    return 0;
}
