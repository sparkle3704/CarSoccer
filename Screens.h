#ifndef SCREENS_H_INCLUDED
#define SCREENS_H_INCLUDED

#include <SDL2/SDL.h>
#include <iostream>
#pragma once

extern bool isRunning;
extern bool mute;
enum gameState {
    TITLE_SCREEN,
    GAMEPLAY,
    EXIT
};

class TitleScreenInstance {
public:
    TitleScreenInstance() {}
    void handle();

private:
    void init();
    void handleButtons();
    bool onTitleScreen;
};

class optionsWindowInstance {
public:
    optionsWindow() {}
    int nameFieldIndex = 0;
    void setNames();
    void handleEvent();
    void handleButtons();
};


class GameplayInstance {
public:
    GameplayInstance() {}
    void handle();

private:
    void init();
    bool onGameplay;
    void handlePausedButtons();
    bool showingPausedMenu;
};

extern gameState currentState;

extern void handleGameplay();

extern TitleScreenInstance titleScreen;
extern GameplayInstance gameplay;
extern optionsWindowInstance optionsWindow;

#endif // SCREENS_H_INCLUDED
