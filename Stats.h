#ifndef STATS_H_INCLUDED
#define STATS_H_INCLUDED
#pragma once
#include "Car.h"
#include "Ball.h"

extern int prvScoreA;
extern int prvScoreB;
extern int scoreA;
extern int scoreB;
extern float player2_curSign, player1_curSign, player2_prvSign, player1_prvSign;
extern bool prvDir;

extern void updateStates(Car& car1, Car& car2, Ball& ball);

extern void handleCollisions(Car& car1, Car& car2, Ball& ball);

extern void Render(Car& car1, Car& car2, Ball& ball);

extern void reset();

#endif // STATS_H_INCLUDED
