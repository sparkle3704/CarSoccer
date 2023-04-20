#ifndef SDL_CONTEXTS_H_INCLUDED
#define SDL_CONTEXTS_H_INCLUDED
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <string>

extern std::string player1_name;
extern std::string player2_name;

extern const std::string fontPath;

extern SDL_Texture* backgroundTexture;
extern SDL_Texture* frontPart;
extern SDL_Texture* scoreBoard;

extern SDL_Texture* car1_Texture;
extern SDL_Texture* car2_Texture;
extern SDL_Texture* ballTexture;

extern float maxPower;

extern SDL_Renderer* renderer;
extern SDL_Surface* screenSurface;
extern TTF_Font* Font;
extern SDL_Window* window;

extern int fullScreen;

extern SDL_Color ORANGE;
extern SDL_Color BLUE;
extern SDL_Color RED;
extern SDL_Color White;

extern void toUpper(std::string& s);

extern SDL_Rect dstRect;

extern SDL_Rect srcRect;

extern bool loadTexture(SDL_Texture*& texture, std::string path);

extern bool loadedAllTextures;

extern bool initTextures();

extern void displayBackground();

extern bool initEverything();

extern void closeEverything();

#endif // SDL_CONTEXTS_H_INCLUDED
