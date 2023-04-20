#include "Stats.h"
#include "Ball.h"
#include "Car.h"
#include "CarBallCollisionHandler.h"

int prvScoreA = -1;
int prvScoreB = -1;
int scoreA = 0;
int scoreB = 0;
float player2_curSign, player1_curSign, player2_prvSign = -1, player1_prvSign = -1;
bool prvDir = 0;

//extern Ball ball;
//extern Car car1, car2;

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
    car1.prvDir = car1.dir;
    car2.prvSign = car2.curSign;
    car2.prvDir = car2.dir;

    if (ball.yPos + ball.radius*2 <= botLeft1.y + 1e-1) { /// Front image
       SDL_RenderCopy(renderer, frontPart, &srcRect, &dstRect);
    }
}
