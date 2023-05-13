#include "Stats.h"
#include "Ball.h"
#include "Car.h"
#include "CarBallCollisionHandler.h"
#include "Controls.h"
#include "PlayerScored.h"
#include "Explosion.h"
#include "Sounds.h"
#include "PlayerScored.h"
#include "Boost.h"

int prvScoreA = -1;
int prvScoreB = -1;
int scoreA = 0;
int scoreB = 0;
float player2_curSign, player1_curSign, player2_prvSign = -1, player1_prvSign = -1;

void updateStates(Car& car1, Car& car2, Ball& ball) {
    ball.moveBall();
    car1.moveCar();
    car2.moveCar();
}

void handleCollisions(Car& car1, Car& car2, Ball& ball) {
    handleCollisionCarBall(car1, ball);
    handleCollisionCarBall(car2, ball);
    car1.handleGroundCollision();
    car2.handleGroundCollision();
}

void Render(Car& car1, Car& car2, Ball& ball) {
    ball.draw();
    car1.draw();
    car2.draw();

    car1.prvSign = car1.curSign;
    car1.prvDir = car1.curDir;
    car2.prvSign = car2.curSign;
    car2.prvDir = car2.curDir;

    if (ball.yPos + ball.radius*2 <= botLeft1.y + 1e-1) { /// Front image
       SDL_RenderCopy(renderer.get(), frontPart_Texture.get(), &srcRect, &dstRect);
    }
}

void reset(bool overtime) {
    if (overtime) {
        prvScoreA = scoreA;
        prvScoreB = scoreB;
    }
    else {
        inOvertime = 0;
        prvScoreA = -1;
        prvScoreB = -1;
        scoreA = 0;
        scoreB = 0;

        timerOvertime.stop();
        for (int i = 0; i <= 7; ++i) {
            stopEffect(i);
        }
    }

    player2_prvSign = -1, player1_prvSign = -1;
    player2_prvLeftRight = "";
    player1_prvLeftRight = "";

    ball.resetBall();
    car1.resetCar();
    car2.resetCar();


    if (unlTimeMode == 0) {
        countDown = 1;
    }
    else {
        countDown = 0;
    }

    lastTime = SDL_GetTicks();
    timeElapsed = 0;
    timeLeft = 300;

    timer = 1;
    explosionParticles[0].clear();
    explosionParticles[1].clear();
    explosionParticles[2].clear();
    boostParticles[0].clear();
    boostParticles[1].clear();
    boostParticles[2].clear();
    startInMidAir = -1;

    displayingScored = 0;
    timerScored.stop();
    timerReminder.stop();
    timerExtraExplosion.stop();
}
