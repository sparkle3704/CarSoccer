#ifndef SCREENS_H_INCLUDED
#define SCREENS_H_INCLUDED

#include <memory>
#include <SDL2/SDL.h>
#include <iostream>
#include <SDL2/SDL_ttf.h>
#pragma once

extern std::shared_ptr<TTF_Font> Font;
extern std::shared_ptr<TTF_Font> fontOutlinet;
extern bool isRunning;
extern bool unlTimeMode;
extern bool inOvertime;
extern bool mute;
enum gameState {
    TITLE_SCREEN,
    GAMEPLAY,
    EXIT,
    VICTORY
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

class OptionsWindowInstance {
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
    bool onGameplay;

private:
    void init();
    void handlePausedButtons();
    bool showingPausedMenu;
};

class VictoryScreenInstance {
public:
    VictoryScreenInstance() {};
    void handle();
    bool init();

private:
    bool onVictoryScreen;
    void handleButtons();
};

extern gameState currentState;

extern void handleGameplay();

extern TitleScreenInstance titleScreen;
extern GameplayInstance gameplay;
extern OptionsWindowInstance optionsWindow;
extern VictoryScreenInstance victoryScreen;
#endif // SCREENS_H_INCLUDED
