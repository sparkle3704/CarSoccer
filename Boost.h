#ifndef BOOST_H_INCLUDED
#define BOOST_H_INCLUDED
#include "Structs.h"
#include <SDL2/SDL_image.h>

extern SDL_Texture* boostTexture;

void addBoost(int numParticles, Point a, Point b, Point c, Point d, int player);

void updateBoost(int player);

void renderBoost(int player);

#endif // BOOST_H_INCLUDED
