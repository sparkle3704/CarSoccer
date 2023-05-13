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
#include "unordered_map"
#include "PlayerScored.h"
#include "utils.h"

std::string player1_name = "Player 1";
std::string player2_name = "Player 2";

const std::string fontPath = "./assets/fonts/usethis/sans.ttf";

std::shared_ptr<SDL_Texture> optionsWindow_Texture(nullptr, SDL_DestroyTexture);
std::shared_ptr<SDL_Texture> background_Texture(nullptr, SDL_DestroyTexture);
std::shared_ptr<SDL_Texture> titleBackground_Texture(nullptr, SDL_DestroyTexture);
std::shared_ptr<SDL_Texture> frontPart_Texture(nullptr, SDL_DestroyTexture);
std::shared_ptr<SDL_Texture> scoreBoard_Texture(nullptr, SDL_DestroyTexture);

std::shared_ptr<SDL_Texture> car1_Texture(nullptr, SDL_DestroyTexture);
std::shared_ptr<SDL_Texture> car2_Texture(nullptr, SDL_DestroyTexture);


std::shared_ptr<SDL_Texture> ballTexture(nullptr, SDL_DestroyTexture);
std::shared_ptr<SDL_Texture> playButton_Selected_Texture(nullptr, SDL_DestroyTexture);
std::shared_ptr<SDL_Texture> playButton_Unselected_Texture(nullptr, SDL_DestroyTexture);
std::shared_ptr<SDL_Texture> exitButton_Selected_Texture(nullptr, SDL_DestroyTexture);
std::shared_ptr<SDL_Texture> exitButton_Unselected_Texture(nullptr, SDL_DestroyTexture);
std::shared_ptr<SDL_Texture> optionsButton_Selected_Texture(nullptr, SDL_DestroyTexture);
std::shared_ptr<SDL_Texture> optionsButton_Unselected_Texture(nullptr, SDL_DestroyTexture);
std::shared_ptr<SDL_Texture> okButton_Selected_Texture(nullptr, SDL_DestroyTexture);
std::shared_ptr<SDL_Texture> okButton_Unselected_Texture(nullptr, SDL_DestroyTexture);
std::shared_ptr<SDL_Texture> tickButton_Selected_Texture(nullptr, SDL_DestroyTexture);
std::shared_ptr<SDL_Texture> tickButton_Unselected_Texture(nullptr, SDL_DestroyTexture);
std::shared_ptr<SDL_Texture> resumeButton_Selected_Texture(nullptr, SDL_DestroyTexture);
std::shared_ptr<SDL_Texture> resumeButton_Unselected_Texture(nullptr, SDL_DestroyTexture);
std::shared_ptr<SDL_Texture> menuButton_Selected_Texture(nullptr, SDL_DestroyTexture);
std::shared_ptr<SDL_Texture> menuButton_Unselected_Texture(nullptr, SDL_DestroyTexture);
std::shared_ptr<SDL_Texture> replayButton_Selected_Texture(nullptr, SDL_DestroyTexture);
std::shared_ptr<SDL_Texture> replayButton_Unselected_Texture(nullptr, SDL_DestroyTexture);
std::shared_ptr<SDL_Texture> victoryP1_Texture(nullptr, SDL_DestroyTexture);
std::shared_ptr<SDL_Texture> victoryP2_Texture(nullptr, SDL_DestroyTexture);

float maxPower = 0;

std::shared_ptr<TTF_Font> Font(nullptr, TTF_CloseFont);
std::shared_ptr<TTF_Font> fontOutline(nullptr, TTF_CloseFont);

std::shared_ptr<SDL_Renderer> renderer(nullptr, SDL_DestroyRenderer);
std::shared_ptr<SDL_Surface> screenSurface(nullptr, SDL_FreeSurface);
std::shared_ptr<SDL_Window> window(nullptr, SDL_DestroyWindow);

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
    TTF_Font* Font = getFont(fontSize);
    textSurface.reset(TTF_RenderText_Blended(Font, content.c_str(), textColor), SDL_FreeSurface);

    textTexture.reset(SDL_CreateTextureFromSurface(renderer.get(), textSurface.get()), SDL_DestroyTexture);

    width = textSurface->w;
    height = textSurface->h;

    if (middle) {
        rect = {x - width/ 2, y - height/ 2, width, height};
    }
    else {
        rect = {x, y - height/2, width, height};
    }

    SDL_RenderCopyF(renderer.get(), textTexture.get(), NULL, &rect);
}
SDL_Rect dstRect = { 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT }; // Set destination rectangle to cover entire window
SDL_Rect srcRect = { 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT }; // Example: render the entire texture

bool loadedAll = 1;

std::unordered_map<std::string, std::shared_ptr<SDL_Texture>> textures;

bool loadTexture(std::shared_ptr<SDL_Texture>& texture, std::string path) {
    auto it = textures.find(path);
    if (textures.find(path) == textures.end()) {
        SDL_Texture* raw_texture = IMG_LoadTexture(renderer.get(), path.c_str());
        if (raw_texture == nullptr) {
            std::cerr << "Error with loading texture from: " << path << "\n";
            loadedAll = 0;
            return 0;
        }
        texture.reset(raw_texture, SDL_DestroyTexture);
        textures[path] = texture;
    }
    else {
        texture = it->second;
    }
    return 1;
}

void displayBackground() {
    SDL_SetRenderDrawColor(renderer.get(), 221, 160, 221, 255);
    SDL_RenderClear(renderer.get());

    SDL_RenderCopy(renderer.get(), background_Texture.get(), &srcRect, &dstRect); /// Background image
    SDL_RenderCopy(renderer.get(), scoreBoard_Texture.get(), &srcRect, &dstRect); /// Scoreboard image
}

void displayTitleBackground() {
    SDL_SetRenderDrawColor(renderer.get(), 221, 160, 221, 255);
    SDL_RenderClear(renderer.get());

    SDL_RenderCopy(renderer.get(), titleBackground_Texture.get(), &srcRect, &dstRect); /// titleBackground
}

Gallery::Gallery(std::shared_ptr<SDL_Texture> texture, std::shared_ptr<SDL_Texture> texture_unselected, std::string name, float centerX, float centerY, bool toggle, bool state):
    texture(texture), texture_unselected(texture_unselected), name(name), centerX(centerX), centerY(y), toggle(toggle), state(state) {
        SDL_QueryTexture(texture.get(), NULL, NULL, &width, &height);
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
        SDL_RenderCopyF(renderer.get(), texture.get(), NULL, &rect);
    }
    else {
        SDL_RenderCopyF(renderer.get(), texture_unselected.get(), NULL, &rect);
    }
}

std::vector<Mix_Chunk*> sfxList;
std::vector<Mix_Music*> songList;

bool loadSoundEffect(Mix_Chunk*& sfx, std::string path) {
    if (sfx == nullptr) {
        sfx = Mix_LoadWAV(path.c_str());
        if (sfx != nullptr) {
            sfxList.push_back(sfx);
        }
        else {
            std::cerr << "Error with loading sfx from: " << path << "\n";
        }
    }
    if (sfx == nullptr) {
        loadedAll = 0;
        return 0;
    }
    return 1;
}

bool loadMusic(Mix_Music*& song, std::string path) {
    if (song == nullptr) {
        song = Mix_LoadMUS(path.c_str());
        if (song != nullptr) {
            songList.push_back(song);
        }
        else {
            std::cerr << "Error with loading music from: " << path << "\n";
        }
    }
    if (song == nullptr) {
        loadedAll = 0;
        return 0;
    }
    return 1;
}

void freeChunksAndMusic() {
    for (auto sfx : sfxList) {
        Mix_FreeChunk(sfx);
    }
    for (auto song : songList) {
        Mix_FreeMusic(song);
    }
}

TextField namePlayer[3];

bool initTextures() {
    loadTexture(background_Texture, "./assets/textures/background.png");
    loadTexture(frontPart_Texture, "./assets/textures/front_part.png");
    loadTexture(scoreBoard_Texture, "./assets/textures/scoreboard.png");
    loadTexture(car1_Texture, "./assets/textures/car1_blue.png");
    loadTexture(car2_Texture, "./assets/textures/car2_red.png");
    loadTexture(ballTexture, "./assets/textures/rl_ball.png");
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


    loadSoundEffect(explosionSound, "./assets/audio/explosion.wav");
    loadSoundEffect(ballHitSound, "./assets/audio/ballhit.wav");
    loadSoundEffect(boostStartSound, "./assets/audio/booststart.wav");
    loadSoundEffect(boostEndSound, "./assets/audio/boostend.wav");
    loadSoundEffect(overtimeSound, "./assets/audio/overtime.wav");
    loadSoundEffect(victorySound, "./assets/audio/hooray.wav");
    loadSoundEffect(clickSound, "./assets/audio/click.wav");


    loadMusic(musicTitle, "./assets/audio/songtitle.wav");
    loadMusic(musicGameplay, "./assets/audio/songgameplay.wav");
    loadMusic(musicVictory, "./assets/audio/songvictory.wav");
    loadMusic(musicVictory0, "./assets/audio/songvictory0.wav");

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
    SDL_RenderCopyF(renderer.get(), texture, NULL, &rect);
}

bool initEverything() {
    toUpper(player1_name);
    toUpper(player2_name);

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "2");

    if (SDL_Init(SDL_INIT_EVERYTHING | SDL_INIT_AUDIO) != 0){
        std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
        return false;
    }

    window.reset(SDL_CreateWindow("Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE), SDL_DestroyWindow);
    if (window.get() == nullptr){
        std::cerr<< "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return false;
    }

    // SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    renderer.reset(SDL_CreateRenderer(window.get(), -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC), SDL_DestroyRenderer);
    if (renderer.get() == nullptr) {
        std::cerr << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return false;
    }

    if (TTF_Init() != 0) {
        std::cerr << "TTF_Init Error : " << SDL_GetError() << "\n";
        return false;
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        std::cerr << "SDL_mixer could not initialize! SDL_mixer Error: " <<  Mix_GetError() << "\n";
        return false;
    }

    if (initTextures() == false) {
        return false;
    }

    initButtons();
    screenSurface.reset(SDL_GetWindowSurface(window.get()), SDL_FreeSurface);

    return true;
}


void unloadTextures() {
    for (auto& texture : textures) {
        SDL_DestroyTexture(texture.second.get());
        texture.second = nullptr;
    }
    textures.clear();
}

void closeEverything() {
    closeFonts();
    unloadTextures();
    freeSurfaces();
    freeChunksAndMusic();

    if (renderer.get() != nullptr) {
        SDL_DestroyRenderer(renderer.get());
        renderer = nullptr;
    }

    // Free the screen surface
    if (screenSurface != nullptr) {
        SDL_FreeSurface(screenSurface.get());
        screenSurface = nullptr;
    }

    // Destroy the window
    if (window != nullptr) {
        SDL_DestroyWindow(window.get());
        window = nullptr;
    }

    TTF_Quit();
    Mix_Quit();
    IMG_Quit();
    SDL_Quit();
}
