#ifndef BOOST_H_INCLUDED
#define BOOST_H_INCLUDED
#pragma once
#include "Structs.h"
#include <SDL2/SDL_image.h>
#include <vector>

extern SDL_Texture* boostTexture;
extern std::vector<Particle> boostParticles[3];

void addBoost(int numParticles, Point a, Point b, Point c, Point d, int player);

void updateBoost(int player);

void renderBoost(int player);

#endif // BOOST_H_INCLUDED
