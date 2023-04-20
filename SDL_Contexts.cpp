#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <string>
#include "SDL_Contexts.h"
#include "Constants.h"
#include "Ball.h"
#include "Car.h"
#include "Sounds.h"
#include <iostream>

std::string player1_name = "Player 1";
std::string player2_name = "Player 2";

const std::string fontPath = "./assets/fonts/usethis/sans.ttf";

SDL_Texture* backgroundTexture = nullptr;
SDL_Texture* frontPart = nullptr;
SDL_Texture* scoreBoard = nullptr;

SDL_Texture* car1_Texture = nullptr;
SDL_Texture* car2_Texture = nullptr;
SDL_Texture* ballTexture = nullptr;

float maxPower = 0;

SDL_Renderer* renderer = nullptr;
SDL_Surface* screenSurface = nullptr;
TTF_Font* Font = nullptr;
SDL_Window* window = nullptr;

int fullScreen = 1;

SDL_Color ORANGE = {244, 145, 89};
SDL_Color BLUE = {79, 128, 236};
SDL_Color RED = {255, 0, 0};
SDL_Color White = {255, 255, 255};

void toUpper(std::string& s) {
    for (auto& c : s) {
        c = toupper(c);
    }
    return;
}

SDL_Rect dstRect = { 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT }; // Set destination rectangle to cover entire window
SDL_Rect srcRect = { 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT }; // Example: render the entire texture

bool loadedAllTextures = 1;

bool loadTexture(SDL_Texture*& texture, std::string path) {
    if (texture == nullptr) {
        texture = IMG_LoadTexture(renderer, path.c_str());
    }
    if (texture == nullptr) {
        loadedAllTextures = 0;
        return 0;
    }
    return 1;
}

bool initTextures() {
    loadTexture(backgroundTexture, "./assets/textures/usable_bg.png");
    loadTexture(frontPart, "./assets/textures/front_part.png");
    loadTexture(scoreBoard, "./assets/textures/score2.png");
    loadTexture(car1_Texture, "./assets/textures/spr_casualcar_0.png");
    loadTexture(car2_Texture, "./assets/textures/car1_red.png");
    loadTexture(ballTexture, "./assets/textures/ggg.png");

    explosionSound = Mix_LoadWAV("./assets/audio/test.wav");
    ballHitSound = Mix_LoadWAV("./assets/audio/ballhitsound.wav");
    boostStartSound = Mix_LoadWAV("./assets/audio/booststart.wav");
    boostEndSound = Mix_LoadWAV("./assets/audio/boostend.wav");

    return loadedAllTextures;
}


void displayBackground() {
    SDL_SetRenderDrawColor(renderer, 221, 160, 221, 255);
    SDL_RenderClear(renderer);

    SDL_RenderCopy(renderer, backgroundTexture, &srcRect, &dstRect); /// Background image
    SDL_RenderCopy(renderer, scoreBoard, &srcRect, &dstRect); /// Scoreboard image
}

bool initEverything() {
    toUpper(player1_name);
    toUpper(player2_name);

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "2");

	if (SDL_Init(SDL_INIT_EVERYTHING) != 0){
		std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
		return 0;
	}
	if (SDL_Init(SDL_INIT_AUDIO) != 0) {
		std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
		return 0;
	}
    window = SDL_CreateWindow("Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    if (window == nullptr){
        std::cerr<< "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 0;
    }
    // SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (renderer == nullptr) {
        SDL_DestroyWindow(window);
        std::cerr << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 0;
    }
    if (TTF_Init() != 0) {
        std::cerr << "TTF_Init Error : " << SDL_GetError() << "\n";
        return 0;
    }
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        std::cerr << "SDL_mixer could not initialize! SDL_mixer Error: " <<  Mix_GetError() << "\n";
        return 0;
    }

    if (initTextures() == 0) {
        return 0;
    }
    screenSurface = SDL_GetWindowSurface(window);
    return 1;
}

void closeEverything() {
//    SDL_DestroyTexture(carB);

    SDL_DestroyRenderer(renderer);
    renderer = NULL;

    SDL_DestroyWindow(window);
    window = NULL;

//    SDL_DestroyTexture(explosionTexture);
//    boostTexture = NULL;

//    SDL_DestroyTexture(car1_Texture);
//    car1_Texture = NULL;
//
//    SDL_DestroyTexture(car2_Texture);
//    car2_Texture = NULL;


	//Free the sound effects
//	Mix_FreeChunk( gScratch );
//	Mix_FreeChunk( gHigh );
//	Mix_FreeChunk( gMedium );
//	Mix_FreeChunk( gLow );
//	gScratch = NULL;
//	gHigh = NULL;
//	gMedium = NULL;
//	gLow = NULL;

	//Free the music
	Mix_FreeChunk(explosionSound);
	explosionSound = NULL;

    SDL_DestroyTexture(ballTexture);
    ballTexture = NULL;

    SDL_DestroyTexture(car1_Texture);
    car1_Texture = NULL;

    SDL_DestroyTexture(car2_Texture);
    car2_Texture = NULL;

    Mix_Quit();
    IMG_Quit();
    SDL_Quit();
}
