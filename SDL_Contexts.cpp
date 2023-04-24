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

SDL_Texture* background_Texture = nullptr;
SDL_Texture* titleBackground_Texture = nullptr;
SDL_Texture* frontPart_Texture = nullptr;
SDL_Texture* scoreBoard_Texture = nullptr;

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
SDL_Texture* tickButton_Selected_Texture = nullptr;
SDL_Texture* tickButton_Unselected_Texture = nullptr;
SDL_Texture* optionsWindow_Texture = nullptr;
SDL_Texture* resumeButton_Selected_Texture = nullptr;
SDL_Texture* resumeButton_Unselected_Texture = nullptr;
SDL_Texture* menuButton_Selected_Texture = nullptr;
SDL_Texture* menuButton_Unselected_Texture = nullptr;
SDL_Texture* replayButton_Selected_Texture = nullptr;
SDL_Texture* replayButton_Unselected_Texture = nullptr;
SDL_Texture* victoryP1_Texture = nullptr;
SDL_Texture* victoryP2_Texture = nullptr;

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

    textSurface = nullptr;
    SDL_RenderCopyF(renderer, textTexture, NULL, &rect);
}

SDL_Rect dstRect = { 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT }; // Set destination rectangle to cover entire window
SDL_Rect srcRect = { 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT }; // Example: render the entire texture

bool loadedAll = 1;

bool loadTexture(SDL_Texture*& texture, std::string path) {
    if (texture == nullptr) {
        texture = IMG_LoadTexture(renderer, path.c_str());
    }
    if (texture == nullptr) {
        loadedAll = 0;
        return 0;
    }
    return 1;
}

void displayBackground() {
    SDL_SetRenderDrawColor(renderer, 221, 160, 221, 255);
    SDL_RenderClear(renderer);

    SDL_RenderCopy(renderer, background_Texture, &srcRect, &dstRect); /// Background image
    SDL_RenderCopy(renderer, scoreBoard_Texture, &srcRect, &dstRect); /// Scoreboard image
}

void displayTitleBackground() {
    SDL_SetRenderDrawColor(renderer, 221, 160, 221, 255);
    SDL_RenderClear(renderer);

    SDL_RenderCopy(renderer, titleBackground_Texture, &srcRect, &dstRect); /// titleBackground
}

Gallery::Gallery(SDL_Texture* texture, SDL_Texture* texture_unselected, std::string name, float centerX, float centerY, bool toggle, bool state):
    texture(texture), texture_unselected(texture_unselected), name(name), centerX(centerX), centerY(y), toggle(toggle), state(state) {
        SDL_QueryTexture(texture, NULL, NULL, &width, &height);
        if (centerX == -1 && centerY == -1) {
            x = 0, y = 0;
        }
        else {
            x = centerX - (float)width * 0.5;
            y = centerY - (float)height * 0.5;
        }
        rect = {x, y, width, height};
        if (name == "play" || name == "replay") {
            nextState = GAMEPLAY;
        }
        else if (name == "menu") {
            nextState = TITLE_SCREEN;
        }
        else if (name == "exit") {
            nextState = EXIT;
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

bool loadSoundEffect(Mix_Chunk*& chunk, std::string path) {
    if (chunk == nullptr) {
        chunk = Mix_LoadWAV(path.c_str());
    }
    if (chunk == nullptr) {
        loadedAll = 0;
        return 0;
    }
    return 1;
}

TextField namePlayer[3];

bool initTextures() {
    loadTexture(background_Texture, "./assets/textures/usable_bg.png");
    loadTexture(frontPart_Texture, "./assets/textures/front_part.png");
    loadTexture(scoreBoard_Texture, "./assets/textures/score2.png");
    loadTexture(car1_Texture, "./assets/textures/spr_casualcar_0.png");
    loadTexture(car2_Texture, "./assets/textures/car1_red.png");
    loadTexture(ballTexture, "./assets/textures/ggg.png");
    loadTexture(titleBackground_Texture, "./assets/textures/titlescreen.jpg");
    loadTexture(optionsWindow_Texture, "./assets/textures/optionswindow.png");
    loadTexture(victoryP1_Texture, "./assets/textures/victory_p1.png");
    loadTexture(victoryP2_Texture, "./assets/textures/victory_p2.png");


    loadTexture(playButton_Selected_Texture, "./assets/textures/play_selected.png");
    loadTexture(playButton_Unselected_Texture, "./assets/textures/play_unselected.png");


    loadTexture(optionsButton_Selected_Texture, "./assets/textures/options_selected.png");
    loadTexture(optionsButton_Unselected_Texture, "./assets/textures/options_unselected.png");


    loadTexture(exitButton_Selected_Texture, "./assets/textures/exit_selected.png");
    loadTexture(exitButton_Unselected_Texture, "./assets/textures/exit_unselected.png");


    loadTexture(okButton_Selected_Texture, "./assets/textures/ok_selected.png");
    loadTexture(okButton_Unselected_Texture, "./assets/textures/ok_unselected.png");


    loadTexture(tickButton_Selected_Texture, "./assets/textures/tick_selected.png");
    loadTexture(tickButton_Unselected_Texture, "./assets/textures/tick_unselected.png");

    loadTexture(resumeButton_Selected_Texture, "./assets/textures/resume_selected.png");
    loadTexture(resumeButton_Unselected_Texture, "./assets/textures/resume_unselected.png");

    loadTexture(menuButton_Selected_Texture, "./assets/textures/menu_selected.png");
    loadTexture(menuButton_Unselected_Texture, "./assets/textures/menu_unselected.png");

    loadTexture(replayButton_Selected_Texture, "./assets/textures/replay_selected.png");
    loadTexture(replayButton_Unselected_Texture, "./assets/textures/replay_unselected.png");


    loadSoundEffect(explosionSound, "./assets/audio/test.wav");
    loadSoundEffect(ballHitSound, "./assets/audio/ballhitsound.wav");
    loadSoundEffect(boostStartSound, "./assets/audio/booststart.wav");
    loadSoundEffect(boostEndSound, "./assets/audio/boostend.wav");

    loadSoundEffect(overtimeSound, "./assets/audio/overtime.wav");


    return loadedAll;
}
std::vector<Gallery> titleButtons, pausedButtons, optionsButtons, victoryButtons;
Gallery playButton, optionsButton, exitButton;
Gallery okButton, unlTimeButton,  noGroundButton, sfxButton, musicButton;
Gallery resumeButton, options2Button, menuButton;
Gallery replayButton, options3Button, exit2Button;

void initButtons() {
    playButton = Gallery(playButton_Selected_Texture, playButton_Unselected_Texture, "play", 960, 830 - 200);
    optionsButton = Gallery(optionsButton_Selected_Texture, optionsButton_Unselected_Texture, "options", 960, 980 - 200);
    exitButton = Gallery(exitButton_Selected_Texture, exitButton_Unselected_Texture, "exit", 960, 1130 - 200);

    okButton = Gallery(okButton_Selected_Texture, okButton_Unselected_Texture, "ok", 960, 840); /// 786

    unlTimeButton = Gallery(tickButton_Selected_Texture, tickButton_Unselected_Texture, "unltime", 864, 676, 1, 0);
    noGroundButton = Gallery(tickButton_Selected_Texture, tickButton_Unselected_Texture, "noground", 864, 716, 1, 0);
    sfxButton = Gallery(tickButton_Selected_Texture, tickButton_Unselected_Texture, "sfx", 1226, 676, 1, 1);
    musicButton = Gallery(tickButton_Selected_Texture, tickButton_Unselected_Texture, "music", 1226, 716, 1, 1);

    resumeButton = Gallery(resumeButton_Selected_Texture, resumeButton_Unselected_Texture, "resume", 960, 540 - 150);
    options2Button = Gallery(optionsButton_Selected_Texture, optionsButton_Unselected_Texture, "options", 960, 540);
    menuButton = Gallery(menuButton_Selected_Texture, menuButton_Unselected_Texture, "menu", 960, 540 + 150);

    replayButton = Gallery(replayButton_Selected_Texture, replayButton_Unselected_Texture, "replay", 160, 540 - 150 + 325);
    options3Button = Gallery(optionsButton_Selected_Texture, optionsButton_Unselected_Texture, "options", 160, 540 + 325);
    exit2Button = Gallery(exitButton_Selected_Texture, exitButton_Unselected_Texture, "exit", 160, 540 + 150 + 325);

    titleButtons = {playButton, optionsButton, exitButton};

    optionsButtons = {okButton, unlTimeButton, noGroundButton, sfxButton, musicButton};

    pausedButtons = {resumeButton, options2Button, menuButton};

    victoryButtons = {replayButton, options3Button, exit2Button};


    ///
    namePlayer[1] = TextField("Player 1", 30, WHITE, 690, 375, 0, {685, 347}, {1234, 404});
    namePlayer[2] = TextField("Player 2", 30, WHITE, 690, 588, 0, {685, 560}, {1234, 617});
}

bool Gallery::withinArea(int mouseX, int mouseY) {
    float minX = x;
    float maxX = x + width;
    float minY = y;
    float maxY = y + height;

    return (minX <= mouseX && mouseX <= maxX && minY <= mouseY && mouseY <= maxY);
}

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

    initButtons();
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
