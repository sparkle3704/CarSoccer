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
#include <vector>
#include "Screens.h"

std::string player1_name = "Player 1";
std::string player2_name = "Player 2";

const std::string fontPath = "./assets/fonts/usethis/sans.ttf";

SDL_Texture* backgroundTexture = nullptr;
SDL_Texture* titleBackgroundTexture = nullptr;
SDL_Texture* frontPart = nullptr;
SDL_Texture* scoreBoard = nullptr;

SDL_Texture* car1_Texture = nullptr;
SDL_Texture* car2_Texture = nullptr;
SDL_Texture* ballTexture = nullptr;
SDL_Texture* playButton_Selected_Texture = nullptr;
SDL_Texture* playButton_Unselected_Texture = nullptr;
SDL_Texture* exitButton_Selected_Texture = nullptr;
SDL_Texture* exitButton_Unselected_Texture = nullptr;
SDL_Texture* optionsButton_Selected_Texture = nullptr;
SDL_Texture* optionsButton_Unselected_Texture = nullptr;
SDL_Texture* okButton_Selected_Texture = nullptr;
SDL_Texture* okButton_Unselected_Texture = nullptr;
SDL_Texture* optionsWindow = nullptr;

float maxPower = 0;

SDL_Renderer* renderer = nullptr;
SDL_Surface* screenSurface = nullptr;
TTF_Font* Font = nullptr;
TTF_Font* fontOutline = nullptr;
SDL_Window* window = nullptr;

int fullScreen = 1;

SDL_Color ORANGE = {244, 145, 89};
SDL_Color BLUE = {79, 128, 236};
SDL_Color RED = {255, 0, 0};
SDL_Color WHITE = {255, 255, 255};
SDL_Color PURPLE = {162, 25, 255};
SDL_Color GREEN = {124, 252, 0};
SDL_Color BLACK = {0, 0, 0};

void toUpper(std::string& s) {
    for (auto& c : s) {
        c = toupper(c);
    }
    return;
}

TextField::TextField(std::string content, int fontSize, SDL_Color textColor, float x, float y, bool middle, Point A, Point B):
    content(content), fontSize(fontSize), textColor(textColor), x(x), y(y), middle(middle), A(A), B(B) {

    maxWidth = B.x - A.x;
    maxHeight = B.y - A.y;
}

bool TextField::withinNameField(int mouseX, int mouseY) {
    int minX = A.x;
    int minY = A.y;
    int maxX = B.x;
    int maxY = B.y;

    if (minX <= mouseX && mouseX <= maxX && minY <= mouseY && mouseY <= maxY) {
        return 1;
    }
    return 0;
}

void normalize(std::string& s) {
    int firstIndex = -1;
    for (int i = 0; i < (int)s.size(); ++i) {
        if (s[i] != ' ') {
            firstIndex = i;
            break;
        }
    }
    if (firstIndex == -1) {
        s = " ";
    }
    else {
        int another_index = firstIndex;
        int cnt = 0;
        for (; another_index < (int)s.size(); ++cnt, ++another_index) {
            s[cnt] = s[another_index];
        }
        while (!s.empty() && (int)s.size() != cnt) {
            s.pop_back();
        }
    }
}

void TextField::displayText() {
    normalize(content);
    Font = TTF_OpenFont(fontPath.c_str(), fontSize); ///
    textSurface = TTF_RenderText_Blended(Font, content.c_str(), textColor); ///
    Font = nullptr; ///
    TTF_CloseFont(Font); ///

    textTexture = SDL_CreateTextureFromSurface(renderer, textSurface); ///

    width = textSurface->w; ///
    height = textSurface->h; ///

    if (middle) {
        rect = {x - width/ 2, y - height/ 2, width, height};
    }
    else {
        rect = {x, y - height/2, width, height};
    }

    SDL_RenderCopyF(renderer, textTexture, NULL, &rect);
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

void displayBackground() {
    SDL_SetRenderDrawColor(renderer, 221, 160, 221, 255);
    SDL_RenderClear(renderer);

    SDL_RenderCopy(renderer, backgroundTexture, &srcRect, &dstRect); /// Background image
    SDL_RenderCopy(renderer, scoreBoard, &srcRect, &dstRect); /// Scoreboard image
}

void displayTitleBackground() {
    SDL_SetRenderDrawColor(renderer, 221, 160, 221, 255);
    SDL_RenderClear(renderer);

    SDL_RenderCopy(renderer, titleBackgroundTexture, &srcRect, &dstRect); /// titleBackground
}

Gallery::Gallery(SDL_Texture* texture, SDL_Texture* texture_unselected, std::string name, float centerX, float centerY):
    texture(texture), texture_unselected(texture_unselected), name(name), centerX(centerX), centerY(y) {
        SDL_QueryTexture(texture, NULL, NULL, &width, &height);
        if (centerX == -1 && centerY == -1) {
            x = 0, y = 0;
        }
        else {
            x = centerX - (float)width * 0.5;
            y = centerY - (float)height * 0.5;
        }
        rect = {x, y, width, height};
        if (name == "play") {
            nextState = GAMEPLAY;
        }
        else if (name == "exit") {
            nextState = EXIT;
        }
        else if (name == "options") {

        }
}

void Gallery::displayImage(bool state) {
    if (state == 1) {
//        std::cerr << "heyy" << "\n";
        SDL_RenderCopyF(renderer, texture, NULL, &rect);
    }
    else {
//        std::cerr << "nooooooo" << "\n";
        SDL_RenderCopyF(renderer, texture_unselected, NULL, &rect);
    }
}

TextField namePlayer[3];
Gallery playButton, optionsButton, exitButton, okButton;
std::vector<Gallery> mainButtons, optionsButtons;
bool initTextures() {
    loadTexture(backgroundTexture, "./assets/textures/usable_bg.png");
    loadTexture(frontPart, "./assets/textures/front_part.png");
    loadTexture(scoreBoard, "./assets/textures/score2.png");
    loadTexture(car1_Texture, "./assets/textures/spr_casualcar_0.png");
    loadTexture(car2_Texture, "./assets/textures/car1_red.png");
    loadTexture(ballTexture, "./assets/textures/ggg.png");
    loadTexture(titleBackgroundTexture, "./assets/textures/titlescreen.jpg");

    loadTexture(playButton_Selected_Texture, "./assets/textures/play_selected.png");
    loadTexture(playButton_Unselected_Texture, "./assets/textures/play_unselected.png");
    playButton = Gallery(playButton_Selected_Texture, playButton_Unselected_Texture, "play", 960, 830 - 200);

    loadTexture(optionsButton_Selected_Texture, "./assets/textures/options_selected.png");
    loadTexture(optionsButton_Unselected_Texture, "./assets/textures/options_unselected.png");
    optionsButton = Gallery(optionsButton_Selected_Texture, optionsButton_Unselected_Texture, "options", 960, 980 - 200);

    loadTexture(exitButton_Selected_Texture, "./assets/textures/exit_selected.png");
    loadTexture(exitButton_Unselected_Texture, "./assets/textures/exit_unselected.png");
    exitButton = Gallery(exitButton_Selected_Texture, exitButton_Unselected_Texture, "exit", 960, 1130 - 200);

    loadTexture(okButton_Selected_Texture, "./assets/textures/ok_selected.png");
    loadTexture(okButton_Unselected_Texture, "./assets/textures/ok_unselected.png");
    okButton = Gallery(okButton_Selected_Texture, okButton_Unselected_Texture, "ok", 960, 786);

    loadTexture(optionsWindow, "./assets/textures/optionswindow.png");

    explosionSound = Mix_LoadWAV("./assets/audio/test.wav");
    ballHitSound = Mix_LoadWAV("./assets/audio/ballhitsound.wav");
    boostStartSound = Mix_LoadWAV("./assets/audio/booststart.wav");
    boostEndSound = Mix_LoadWAV("./assets/audio/boostend.wav");


    mainButtons = {playButton, optionsButton, exitButton};
    optionsButtons = {okButton};

    namePlayer[1] = TextField("Player 1", 30, WHITE, 690, 375, 0, {685, 347}, {1234, 404});
    namePlayer[2] = TextField("Player 2", 30, WHITE, 690, 588, 0, {685, 560}, {1234, 617});

    return loadedAllTextures;
}


bool Gallery::withinArea(int mouseX, int mouseY) {
    float minX = x;
    float maxX = x + width;
    float minY = y;
    float maxY = y + height;

    return (minX <= mouseX && mouseX <= maxX && minY <= mouseY && mouseY <= maxY);
}

//void displayText(string& text, SDL_Texture*& textTexture, SDL_Surface*& textSurface, float x, float y, bool middle, int fontSize) {
//    Font = TTF_OpenFont(fontPath.c_str(), fontSize); ///
//    textSurface = TTF_RenderText_Blended(Font, text.c_str(), { 255, 255, 255 }); ///
//    Font = nullptr; ///
//    TTF_CloseFont(Font); ///
//
//    textTexture = SDL_CreateTextureFromSurface(renderer, textSurface); ///
//
//    int text_width = textSurface->w; ///
//    int text_height = textSurface->h; ///
//
//    SDL_FRect rect;
//    if (middle) {
//        rect = {x - text_width/ 2, y - text_height/ 2, text_width, text_height};
//    }
//    else {
//        rect = {x, y, text_width, text_height};
//    }
//
//    SDL_RenderCopy(renderer, textTexture, NULL, &rect);
//}

void displayImage(SDL_Texture* texture, float x, float y, float percent) {
    int width, height;
    SDL_QueryTexture(texture, NULL, NULL, &width, &height);

    width *= percent;
    height *= percent;
    if (x == -1 && y == -1) {
        x = 0, y = 0;
    }
    else if (x == -2 && y == -2) {
        x = WINDOW_WIDTH*0.5;
        y = WINDOW_HEIGHT*0.5;
        x -= (float)width * 0.5;
        y -= (float)height * 0.5;
    }
    else {
        x -= (float)width * 0.5;
        y -= (float)height * 0.5;
    }
    SDL_FRect rect = {x, y, width, height};
    SDL_RenderCopyF(renderer, texture, NULL, &rect);
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
