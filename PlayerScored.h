#ifndef PLAYERSCORED_H_INCLUDED
#define PLAYERSCORED_H_INCLUDED
#include <string>

#include <SDl2/SDL.h>

extern SDL_Texture* timeTexture;
extern SDL_Surface* textSurface;
extern SDL_Texture* scoreTextureA;
extern SDL_Texture* scoreTextureB;
extern int messageTextWidth[];
extern int messageTextHeight[];

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
#endif // PLAYERSCORED_H_INCLUDED
