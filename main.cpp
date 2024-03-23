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
void updateParticleHash(Particle *particles, int numParticles , std::pair<int, int> *particleHash, std::array<int, GRID_COLS * GRID_ROWS> &startIndices){
    startIndices.fill(-1);
    for(int i = 0; i < numParticles; i++){
        int x = (int)particles[i].getPosition().x / (800 / GRID_COLS);
        int y = (int)particles[i].getPosition().y / (600 / GRID_ROWS);
        int cellIndex = y * GRID_COLS + x;
        particleHash[i] = std::make_pair(i, cellIndex);
    }
    //sort particleHash by particle cell index
    for(int i = 0; i < numParticles; i++){
        for(int j = 0; j < numParticles; j++){
            if(particleHash[j].first > particleHash[i].first){
                std::swap(particleHash[j], particleHash[i]);
            }
        }
    }
    //find start indices
    for(int i = 0; i < numParticles; i++){
        if(particleHash[i].first != -1 && startIndices[particleHash[i].second] != i){
            startIndices[particleHash[i].second] = i;
        }
    }
}
int main() {
    const int numParticles = 600;
    const int numCols = 20;
    const int radius = 5;
    float kernelRadius = 25;
    const float gravity = 0;
    const int framerate = 60;
    float gasConstant = 100;


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
    std::uniform_int_distribution<> dist(0, 800);
    std::uniform_int_distribution<> dist1(0, 600);
    std::uniform_int_distribution<> forceDist(0, 10);
    sf::RenderWindow window(sf::VideoMode(800, 600), "Fluid Sim");
    window.setPosition(sf::Vector2i(sf::VideoMode::getDesktopMode().width / 2 - 400,
                                    sf::VideoMode::getDesktopMode().height / 2 - 320));

    sf::Vector2i MousePos;
    sf::CircleShape circle(kernelRadius);
    circle.setOutlineThickness(1);
    circle.setOutlineColor(sf::Color::White);
    circle.setFillColor(sf::Color::Transparent);
    circle.setOrigin(kernelRadius, kernelRadius);
    float density;

    Particle particles[numParticles];
    for(int i = 0; i < numParticles / numCols; i++){
        for(int j = 0; j < numCols; j++){
//            particles[i * numCols + j] = Particle(i * numCols + j, radius,  j * radius * 2.5, i * radius * 2.5, 10, sf::Vector2f(0, gravity));
            particles[i * numCols + j] = Particle(i * numCols + j, radius,  (float )dist(gen), (float)dist1(gen), 10, sf::Vector2f(0, gravity));
        }
    }

    SPH sph(numParticles, kernelRadius, 200, gasConstant, 1);

    //    Particle particle(5, dist(gen), dist1(gen), 1, sf::Vector2f(0, 9.8));

    sf::Clock clock;
    window.setFramerateLimit(framerate);
    sf::Time t;

    while(window.isOpen()){
        sf::Event e;
        while(window.pollEvent(e)){
            switch(e.type){
                case sf::Event::Closed:
                    window.close();
                    break;
                case sf::Event::MouseWheelScrolled:
                    gasConstant += e.mouseWheelScroll.delta;
                    sph.setGasConstant(gasConstant);
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
        MousePos = sf::Mouse::getPosition(window);
        circle.setPosition(MousePos.x, MousePos.y);
        density = sph.getDensity(MousePos.x, MousePos.y, particles);
        str.setString("Density at Mouse: " + std::to_string(density) + "\nTemperature: " + std::to_string(gasConstant));
        sph.updateDensities(particles);
        for(auto & particle : particles){
            //calculate pressure and add the acceleration to the velocity
            sf::Vector2f pressureForce = sph.CalculatePressureForce(&particle, particles);
            particle.setVelocity( particle.getVelocity() + pressureForce / (sph.getDensity()[particle.getId()] * framerate) );
            particle.setVelocity(particle.getVelocity() + sf::Vector2f(0, gravity / framerate ));

            velocityToColor(&particle, particle.getVelocity());
            particle.update();
            window.draw(particle.getCircle());
        }
        t = clock.restart();
        window.draw(str);
//        window.draw(circle);
        window.display();
    }
    return 0;
}
