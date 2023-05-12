#include "Screens.h"
#include <SDL2/SDL.h>
#include "Car.h"
#include "Ball.h"
#include "Controls.h"
#include "Stats.h"
#include "PlayerScored.h"
#include "SDL_Contexts.h"
#include "Sounds.h"
#include <vector>
#include "Explosion.h"
#include "Sounds.h"

//std::shared_ptr<TTF_Font> Font(nullptr, TTF_CloseFont);
//std::shared_ptr<TTF_Font> fontOutline(nullptr, TTF_CloseFont);


bool isRunning = 1;
bool inOvertime = 0;
bool unlTimeMode = 0;
gameState currentState = TITLE_SCREEN;

TitleScreenInstance titleScreen;
GameplayInstance gameplay;
VictoryScreenInstance victoryScreen;
OptionsWindowInstance optionsWindow;
bool showingOptions = 0;
bool isTyping, clicked;
int mouseX, mouseY;
SDL_Event event;

int withinNameField(int mouseX, int mouseY) {
    if (namePlayer[1].withinNameField(mouseX, mouseY)) {
        return 1;
    }
    if (namePlayer[2].withinNameField(mouseX, mouseY)) {
        return 2;
    }
    return 0;
}


void TitleScreenInstance::init() {
    isTyping = 0;
    onTitleScreen = 1;
    showingOptions = 0;
    clicked = 0;
    Mix_HaltMusic();
    music = musicTitle;
}

void TitleScreenInstance::handleButtons() {
    for (auto& button: titleButtons) {
        if (button.withinArea(mouseX, mouseY)) {
            button.displayImage(1);
            if (clicked) {
                playEffectOnce(clickSound, clickChannel);
                if (button.name != "options") {
                    currentState = button.nextState;
                    onTitleScreen = 0;
                    break;
                }
                else {
                    showingOptions = 1;
                }
            }
        }
        else {
            button.displayImage(0);
        }
    }
}

void TitleScreenInstance::handle() {
    init();
    while (onTitleScreen) {
        playMusic();
        clicked = 0;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                onTitleScreen = 0;
                break;
            }
            SDL_GetMouseState(&mouseX, &mouseY);

            if (showingOptions) {
                optionsWindow.handleEvent();
            }
            if (event.type == SDL_MOUSEBUTTONDOWN) {
                if (event.button.button == SDL_BUTTON_LEFT) {
                    clicked = 1;
                }
            }
        }

        SDL_SetRenderDrawColor(renderer.get(), 221, 160, 221, 255);
        SDL_RenderClear(renderer.get());

        displayImage(titleBackground_Texture.get());

        if (showingOptions == 0) {
            handleButtons();
        }
        else {
            optionsWindow.handleButtons();
        }

        SDL_RenderPresent(renderer.get());
    }
}

void OptionsWindowInstance::handleEvent() {
    SDL_GetMouseState(&mouseX, &mouseY);
    if (event.type == SDL_MOUSEBUTTONDOWN) {
        if (event.button.button == SDL_BUTTON_LEFT) {
            clicked = 1;
            isTyping = 1;
            SDL_StartTextInput();
            nameFieldIndex = withinNameField(mouseX, mouseY);
        }
    } else if (event.type == SDL_TEXTINPUT && isTyping) {
        if (nameFieldIndex != 0 && namePlayer[nameFieldIndex].width < namePlayer[nameFieldIndex].maxWidth/2) {
            namePlayer[nameFieldIndex].content += event.text.text;
        }

    } else if (event.type == SDL_KEYDOWN && isTyping) {
        if (event.key.keysym.sym == SDLK_RETURN) {
            isTyping = 0;
            SDL_StopTextInput();
        } else if (nameFieldIndex != 0 && event.key.keysym.sym == SDLK_BACKSPACE && !namePlayer[nameFieldIndex].content.empty()) {
            namePlayer[nameFieldIndex].content.pop_back();
        }
    }
}

void OptionsWindowInstance::setNames() {
    player1_name = namePlayer[1].content;
    player2_name = namePlayer[2].content;

    if (player1_name.empty()) {
        player1_name = "Player 1";
    }
    if (player2_name.empty()) {
        player2_name = "Player 2";
    }
    toUpper(player1_name);
    toUpper(player2_name);
}


void OptionsWindowInstance::handleButtons() {
    displayImage(optionsWindow_Texture.get(), -2, -2);
    if (nameFieldIndex == 1) {
        namePlayer[1].content += "|";
    }
    else if (nameFieldIndex == 2){
        namePlayer[2].content += "|";
    }

    namePlayer[1].displayText();
    namePlayer[2].displayText();

    if (nameFieldIndex == 1) {
        namePlayer[1].content.pop_back();
    }
    else if (nameFieldIndex == 2){
        namePlayer[2].content.pop_back();
    }
    for (auto& button: optionsButtons) {
        if (button.withinArea(mouseX, mouseY)) {
            if (button.toggle == 1) {
                button.displayImage(button.state);
                if (clicked) {
                    playEffectOnce(clickSound, clickChannel);
                    button.state = !button.state;
                    if (button.name == "unltime") {
                        unlTimeMode = button.state;
                        countDown = !unlTimeMode;
                        if (countDown) {
                            timeLeft = 300;
                            timeElapsed = 0;
                        }

                    }
                    else if (button.name == "sfx") {
                        muteSFX = !button.state;
                    }
                    else if (button.name == "music") {
                        ///
                        muteMusic = !button.state;
                    }
                    else if (button.name == "noground") {
                        ///
                        noGroundMode = button.state;
                    }
                }
            }
            else {
                button.displayImage(1);
                if (clicked) {
                    if (button.name == "ok") {
                        playEffectOnce(clickSound, clickChannel);
                        showingOptions = 0;
                    }
                }
            }
        }
        else {
            if (button.toggle == 0) {
                button.displayImage(0);
            }
            else {
                button.displayImage(button.state);
            }
        }
    }
    setNames();
}

void GameplayInstance::init() {
    isTyping = 0;
    onGameplay = 1;
    showingOptions = 0;
    clicked = 0;
    Mix_HaltMusic();
    music = musicGameplay;
}

void GameplayInstance::handlePausedButtons() {
    SDL_GetMouseState(&mouseX, &mouseY);
//    std::cerr << mouseX << " " << mouseY << "\n";
    for (auto& button: pausedButtons) {
        if (button.withinArea(mouseX, mouseY)) {
            button.displayImage(1);
            if (clicked) {
                playEffectOnce(clickSound, clickChannel);
                if (button.name == "options") {
                    showingOptions = 1;
                    showingPausedMenu = 0;
                    break;
                }
                else if (button.name == "menu") {
                    currentState = TITLE_SCREEN;
                    showingOptions = 0;
                    showingPausedMenu = 0;
                    onGameplay = 0;
                    break;
                }
                else if (button.name == "resume") {
                    showingOptions = 0;
                    showingPausedMenu = 0;
                }
            }
        }
        else {
            button.displayImage(0);
        }
    }
}

void GameplayInstance::handle() {
    init();
    reset(0);
    while (onGameplay) {
        playMusic();
        const Uint8* state = SDL_GetKeyboardState(NULL);
        clicked = 0;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                onGameplay = 0;
                break;
            }
            else {
                if ((event.type == SDL_KEYDOWN) && (event.key.keysym.sym == player1_sym_pause || event.key.keysym.sym == player2_sym_pause)) {
                    if (showingPausedMenu == 0) {
                        if (showingOptions == 1) {
                            showingOptions = 0;
                            showingPausedMenu = 1;
                        }
                        else {
                            showingPausedMenu = 1;
                        }
                    }
                    else {
                        showingPausedMenu = 0;
                        showingOptions = 0;
                    }
                }
                if (showingPausedMenu == 1) {
                    handlePausedButtons();
                }
                else if (showingOptions == 1) {
                    optionsWindow.handleEvent();
                }
                else {
                    if (event.type == SDL_KEYDOWN) {
                        if (event.key.keysym.sym == player2_sym_jump) { // jump
                            car2.jump();
                            car2.canJump = 0;
                            if  (state[SDL_SCANCODE_DOWN]) {

                                car2.dodgeUp();
                            }
                            if (state[SDL_SCANCODE_UP]) {
                                car2.dodgeDown();
                            }
                        }

                        if (event.key.keysym.sym == player1_sym_jump) { // jump
                            car1.jump();
                            car1.canJump = 0;
                            if (state[player1_scancode_down]) {
                                car1.dodgeUp();
                            }
                            if (state[player1_scancode_up]) {
                                car1.dodgeDown();
                            }
                        }
                    }
                    else if (event.type == SDL_KEYUP) {
                        if (event.key.keysym.sym == player2_sym_jump) {
                            car2.canJump = 1;
                        }
                        if (event.key.keysym.sym == player1_sym_jump) {
                            car1.canJump = 1;
                        }
                    }
                }
                if (event.type == SDL_MOUSEBUTTONDOWN) {
                    if (event.button.button == SDL_BUTTON_LEFT) {
                        clicked = 1;
                    }
                }
            }
        }

        /// player2
        if (enable_player2 && (showingOptions == 0 && showingPausedMenu == 0)) {
            car2.tilting = 0;
            if (state[player2_scancode_up]) {
                car2.tiltUp();
                car2.tilting = 1;
            }
            else if (state[player2_scancode_down]) {
                car2.tiltDown();
                car2.tilting = 1;
            }

            std::string player2_stateLeft = (state[player2_scancode_left] ? "1" : "0");
            std::string player2_stateRight = (state[player2_scancode_right] ? "1" : "0");
            std::string player2_curLeftRight = player2_stateLeft + player2_stateRight;
            if (state[player2_scancode_left] && state[player2_scancode_right]) {
                if (player2_prvLeftRight == "10") {
                    car2.moveRight();
                    car2.setDir(1);
                }
                else {
                    car2.moveLeft();
                    car2.setDir(0);
                }
            }
            else {
                if (state[player2_scancode_left]) {
                    car2.moveLeft();
                    car2.setDir(0);
                }
                else if (state[player2_scancode_right]) {
                    car2.moveRight();
                    car2.setDir(1);
                }
                else {
                    car2.goingVelocityX = 0;
                }
            }

            if (state[player2_scancode_boost]) {
                car2.curSign = 1.0;
                car2.boost(1.0);
            }
            else {
                car2.curSign = -1.0;
                car2.boost(-1.0);
            }

            if (player2_curLeftRight == "10" || player2_curLeftRight == "01") {
                player2_prvLeftRight = player2_curLeftRight;
            }
        }

        /// player1
        if (enable_player1 && (showingOptions == 0 && showingPausedMenu == 0)) {
            car1.tilting = 0;
            if (state[player1_scancode_up]) {
                car1.tiltUp();
                car1.tilting = 1;
            }
            else if (state[player1_scancode_down]) {
                car1.tiltDown();
                car1.tilting = 1;
            }

            std::string player1_stateLeft = (state[player1_scancode_left] ? "1" : "0");
            std::string player1_stateRight = (state[player1_scancode_right] ? "1" : "0");
            std::string player1_curLeftRight = player1_stateLeft + player1_stateRight;
            if (state[player1_scancode_left] && state[player1_scancode_right]) {
                if (player1_prvLeftRight == "10") {
                    car1.moveRight();
                    car1.setDir(1);
                }
                else {
                    car1.moveLeft();
                    car1.setDir(0);
                }
            }
            else {
                if (state[player1_scancode_left]) {
                    car1.moveLeft();
                    car1.setDir(0);
                }
                else if (state[player1_scancode_right]) {
                    car1.moveRight();
                    car1.setDir(1);
                }
                else {
                    car1.goingVelocityX = 0;
                }
            }

            if (player1_curLeftRight == "10" || player1_curLeftRight == "01") {
                player1_prvLeftRight = player1_curLeftRight;
            }

            if (state[player1_scancode_boost]) {
                car1.curSign = 1.0;
                car1.boost(1.0);
            }
            else {
                car1.curSign = -1.0;
                car1.boost(-1.0);
            }

        }
        displayBackground();

        updateStates(car1, car2, ball);
        handleCollisions(car1, car2, ball);
        Render(car1, car2, ball);

        displayScores();
        displayTime();
        displayInMidAir(ball);

        if (showingPausedMenu) {
            handlePausedButtons();
        }
        else if (showingOptions) {
            optionsWindow.handleButtons();
            if (showingOptions == 0) {
                showingPausedMenu = 1;
            }
        }
        SDL_RenderPresent(renderer.get());
    }
}

/// no
//void VictoryScreenInstance::init() {
//    onVictoryScreen = 1;
//    music = musicVictory;
//    Font = TTF_OpenFont(fontPath.c_str(), 100); ///
//    if (scoreA > scoreB) {
//        s = player1_name + " " + "WON!";
//        winnerSurface = TTF_RenderText_Blended(Font, s.c_str(), BLUE); ///
//    }
//    else {
//        s = player2_name + " " + "WON!";
//        winnerSurface = TTF_RenderText_Blended(Font, s.c_str(), ORANGE); ///
//    }
//
//    winnerTexture = SDL_CreateTextureFromSurface(renderer.get(), winnerSurface); ///
//
//    width = winnerSurface->w; ///
//    height = winnerSurface->h; ///
//    rect = {960 - width/2, 286 - height/2, width, height};
//
//    Font = nullptr; ///
//    TTF_CloseFont(Font); ///
//    winnerSurface = nullptr;
//}

void VictoryScreenInstance::init() {
    onVictoryScreen = 1;
    if (music != musicVictory) {
        music = musicVictory0;
        Mix_HaltMusic();
    }
//    music = musicVictory;
//    auto FontDeleter = [](TTF_Font* font) { TTF_CloseFont(font); };
//    std::shared_ptr<TTF_Font> Font(TTF_OpenFont(fontPath.c_str(), 100), FontDeleter);

    Font.reset(TTF_OpenFont(fontPath.c_str(), 100), TTF_CloseFont);


    if (scoreA > scoreB) {
        s = player1_name + " " + "WON!";
        winnerSurface.reset(TTF_RenderText_Blended(Font.get(), s.c_str(), BLUE), SDL_FreeSurface);
    }
    else {
        s = player2_name + " " + "WON!";
        winnerSurface.reset(TTF_RenderText_Blended(Font.get(), s.c_str(), ORANGE), SDL_FreeSurface);
    }

    winnerTexture.reset(SDL_CreateTextureFromSurface(renderer.get(), winnerSurface.get()), SDL_DestroyTexture);

    width = winnerSurface->w;
    height = winnerSurface->h;
    rect = {960 - width/2, 286 - height/2, width, height};
}

void VictoryScreenInstance::handleButtons() { /// replay, options, menu
    for (auto& button: victoryButtons) {
        if (button.withinArea(mouseX, mouseY)) {
            button.displayImage(1);
            if (clicked) {
                playEffectOnce(clickSound, clickChannel);
                if (button.name != "options") {
                    currentState = button.nextState;
                    onVictoryScreen = 0;
                    break;
                }
                else {
                    showingOptions = 1;
                }
            }
        }
        else {
            button.displayImage(0);
        }
    }
}

void VictoryScreenInstance::handle() {
    init();
    playEffectOnce(victorySound, victoryChannel);
    addExplosion(100, 960, 286, 1);
    addExplosion(100, 960, 286, 2);

    while (onVictoryScreen) {
        playMusic();
        clicked = 0;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                onVictoryScreen = 0;
                break;
            }
            SDL_GetMouseState(&mouseX, &mouseY);

            if (showingOptions) {
                optionsWindow.handleEvent();
            }
            if (event.type == SDL_MOUSEBUTTONDOWN) {
                if (event.button.button == SDL_BUTTON_LEFT) {
                    clicked = 1;
                }
            }
        }

        SDL_SetRenderDrawColor(renderer.get(), 221, 160, 221, 255);
        SDL_RenderClear(renderer.get());


        if (scoreA > scoreB) {
            displayImage(victoryP1_Texture.get());
        }
        else {
            displayImage(victoryP2_Texture.get());
        }

        SDL_RenderCopyF(renderer.get(), winnerTexture.get(), NULL, &rect);
        updateExplosion(1);
        renderExplosion(1);
        updateExplosion(2);
        renderExplosion(2);

        if (showingOptions == 0) {
            handleButtons();
        }
        else {
            optionsWindow.handleButtons();
        }

        SDL_RenderPresent(renderer.get());
    }
}
