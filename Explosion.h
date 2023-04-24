#ifndef EXPLOSION_H_INCLUDED
#define EXPLOSION_H_INCLUDED
#pragma once
#include "Structs.h"
#include <vector>

extern std::vector<Particle> explosionParticles[3];
void addExplosion(int numParticles, float startX, float startY, int player);

void updateExplosion(int player);

void renderExplosion(int player);


#endif // EXPLOSION_H_INCLUDED
