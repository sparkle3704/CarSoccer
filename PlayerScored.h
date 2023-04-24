#ifndef PLAYERSCORED_H_INCLUDED
#define PLAYERSCORED_H_INCLUDED
#pragma once
#include <string>
#include "Ball.h"
#include <SDl2/SDL.h>

extern SDL_Texture* timeTexture;
extern SDL_Surface* textSurface;
extern SDL_Texture* scoreTextureA;
extern SDL_Texture* scoreTextureB;
extern int messageTextWidth[];
extern int messageTextHeight[];
extern bool displayingScored;
extern bool timer;

extern SDL_Surface* surfaceMessage;

int getFontSize(int score);

std::string toString(int x);

extern Uint64 scoredBeginTime;
extern SDL_Surface* scoredSurface;
extern SDL_Texture* scoredTexture;
extern SDL_Rect scoredRect;
extern Uint64 reminderBeginTime;
extern SDL_Surface* reminderSurface;
extern SDL_Texture* reminderTexture;
extern SDL_Rect reminderRect;

extern Uint64 overtimeBeginTime;
extern SDL_Surface* overtimeSurface;
extern SDL_Texture* overtimeTexture;
extern SDL_Rect overtimeRect;
extern bool displayingOvertime;

void printPlayerScored(int player);

void printReminder();

std::pair<SDL_FRect, float> scorePlayer(int player, int w, int h);

void displayScore(int score, int player);

extern Uint64 lastTime;
extern Uint64 currentTime;
extern int timeElapsed;
extern int timeLeft;
extern bool countDown;

void displayTime();

void displayScores();

extern Uint64 currentInMidAir;
extern Uint64 startInMidAir;
extern SDL_Surface* inMidAirSurface;
extern SDL_Texture* inMidAirTexture;

void displayInMidAir(Ball& ball);

#endif // PLAYERSCORED_H_INCLUDED
