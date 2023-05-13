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
    createCars();
    createBall();

    while (isRunning) {
        switch (currentState) {
            case TITLE_SCREEN:
                titleScreen.handle();
                break;
            case GAMEPLAY:
                gameplay.handle();
                break;
            case VICTORY:
                victoryScreen.handle();
                break;
            case EXIT:
                isRunning = 0;
                break;
        }
    }

    closeEverything();
    return 0;
}
