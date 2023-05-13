#ifndef PLAYERSCORED_H_INCLUDED
#define PLAYERSCORED_H_INCLUDED
#pragma once
#include <string>
#include "Ball.h"
#include <SDl2/SDL.h>
#include "utils.h"

extern bool displayingScored;
extern bool timer;

int getFontSize(int score);

std::string toString(int x);

extern bool displayingOvertime;

bool printPlayerScored(int player);

bool printReminder();

std::pair<SDL_FRect, float> scorePlayer(int player, int w, int h);

bool displayScore(int score, int player);

extern Uint64 lastTime;
extern Uint64 currentTime;
extern int timeElapsed;
extern int timeLeft;
extern bool countDown;

void displayTime();

bool displayScores();

extern Uint64 currentInMidAir;
extern Uint64 startInMidAir;
extern SDL_Surface* inMidAirSurface;
extern SDL_Texture* inMidAirTexture;

void displayInMidAir(Ball& ball);

extern LTimer timerScored, timerReminder, timerOvertime, timerExtraExplosion;

extern void destroyAndFree();

#endif // PLAYERSCORED_H_INCLUDED
