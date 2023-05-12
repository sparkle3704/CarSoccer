#ifndef CAR_H_INCLUDED
#define CAR_H_INCLUDED
#pragma once
#include <deque>
#include <SDL2/SDL.h>
#include "Structs.h"
#include "Constants.h"
#include "SDL_Contexts.h"

class Car {
public:
    Car(float xPos, float yPos, int player);
    float goingVelocityX = 0;
    bool canJump = 1;
    float prvSign = -1, curSign;
    bool prvDir = 0, curDir;

    float xPos;
    float yPos;

    void moveRight();
    void setDir(bool newDir);
    void moveLeft();
    void jump();
    void dodgeUp();
    void dodgeDown();
    void tiltUp();
    void tiltDown();
    void boost(float sign);

    float width = 100;
    float height;
    float angle;
    float velocityX = 0;
    bool tilting = 0;
    float velocityY = 0;
    float mass = 200;

    void moveCar();
    void draw();
    void handleGroundCollision();
    void resetCar();

private:
    float accelerationX = 0;
    float accelerationY = 0;
    bool onGround = 1;
    bool jumping = 0;
    SDL_RendererFlip flip;
    bool clockWise;
    bool pointing;
    float gravityVelocityY = 0;
    float jumpVelocityX = 0;
    float jumpVelocityY = 0;
    float initialJumpDragAccelerationX = 0;
    float initialJumpDragAccelerationY = 0;
    float initialGravityAccelerationY = 0;
    float boostVelocityX = 0;
    float boostVelocityY = 0;
    float boostVelocity = 0;
    float initialBoostAccelerationX = 0;
    float initialBoostAccelerationY = 0;
    bool spinningClockWise = 0;
    float spinnedClockWise = 0;
    bool spinningCounterClockWise = 0;
    float spinnedCounterClockWise = 0;
    float dodgeVelocityX = 0;
    float dodgeVelocityY = 0;
    bool dodgingDown = 0;
    bool dodgingUp = 0;
    bool collideWithGround = 0;

    Point parallelBeforeDodgeA;
    Point parallelBeforeDodgeB;

    float carBallCollisionVelocityX = 0;
    float carBallCollisionVelocityY = 0;
    float carBallCollisionDragAccelerationX = 0;
    float carBallCollisionDragAccelerationY = 0;
//    SDL_Texture* carTexture = nullptr;
    std::shared_ptr<SDL_Texture> carTexture;
    int player;

    float dodgeDragAccelerationX = 0;
    float dodgeDragAccelerationY = 0;
    std::deque<SDL_FPoint> headBuffer;

    bool hasCollision();

    bool hasCollisionWithGround();

    void correctAngle();

    float binS(float sign);

    void verticalFlip();

    void correctPosition();

    void applyGravity();
};

extern void createCars();

extern Car car1;
extern Car car2;

#endif // CAR_H_INCLUDED
