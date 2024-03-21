cmake_minimum_required(VERSION 3.22)
project(FluidSimSFML)
set(CMAKE_CXX_STANDARD 17)

find_package(SFML 2.5 COMPONENTS audio system window REQUIRED)
include_directories(${SFML_INCLUDE_DIR})

add_executable(FluidSimSFML main.cpp
        Force.h
        Force.cpp
        Particle.cpp
        Particle.h
        SPH.cpp
        SPH.h
        SPH.h)

target_link_libraries(FluidSimSFML sfml-network sfml-audio sfml-graphics sfml-window sfml-system)
install(TARGETS FluidSimSFML DESTINATION bin)