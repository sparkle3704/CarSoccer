#include "Screens.h"
#include <SDL2/SDL.h>
#include "Car.h"
#include "Ball.h"
#include "Controls.h"
#include "Stats.h"
#include "PlayerScored.h"
#include "SDL_Contexts.h"
#include <vector>

bool isRunning = 1;

gameState currentState = TITLE_SCREEN;

int withinNameField(int mouseX, int mouseY) {
    if (namePlayer[1].withinNameField(mouseX, mouseY)) {
        return 1;
    }
    if (namePlayer[2].withinNameField(mouseX, mouseY)) {
        return 2;
    }
    return 0;
}

int nameFieldIndex = 0;


void handleTitleScreen() {
    bool isTyping = false;
    int mouseX, mouseY;
    bool onTitleScreen = 1;
    bool showingOptions = 0;
    while (onTitleScreen) {

        SDL_Event event;
        bool clicked = 0;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                onTitleScreen = 0;
                break;
            }
            SDL_GetMouseState(&mouseX, &mouseY);

            if (event.type == SDL_MOUSEBUTTONDOWN) {
                if (event.button.button == SDL_BUTTON_LEFT) {
                    clicked = 1;
                    if (showingOptions) {
                        isTyping = true;
                        SDL_StartTextInput();
                        nameFieldIndex = withinNameField(mouseX, mouseY);
                        std::cerr << nameFieldIndex << "\n";
                    }

                }
            } else if (event.type == SDL_TEXTINPUT && isTyping && showingOptions) {
                if (namePlayer[nameFieldIndex].width < namePlayer[nameFieldIndex].maxWidth/2) {
                    namePlayer[nameFieldIndex].content += event.text.text;
                }

            } else if (event.type == SDL_KEYDOWN && isTyping && showingOptions) {
                if (event.key.keysym.sym == SDLK_RETURN) {
                    isTyping = false;
                    SDL_StopTextInput();
                } else if (event.key.keysym.sym == SDLK_BACKSPACE && !namePlayer[nameFieldIndex].content.empty()) {
                    namePlayer[nameFieldIndex].content.pop_back();
                }
            }
        }

        SDL_SetRenderDrawColor(renderer, 221, 160, 221, 255);
        SDL_RenderClear(renderer);

        displayImage(titleBackgroundTexture);

//
//        displayImage(play_Selected, 960, 830 - 200);
//        displayImage(play_Selected, 960, 980 - 200);
//        displayImage(play_Selected, 960, 1130 - 200);
        if (showingOptions == 0) {
            for (auto& button: mainButtons) {
                if (button.withinArea(mouseX, mouseY)) {
                    button.displayImage(1);
                    if (clicked) {
                        if (button.name != "options") {
                            currentState = button.nextState;
                            onTitleScreen = 0;
                            break;
                        }
                        else {
                            showingOptions = 1;
                            std::cerr << "showing Option" << "\n";
                        }
                    }
                }
                else {
                    button.displayImage(0);
                }
            }
        }
        else {
            displayImage(optionsWindow, -2, -2);
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
                    button.displayImage(1);
                    if (clicked) {
                        if (button.name == "ok") {
                            showingOptions = 0;
                        }
                    }
                }
                else {
                    button.displayImage(0);
                }
            }
        }
        SDL_RenderPresent(renderer);
    }

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

//void midGameESC() {
//
//}

void handleGameplay() {
    bool onGameplay = true;
    while (onGameplay) {
        SDL_Event event;
        const Uint8* state = SDL_GetKeyboardState(NULL);

        /// player2
        if (enable_player2) {
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

            while (SDL_PollEvent(&event)) {
                switch (event.type) {
                    case SDL_QUIT:
                        onGameplay = false;
                        break;
                    case SDL_KEYDOWN:
                        if (event.key.keysym.sym == player2_sym_jump) { // jump
                            car2.jump();
                            car2.canJump = 0;
                            if (state[SDL_SCANCODE_DOWN]) {

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

                        break;
                    case SDL_KEYUP:
                        if (event.key.keysym.sym == player2_sym_jump) {
                            car2.canJump = 1;
                        }
                        if (event.key.keysym.sym == player1_sym_jump) {
                            car1.canJump = 1;
                        }
                }
            }

            if (player2_curLeftRight == "10" || player2_curLeftRight == "01") {
                player2_prvLeftRight = player2_curLeftRight;
            }

            if (state[player2_scancode_up]) {
                car2.tiltUp();
            }
            else if (state[player2_scancode_down]) {
                car2.tiltDown();
            }

            if (state[player2_scancode_boost]) {
                car2.curSign = 1.0;
                car2.boost(1.0);
            }
            else {
                car2.curSign = -1.0;
                car2.boost(-1.0);
            }
        }

        /// player1
        if (enable_player1) {
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

            while (SDL_PollEvent(&event)) {
                switch (event.type) {
                    case SDL_QUIT:
                        onGameplay = false;
                        break;
                    case SDL_KEYDOWN:
                        if (event.key.keysym.sym == player1_sym_jump) { // jump
                            std::cerr << "player1 hit jump" << "\n";
                            car1.jump();
                            car1.canJump = 0;
                            if (state[player1_scancode_down]) {
                                car1.dodgeUp();
                            }
                            if (state[player1_scancode_up]) {
                                car1.dodgeDown();
                            }
                        }
                        break;
                    case SDL_KEYUP:
                        if (event.key.keysym.sym == player1_sym_jump) {
                            car1.canJump = 1;
                        }
                }
            }

            if (player1_curLeftRight == "10" || player1_curLeftRight == "01") {
                player1_prvLeftRight = player1_curLeftRight;
            }

            if (state[player1_scancode_up]) {
                car1.tiltUp();
            }
            else if (state[player1_scancode_down]) {
                car1.tiltDown();
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
        ///

        displayBackground();

        updateStates(car1, car2, ball);
        handleCollisions(car1, car2, ball);
        Render(car1, car2, ball);

        displayScores();
        displayTime();
        displayInMidAir(ball);
        SDL_RenderPresent(renderer);
    }
}
