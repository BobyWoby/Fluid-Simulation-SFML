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

void resetParticles( int radius, int numParticles, Particle *particles){
    for(int i = 0; i < numParticles / sqrt(numParticles); i++){
        for(int j = 0; j <sqrt(numParticles); j++){
//            std::cout << "Particle " << i << "," << j<< ": " << i * 2 * radius  << "," <<  j * 2 * radius << std::endl;
            particles[i * (int)sqrt(numParticles) + j] = Particle(i, radius,  i * 2 * radius + radius, j * 2 * radius + radius, 10, 100, sf::Vector2f(0, 0));
        }
    }
    for(int i = 0; i < numParticles - (int)sqrt(numParticles) * (int)sqrt(numParticles); i++){
        particles[(int)sqrt(numParticles) * (int)sqrt(numParticles) + i] = Particle((int)sqrt(numParticles) * (int)sqrt(numParticles) + i, radius,  i * 2 * radius + 100, ((int)sqrt(numParticles)+1) * 2 * radius + 100, 10, 1, sf::Vector2f(0, 0));
    }

}


int main() {
    const int numParticles = 529;
    const int width = 400, height = 300;
    const int numCols = 20;
    const int radius = 5;
    float kernelRadius = 25;
    const int framerate = 60;
    float gasConstant = 1.5;
    int mouseInfluence = 5000;
    float mouseRadius = 50;


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
    sf::RenderWindow window(sf::VideoMode(width, height), "Fluid Sim");
    window.setPosition(sf::Vector2i(sf::VideoMode::getDesktopMode().width / 2 - width / 2,
                                    sf::VideoMode::getDesktopMode().height / 2 - height / 2));

    sf::Vector2i MousePos;
    Particle mouse = Particle(-1, 0, (float)MousePos.x, (float)MousePos.y, mouseInfluence, 0, sf::Vector2f(0, 0), sf::Color(0,0,0,0));

    sf::CircleShape circle(kernelRadius);
    circle.setOutlineThickness(1);
    circle.setOutlineColor(sf::Color::White);
    circle.setFillColor(sf::Color::Transparent);
    circle.setOrigin(kernelRadius, kernelRadius);
    float density;

    Particle particles[numParticles];
    resetParticles(radius, numParticles, particles);
    SPH sph(numParticles, kernelRadius, 1, gasConstant, 1);

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
                    mouseInfluence += e.mouseWheelScroll.delta;
                    mouse.setMass(mouseInfluence);
//                    kernelRadius += e.mouseWheelScroll.delta;
//                    sph.setKernelRadius(kernelRadius);
//                    circle.setRadius(kernelRadius);
//                    circle.setOrigin(kernelRadius, kernelRadius);
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
                            resetParticles(radius, numParticles, particles);
                            break;
                        case sf::Keyboard::Up:
                            gasConstant += 0.1;
                            break;
                        case sf::Keyboard::Down:
                            gasConstant -= 0.1;
                            break;

                        default:
                            break;
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
//        str.setString("Density at Mouse: " + std::to_string(density) + "\nPressure Scaler: " + std::to_string(gasConstant));
        str.setString("Density at Mouse: " + std::to_string(density) + "\nMouse Influence: " + std::to_string(mouseInfluence));
        sph.updateDensities(particles);
        mouse.setPosition(MousePos.x, MousePos.y);
        if(sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
            mouseSwitch = -1;
        else if(sf::Mouse::isButtonPressed(sf::Mouse::Button::Right))
            mouseSwitch = 1;
        else mouseSwitch = 0;

        for(auto & particle : particles){
//            std::cout << "theres a particle" << std::endl;
            //calculate pressure and add the acceleration to the velocity
            sf::Vector2f pressureForce = sph.CalculatePressureForce(&particle, particles);
            sf::Vector2f viscosityForce = sph.CalculateViscosityForce(&particle, particles);
            mousePressure = sph.CalculateParticlePressure(particle, mouse, mouseRadius) * mouseSwitch;
            if(sph.getDensity()[particle.getId()]){
                particle.setVelocity(particle.getVelocity() +  (( pressureForce) / ((sph.getDensity()[particle.getId()]) * framerate)) + (viscosityForce / (sph.getDensity()[particle.getId()] * framerate)));
            }
            particle.setVelocity(particle.getVelocity() + sf::Vector2f(mousePressure.x / framerate, mousePressure.y / framerate));

            velocityToColor(&particle, particle.getVelocity());
            particle.update();
            window.draw(particle.getCircle());
        }
        t = clock.restart();
        window.draw(str);
//        window.draw(circle);
        window.display();
        mouseSwitch = 0;
    }
    return 0;
}
