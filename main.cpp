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
#include "Screens.h"

int main(int argc, char* argv[]) {
    if (!initEverything()) {
        return 1;
    }

    mute = 1;
    createCars();
    createBall();
    std::cerr << "hello" << "\n";


    while (isRunning) {
        switch (currentState) {
            case TITLE_SCREEN:
                std::cerr << "titleScreen" << "\n";
                handleTitleScreen();
                break;
            case GAMEPLAY:
                std::cerr << "gameplay" << "\n";
                handleGameplay();
                break;
            case EXIT:
                isRunning = 0;
                break;
        }
    }
    SDL_FreeSurface(surfaceMessage);
//    SDL_DestroyTexture(Message);
    closeEverything();
    return 0;
}
