#ifndef SDL_CONTEXTS_H_INCLUDED
#define SDL_CONTEXTS_H_INCLUDED
#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <string>
#include <vector>
#include "Screens.h"
#include "Structs.h"

extern bool mute;
extern SDL_Color ORANGE;
extern SDL_Color BLUE;
extern SDL_Color RED;
extern SDL_Color WHITE;
extern SDL_Color PURPLE;
extern SDL_Color GREEN;
extern SDL_Color BLACK;

class Gallery {
public:
    Gallery() {}
    Gallery(SDL_Texture* texture, SDL_Texture* texture_unselected = nullptr, std::string name = "", float centerX = -1, float centerY = -1, bool toggle = 0, bool state = 1);

    gameState nextState;
    SDL_Texture* texture;
    SDL_Texture* texture_unselected;
    std::string name;
    SDL_FRect rect;
    int width, height;
    float x, y;
    float centerX, centerY;
    bool state;
    bool toggle;
    void displayImage(bool state = 1);

    bool withinArea(int mouseX, int mouseY);
};

class TextField {
public:
    TextField() {}
    TextField(std::string content, int fontSize = 25, SDL_Color textColor = WHITE, float x= 960, float y = 286, bool middle = 1, Point A = {0, 0}, Point B = {0, 0});
    std::string content;
    int fontSize;
    Point A, B;
    SDL_Color textColor;
    float x, y;
    bool middle;
    SDL_Surface* textSurface;
    SDL_Texture* textTexture;
    Uint64 displayBeginTime;
    int width;
    int height;
    SDL_FRect rect;
    int maxWidth = 3000;
    int maxHeight = 3000;
    void displayText();

    bool withinNameField(int mouseX, int mouseY);
};

extern std::vector<Gallery> titleButtons, optionsButtons, pausedButtons, victoryButtons;
extern TextField namePlayer[3];

extern SDL_Texture* optionsWindow_Texture;
extern std::string player1_name;
extern std::string player2_name;
extern std::string inMidAirText;
extern const std::string fontPath;

extern SDL_Texture* background_Texture;
extern SDL_Texture* frontPart_Texture;
extern SDL_Texture* scoreBoard_Texture;

extern SDL_Texture* car1_Texture;
extern SDL_Texture* car2_Texture;
extern SDL_Texture* ballTexture;

extern SDL_Texture* titleBackground_Texture;
extern float maxPower;

extern SDL_Renderer* renderer;
extern SDL_Surface* screenSurface;
extern TTF_Font* Font;
extern TTF_Font* fontOutline;
extern SDL_Window* window;

extern int fullScreen;

extern SDL_Texture* playButton_Selected_Texture;
extern SDL_Texture* playButton_Unselected_Texture;
extern SDL_Texture* exitButton_Selected_Texture;
extern SDL_Texture* exitButton_Unselected_Texture;
extern SDL_Texture* optionsButton_Selected_Texture;
extern SDL_Texture* optionsButton_Unselected_Texture;
extern SDL_Texture* okButton_Selected_Texture;
extern SDL_Texture* okButton_Unselected_Texture;
extern SDL_Texture* tickButton_Selected_Texture;
extern SDL_Texture* tickButton_Unselected_Texture;
extern SDL_Texture* resumeButton_Selected_Texture;
extern SDL_Texture* resumeButton_Unselected_Texture;
extern SDL_Texture* menuButton_Selected_Texture;
extern SDL_Texture* menuButton_Unselected_Texture;
extern SDL_Texture* replayButton_Selected_Texture;
extern SDL_Texture* replayButton_Unselected_Texture;
extern SDL_Texture* victoryP1_Texture;
extern SDL_Texture* victoryP2_Texture;

extern void displayTitleBackground();
extern void toUpper(std::string& s);

extern SDL_Rect dstRect;

extern SDL_Rect srcRect;

extern bool loadTexture(SDL_Texture*& texture, std::string path);

extern void displayTitleBackground();

extern void displayImage(SDL_Texture* texture, float x = -1, float y = -1, float percent = 1);
extern bool loadedAllTextures;

extern bool initTextures();

extern void displayBackground();

extern bool initEverything();

extern void closeEverything();

#endif // SDL_CONTEXTS_H_INCLUDED
