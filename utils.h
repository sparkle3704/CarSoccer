#ifndef UTILS_H_INCLUDED
#define UTILS_H_INCLUDED

#include "SDL2/SDL_ttf.h"
#include <string>
#include <SDL2/SDL_image.h>
#include <vector>
#include <SDL2/SDL_ttf.h>

class SmartText {
public:
    SmartText();
    ~SmartText();

    void free();
    int getX();
    int getY();
    int getW();
    int getH();

    bool valid();
    bool loadFromRenderedText(TTF_Font* font, std::string textureText, SDL_Color textColor);
    bool loadFromFile(std::string path);
    void render(int x, int y);
    void renderLeftMiddle(int x, int y);

private:
    int w;
    int h;
    int x;
    int y;
    SDL_Texture* texture;
};

class SmartSurface {
public:
    SmartSurface(TTF_Font* font, std::string textureText, SDL_Color textColor);
    ~SmartSurface();
    SDL_Surface* GetSDLSurface();
    void free();

private:
    SDL_Surface* m_surface;
};

class LTimer {
public:
  // Initializes variables
  LTimer();

  // The various clock actions
  void start();
  void stop();
  void paused();
  void unpaused();

  // Gets the timer's time
  Uint64 getTicks();

  // Checks the status of the timer
  bool isStarted();
  bool isPaused();

private:
  // The clock time when the timer started
  Uint64 mStartTicks;

  // The ticks stored when the timer was paused
  Uint64 mPausedTicks;

  // The timer status
  bool mPaused;
  bool mStarted;
};

extern std::vector<int> vFonts;
extern std::vector<SDL_Surface*> vSurfaces;

extern std::pair<TTF_Font*, TTF_Font*> FONT[300];


extern void closeFonts();
extern void freeSurfaces();
extern TTF_Font* getFont(int fontSize, bool outline = 0, int thickness = 2);

#endif // UTILS_H_INCLUDED
