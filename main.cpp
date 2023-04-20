#include <iostream>
#include "SDL_Contexts.h"
#include "Constants.h"
#include "Structs.h"
#include "Ball.h"
#include "Car.h"
#include "CarBallCollisionHandler.h"
#include "Sounds.h"
#include "Stats.h"
#include "PlayerScored.h"
#include "Controls.h"

int main(int argc, char* argv[]) {
    if (!initEverything()) {
        return 1;
    }

    mute = 1;
    createCars();
    createBall();

    bool isRunning = true;

    while (isRunning) {
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
                        isRunning = false;
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
                        isRunning = false;
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

        SDL_RenderPresent(renderer);

    }
    SDL_FreeSurface(surfaceMessage);
//    SDL_DestroyTexture(Message);
    closeEverything();
    return 0;
}
