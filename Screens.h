#ifndef SCREENS_H_INCLUDED
#define SCREENS_H_INCLUDED

#include <SDL2/SDL.h>
#include <iostream>

extern bool isRunning;
enum gameState {
    TITLE_SCREEN,
    GAMEPLAY,
    EXIT
};


extern gameState currentState;

extern void handleTitleScreen();

extern void handleGameplay();


#endif // SCREENS_H_INCLUDED
