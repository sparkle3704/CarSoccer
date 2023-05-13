#include "SDL_Contexts.h"
#include "Car.h"
#include "Constants.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <vector>
#include "CommonFunctions.h"
#include <algorithm>
#include "Sounds.h"
#include <deque>
#include "Boost.h"
#include <iostream>

Car car1, car2;
Car::Car(float xPos, float yPos, int player):
    xPos(xPos), yPos(yPos), player(player) {

    if (player == 0) {
        return;
    }

    initTextures();

    if (player == 1) {
        carTexture = car1_Texture;
    }
    else {
        carTexture = car2_Texture;
    }

    int w, h;
    SDL_QueryTexture(carTexture.get(), NULL, NULL, &w, &h);
    float scale = width / float(w);
    height = float(h) * scale;

    if (player == 1) {
        angle = 180;
        pointing = 1;
        clockWise = 1;
        curDir = 1;
    }
    else {
        angle = 0;
        pointing = 0;
        clockWise = 0;
        curDir = 0;
    }
    xPos -= width*0.5;
}

void Car::resetCar() {
    if (player == 1) {
        angle = 180;
        pointing = 1;
        clockWise = 1;
        curDir = prvDir = 1;
        xPos = WINDOW_WIDTH/2 - 300 - width*0.5;
        yPos = groundY - 32;
    }
    else {
        angle = 0;
        pointing = 0;
        clockWise = 0;
        curDir = prvDir = 0;
        xPos = WINDOW_WIDTH/2 + 300 - width*0.5;
        yPos = groundY - 32;
    }
    curSign = prvSign = -1;
    goingVelocityX = 0;

    velocityX = 0;
    velocityY = 0;

    accelerationX = 0;
    accelerationY = 0;
    onGround = 1;
    jumping = 0;

    gravityVelocityY = 0;
    jumpVelocityX = 0;
    jumpVelocityY = 0;
    initialJumpDragAccelerationX = 0;
    initialJumpDragAccelerationY = 0;
    initialGravityAccelerationY = 0;
    boostVelocityX = 0;
    boostVelocityY = 0;
    boostVelocity = 0;
    initialBoostAccelerationX = 0;
    initialBoostAccelerationY = 0;
    spinningClockWise = 0;
    spinnedClockWise = 0;
    spinningCounterClockWise = 0;
    spinnedCounterClockWise = 0;
    dodgeVelocityX = 0;
    dodgeVelocityY = 0;
    dodgingDown = 0;
    dodgingUp = 0;

    carBallCollisionVelocityX = 0;
    carBallCollisionVelocityY = 0;
    carBallCollisionDragAccelerationX = 0;
    carBallCollisionDragAccelerationY = 0;

    dodgeDragAccelerationX = 0;
    dodgeDragAccelerationY = 0;
}

void createCars() {
    initTextures();
    car1 = Car(WINDOW_WIDTH/2 - 300, groundY - 32, 1);
    car2 = Car(WINDOW_WIDTH/2 + 300, groundY - 32, 2);
}

bool Car::hasCollision() {
    SDL_FRect carRect = {xPos, yPos, width, height};
    std::vector<Point> tmp = getCoords(carRect, angle);
    float x1 = tmp[0].x, y1 = tmp[0].y;
    float x2 = tmp[1].x, y2 = tmp[1].y;
    float x3 = tmp[2].x, y3 = tmp[2].y;
    float x4 = tmp[3].x, y4 = tmp[3].y;

    float minX = std::min({x1, x2, x3, x4});
    float minY = std::min({y1, y2, y3, y4});
    float maxX = std::max({x1, x2, x3, x4});
    float maxY = std::max({y1, y2, y3, y4});

    if (maxX >= WINDOW_WIDTH) {
//            std::cerr << "Hit right wall!" << "maxX = " << maxX << ", " << "WINDOW_WIDTH = " << WINDOW_WIDTH << "\n";
        return 1;
    }
    if (minX <= 0) {
//            std::cerr << "Hit left wall!" << "\n";
        return 1;
    }
    if (minY <= 0) {
//            std::cerr << "Hit ceiling!" << "\n";
        return 1;
    }
    if (maxY >= groundY) {
//            std::cerr << "Hit ground" << "\n";
        return 1;
    }
    return 0;
}

bool Car::hasCollisionWithGround() {
    SDL_FRect carRect = {xPos, yPos, width, height};
    std::vector<Point> tmp = getCoords(carRect, angle);
    float x1 = tmp[0].x, y1 = tmp[0].y;
    float x2 = tmp[1].x, y2 = tmp[1].y;
    float x3 = tmp[2].x, y3 = tmp[2].y;
    float x4 = tmp[3].x, y4 = tmp[3].y;

    float minX = std::min({x1, x2, x3, x4});
    float minY = std::min({y1, y2, y3, y4});
    float maxX = std::max({x1, x2, x3, x4});
    float maxY = std::max({y1, y2, y3, y4});

    if (maxY >= groundY) {
        return 1;
    }
    return 0;
}
void Car::correctAngle() {

    if (!(angle >= 0 && angle <= 359)) {
        if (angle >= 0) {
            angle = fmod(angle, 360);
        }
        else {
            angle = 360 - fmod(-angle, 360);
        }
        angle = fmod(angle, 360);
    }

    if ((angle >= 0 && angle <= 90) || (angle >= 270 && angle <= 359)) {
        pointing = 0;
    }
    else {
        pointing = 1;
    }
    if (angle == 90) {
        if (curDir == 0) {
            pointing = 0;
        }
        else {
            pointing = 1;
        }
    }
    if (angle == 270) {
        if (curDir == 0) {
            pointing = 0;
        }
        else {
            pointing = 1;
        }
    }
    if (pointing == 1) {
        if (curDir == 1) {
            flip = SDL_FLIP_VERTICAL;
            clockWise = 1;
        }
        else {
            flip = SDL_FLIP_NONE;
            clockWise = 0;
        }
    }
    else {
        if (curDir == 0) {
            flip = SDL_FLIP_NONE;
            clockWise = 0;
        }
        else {
            flip = SDL_FLIP_VERTICAL;
            clockWise = 1;
        }
    }
}

float Car::binS(float sign) { // find biggest so that angle +- ans errorfree
    float _angle = angle;
    float l = 0, r = ROTATE_FACTOR, ans = l, m;
    for (int iter = 1; iter <= 5; ++iter) {
        m = l + (r - l)*0.5;
        angle += sign*m;
        if (hasCollisionWithGround() == 0) {
            ans = m;
            l = m;
        }
        else {
            r = m;
        }
    }
    angle = _angle;
    return ans;
}
void Car::tiltDown() {
    if (spinningClockWise == 0 && spinningCounterClockWise == 0 && collideWithGround == 0) {
        if (clockWise == 0) {
            angle += binS(1.0);
//                angle += ROTATE_FACTOR;
        }
        else {
            angle -= binS(-1.0);
//                angle -= ROTATE_FACTOR;
        }
    }
    else {

    }
    correctAngle();
    correctPosition();

    if (angle >= 360) {
        angle = angle - 360;
    }
    if (angle < 0) {
        angle = 360 + angle;
    }
    if (angle == 360) {
        angle = 0;
    }
}

void Car::tiltUp() {
    if (spinningClockWise == 0 && spinningCounterClockWise == 0 && collideWithGround == 0) {
        if (clockWise == 0) {
            angle -= binS(-1.0);
//                angle -= ROTATE_FACTOR;
        }
        else {
            angle += binS(1.0);
//                angle += ROTATE_FACTOR;
        }
    }
    else {
//            std::cerr << "Won't spin" << "\n";
    }
    correctAngle();
    correctPosition();

    if (angle >= 360) {
        angle = angle - 360;
    }
    if (angle < 0) {
        angle = 360 + angle;
    }
    if (angle == 360) {
        angle = 0;
    }
}

void Car::verticalFlip() {
    if (angle <= 180) {
        angle = 90 - angle + 90;
    }
    else {
        angle = 270 - angle + 270;
    }
//    curDir = !curDir;
//    pointing = !pointing;
//    if (flip == SDL_FLIP_NONE) {
//        flip = SDL_FLIP_VERTICAL;
//    }
//    else {
//        flip = SDL_FLIP_NONE;
//    }
}

void Car::moveRight() {
    clockWise = 1;
    goingVelocityX = VELOCITY_X_FACTOR;
}

void Car::moveLeft() {
    clockWise = 0;
    goingVelocityX = -VELOCITY_X_FACTOR;
}

void Car::correctPosition() {
    SDL_FRect carRect = {xPos, yPos, width, height};
    std::vector<Point> tmp = getCoords(carRect, angle);
    float x1 = tmp[0].x, y1 = tmp[0].y;
    float x2 = tmp[1].x, y2 = tmp[1].y;
    float x3 = tmp[2].x, y3 = tmp[2].y;
    float x4 = tmp[3].x, y4 = tmp[3].y;

    float minX = std::min({x1, x2, x3, x4});
    float minY = std::min({y1, y2, y3, y4});
    float maxX = std::max({x1, x2, x3, x4});
    float maxY = std::max({y1, y2, y3, y4});

    if (maxY > groundY) {
        float delta = maxY - groundY;
        y1 -= delta;
        y2 -= delta;
        y3 -= delta;
        y4 -= delta;
        yPos -= delta;
        minX = std::min({x1, x2, x3, x4});
        minY = std::min({y1, y2, y3, y4});
        maxX = std::max({x1, x2, x3, x4});
        maxY = std::max({y1, y2, y3, y4});
    }

    if (minY < 0) {
        float delta = -minY;
        y1 += delta;
        y2 += delta;
        y3 += delta;
        y4 += delta;
        yPos += delta;
        minX = std::min({x1, x2, x3, x4});
        minY = std::min({y1, y2, y3, y4});
        maxX = std::max({x1, x2, x3, x4});
        maxY = std::max({y1, y2, y3, y4});
    }

    if (maxX > WINDOW_WIDTH) {
        float delta = maxX - WINDOW_WIDTH;
        x1 -= delta;
        x2 -= delta;
        x3 -= delta;
        x4 -= delta;
        xPos -= delta;
        minX = std::min({x1, x2, x3, x4});
        minY = std::min({y1, y2, y3, y4});
        maxX = std::max({x1, x2, x3, x4});
        maxY = std::max({y1, y2, y3, y4});
    }

    if (minX < 0) {
        float delta = -minX;
        x1 += delta;
        x2 += delta;
        x3 += delta;
        x4 += delta;
        xPos += delta;
        minX = std::min({x1, x2, x3, x4});
        minY = std::min({y1, y2, y3, y4});
        maxX = std::max({x1, x2, x3, x4});
        maxY = std::max({y1, y2, y3, y4});
    }
}

void Car::applyGravity() {
    if (onGround == 0) {
        initialGravityAccelerationY = GRAVITY_ACCELERATION;
    }
}


void Car::boost(float sign) {
    SDL_FRect carRect = {xPos, yPos, width, height};
    std::vector<Point> tmp = getCoords(carRect, angle);
    float x1 = tmp[0].x, y1 = tmp[0].y;
    float x2 = tmp[1].x, y2 = tmp[1].y;
    float x3 = tmp[2].x, y3 = tmp[2].y;
    float x4 = tmp[3].x, y4 = tmp[3].y;

    float minX = std::min({x1, x2, x3, x4});
    float minY = std::min({y1, y2, y3, y4});
    float maxX = std::max({x1, x2, x3, x4});
    float maxY = std::max({y1, y2, y3, y4});

    if (sign == 1) {
        if (prvSign == -1) {
            playEffectOnce(boostStartSound, boostStartChannel[player]);
            playEffect(boostEndSound, boostEndChannel[player]);
            if (velocityY == 0) {
                boostVelocity = abs(velocityX);
            }
            else {

            }
            gravityVelocityY = GRAVITY_ACCELERATION/5;
        }
        else {
            goingVelocityX = 0;
            gravityVelocityY = GRAVITY_ACCELERATION/5;
        }
    }
    else {

        if (prvSign == 1) {
            stopEffect(boostEndChannel[player]);
            if (velocityY == 0) {
                if (goingVelocityX != 0) {
                    boostVelocity -= sign*BOOST_ACCELERATION * deltaTime;
                }
            }
        }
    }
    if (curDir != prvDir) {
        boostVelocity /= 5;
    }

    if (spinningClockWise || spinningCounterClockWise) {

    }
    else {
        boostVelocity += sign*BOOST_ACCELERATION * deltaTime;
    }

    boostVelocity = std::min(boostVelocity, MAX_BOOST_VELOCITY);
    boostVelocity = std::max(boostVelocity, 0.0f);

    Point projected = findParallelVector(tmp[1], tmp[0], boostVelocity);

    if (spinningClockWise || spinningCounterClockWise) {
        projected = findParallelVector(parallelBeforeDodgeA, parallelBeforeDodgeB, boostVelocity);
    }

    boostVelocityX = projected.x;
    boostVelocityY = projected.y;
}

void Car::moveCar() {
    applyGravity();
    SDL_FRect carRect = {xPos, yPos, width, height};
    std::vector<Point> tmp = getCoords(carRect, angle);
    float x1 = tmp[0].x, y1 = tmp[0].y;
    float x2 = tmp[1].x, y2 = tmp[1].y;
    float x3 = tmp[2].x, y3 = tmp[2].y;
    float x4 = tmp[3].x, y4 = tmp[3].y;

    float minX = std::min({x1, x2, x3, x4});
    float minY = std::min({y1, y2, y3, y4});
    float maxX = std::max({x1, x2, x3, x4});
    float maxY = std::max({y1, y2, y3, y4});

    if (maxY >= groundY) {
        float delta = maxY - groundY;
        y1 -= delta;
        y2 -= delta;
        y3 -= delta;
        y4 -= delta;
        yPos -= delta;
        maxY = std::max({y1, y2, y3, y4});
    }
    if (maxY >= groundY && jumping == 0) {
        velocityY = 0;
        gravityVelocityY = 0;
        initialGravityAccelerationY = 0;
        onGround = 1;
    }
    else {
        if (jumping) {
            if (velocityY >= 0) {
                jumping = 0;
            }
        }
    }

    if (curSign == 1) {
        if (prvSign == 1) {

        }
    }
    if (spinningClockWise) {
        if (spinnedClockWise >= 360) { // 360
            spinningClockWise = 0;
            spinnedClockWise = 0;
            dodgingDown = 0;
            dodgingUp = 0;
            initialBoostAccelerationY = GRAVITY_ACCELERATION;
        }
        else {
            spinnedClockWise += DODGE_SPIN_FACTOR;
            angle += DODGE_SPIN_FACTOR;
            jumpVelocityX = 0;
            jumpVelocityY = 0;

            gravityVelocityY = 0;
            initialGravityAccelerationY = 0;
        }
    }
    else if (spinningCounterClockWise) {
        if (spinnedCounterClockWise >= 360) {
            spinningCounterClockWise = 0;
            spinnedCounterClockWise = 0;
            dodgingDown = 0;
            dodgingUp = 0;
            initialBoostAccelerationY = GRAVITY_ACCELERATION;
        }
        else {
            spinnedCounterClockWise += DODGE_SPIN_FACTOR;
            angle -= DODGE_SPIN_FACTOR;
            jumpVelocityX = 0;
            jumpVelocityY = 0;

            gravityVelocityY = 0;
            initialGravityAccelerationY = 0;
        }
    }
    if (dodgingUp) {
        boostVelocityX = 0;
        boostVelocityY = 0;
    }

    correctPosition();

    /// ///////////////////////////////////////////////////////
    velocityX = goingVelocityX + jumpVelocityX + boostVelocityX + dodgeVelocityX + carBallCollisionVelocityX;
    velocityY =  gravityVelocityY + jumpVelocityY + boostVelocityY + dodgeVelocityY + carBallCollisionVelocityY;

    accelerationX = initialJumpDragAccelerationX + carBallCollisionDragAccelerationX + dodgeDragAccelerationX;
    accelerationY = initialGravityAccelerationY + initialJumpDragAccelerationY + carBallCollisionDragAccelerationY + dodgeDragAccelerationY;

    xPos += (velocityX + accelerationX * deltaTime);
    yPos += (velocityY + accelerationY * deltaTime);

    if (abs(dodgeDragAccelerationX)*deltaTime > abs(dodgeVelocityX)) {
        dodgeVelocityX = 0;
        dodgeDragAccelerationX = 0;
    }
    else {
        dodgeVelocityX += dodgeDragAccelerationX * deltaTime;
    }

    if (abs(dodgeDragAccelerationY)*deltaTime > abs(dodgeVelocityY)) {
        dodgeVelocityY = 0;
        dodgeDragAccelerationY = 0;
    }
    else {
        dodgeVelocityY += dodgeDragAccelerationY * deltaTime;
    }
    if (curDir != prvDir) {
        dodgeVelocityX = 0;
        dodgeDragAccelerationX = 0;
    }

    if (abs(carBallCollisionDragAccelerationX)*deltaTime > abs(carBallCollisionVelocityX)) {
        carBallCollisionDragAccelerationX = 0;
        carBallCollisionVelocityX = 0;
    }
    else {
        carBallCollisionVelocityX += carBallCollisionDragAccelerationX * deltaTime;
    }

    if (abs(carBallCollisionDragAccelerationY)*deltaTime > abs(carBallCollisionVelocityY)) {
        carBallCollisionDragAccelerationY = 0;
        carBallCollisionVelocityY = 0;
    }
    else {
        carBallCollisionVelocityY += carBallCollisionDragAccelerationY * deltaTime;
    }

    /// ///////////////////////////////////////////////////////


    if (abs(initialJumpDragAccelerationX) * deltaTime > abs(jumpVelocityX)) {
        jumpVelocityX = 0;
        initialJumpDragAccelerationX = 0;
    }
    else {
        jumpVelocityX += initialJumpDragAccelerationX * deltaTime;
    }

    if (abs(initialJumpDragAccelerationY) * deltaTime > abs(jumpVelocityY)) {
        jumpVelocityY = 0;
        initialJumpDragAccelerationY = 0;
    }
    else {
        jumpVelocityY += initialJumpDragAccelerationY * deltaTime;
    }

    gravityVelocityY += GRAVITY_ACCELERATION * deltaTime;

    carRect = {xPos, yPos, width, height};
    tmp = getCoords(carRect, angle);
    x1 = tmp[0].x, y1 = tmp[0].y;
    x2 = tmp[1].x, y2 = tmp[1].y;
    x3 = tmp[2].x, y3 = tmp[2].y;
    x4 = tmp[3].x, y4 = tmp[3].y;

    minX = std::min({x1, x2, x3, x4});
    minY = std::min({y1, y2, y3, y4});
    maxX = std::max({x1, x2, x3, x4});
    maxY = std::max({y1, y2, y3, y4});

    if (maxY >= groundY) {
        float delta = maxY - groundY;
        y1 -= delta;
        y2 -= delta;
        y3 -= delta;
        y4 -= delta;
        yPos -= delta;
        maxY = std::max({y1, y2, y3, y4});
    }
}

void Car::dodgeDown() {
    if (spinningClockWise || spinningCounterClockWise) {
        return;
    }
    if (clockWise == 0) {
        spinningCounterClockWise = 1;
    }
    else {
        spinningClockWise = 1;
    }
    dodgingDown = 1;
    SDL_FRect carRect = {xPos, yPos, width, height};
    std::vector<Point> tmp = getCoords(carRect, angle);
    float x1 = tmp[0].x, y1 = tmp[0].y;
    float x2 = tmp[1].x, y2 = tmp[1].y;
    float x3 = tmp[2].x, y3 = tmp[2].y;
    float x4 = tmp[3].x, y4 = tmp[3].y;

    float minX = std::min({x1, x2, x3, x4});
    float minY = std::min({y1, y2, y3, y4});
    float maxX = std::max({x1, x2, x3, x4});
    float maxY = std::max({y1, y2, y3, y4});

    ///
    Point projected = findParallelVector(tmp[1], tmp[0], DODGE_VELOCITY);
    dodgeVelocityX = projected.x;
    dodgeVelocityY = projected.y;

    dodgeDragAccelerationX = -dodgeVelocityX * 0.28;
    dodgeDragAccelerationY = -dodgeVelocityY * 0.3;

    parallelBeforeDodgeA = tmp[1];
    parallelBeforeDodgeB = tmp[0];
}

void Car::dodgeUp() {

    if (spinningClockWise || spinningCounterClockWise) {
        return;
    }
    if (clockWise == 0) {
        spinningClockWise = 1;
    }
    else {
        spinningCounterClockWise = 1;
    }
    dodgingUp = 1;
    SDL_FRect carRect = {xPos, yPos, width, height};
    std::vector<Point> tmp = getCoords(carRect, angle);
    float x1 = tmp[0].x, y1 = tmp[0].y;
    float x2 = tmp[1].x, y2 = tmp[1].y;
    float x3 = tmp[2].x, y3 = tmp[2].y;
    float x4 = tmp[3].x, y4 = tmp[3].y;

    float minX = std::min({x1, x2, x3, x4});
    float minY = std::min({y1, y2, y3, y4});
    float maxX = std::max({x1, x2, x3, x4});
    float maxY = std::max({y1, y2, y3, y4});
    ///
    Point projected = findParallelVector(tmp[1], tmp[0], DODGE_VELOCITY);

    dodgeVelocityX = -projected.x;
    dodgeVelocityY = -projected.y;

    dodgeDragAccelerationX = -dodgeVelocityX * 0.28; // 0.3
    dodgeDragAccelerationY = -dodgeVelocityY * 0.3;

    parallelBeforeDodgeA = tmp[1];
    parallelBeforeDodgeB = tmp[0];
}

std::deque<SDL_FPoint> headBuffer;

float maxSpeed = 0;
void Car::draw() {
    maxSpeed = std::max(maxSpeed, sqrt((velocityX*velocityX) + (velocityY*velocityY)));
    correctAngle();
    correctPosition();

    SDL_FRect carRect = {xPos, yPos, width, height};
    std::vector<Point> tmp = getCoords(carRect, angle);
    float x1 = tmp[0].x, y1 = tmp[0].y;
    float x2 = tmp[1].x, y2 = tmp[1].y;
    float x3 = tmp[2].x, y3 = tmp[2].y;
    float x4 = tmp[3].x, y4 = tmp[3].y;

    float minX = std::min({x1, x2, x3, x4});
    float minY = std::min({y1, y2, y3, y4});
    float maxX = std::max({x1, x2, x3, x4});
    float maxY = std::max({y1, y2, y3, y4});

    std::vector<Point> projected = findPerpendicularVectors(tmp[1], tmp[0], JUMP_VELOCITY);

    Point projected_v1 = projected[0]; // clockwise
    Point projected_v2 = projected[1]; // counterclockwise

    float deltaVx, deltaVy;
    if (flip == SDL_FLIP_NONE) {
        deltaVx = projected_v1.x;
        deltaVy = projected_v1.y;
    }
    else {
        deltaVx = projected_v2.x;
        deltaVy = projected_v2.y;
    }

    SDL_FPoint center = { width/2, height/2 };

    SDL_FPoint carCenterPos = {xPos + width/2, yPos + height/2};

    if (curSign == 1) {
        addBoost(4, tmp[1], tmp[2], tmp[1], tmp[0], player);
    }
    else {

    }

    std::vector<Point> perpen = findPerpendicularVectors(tmp[1], tmp[0], JUMP_VELOCITY);

    SDL_RenderCopyExF(renderer.get(), carTexture.get(), NULL, &carRect, angle, &center, flip);

    updateBoost(player);
    renderBoost(player);
}

void Car::jump() {

    if (canJump) {
        correctPosition();
        onGround = false;
        jumping = true;

        SDL_FRect carRect = {xPos, yPos, width, height};
        std::vector<Point> tmp = getCoords(carRect, angle);
        float x1 = tmp[0].x, y1 = tmp[0].y;
        float x2 = tmp[1].x, y2 = tmp[1].y;
        float x3 = tmp[2].x, y3 = tmp[2].y;
        float x4 = tmp[3].x, y4 = tmp[3].y;

        float minX = std::min({x1, x2, x3, x4});
        float minY = std::min({y1, y2, y3, y4});
        float maxX = std::max({x1, x2, x3, x4});
        float maxY = std::max({y1, y2, y3, y4});

        std::vector<Point> projected = findPerpendicularVectors(tmp[1], tmp[0], JUMP_VELOCITY);

        Point projected_v1 = projected[0]; // clockwise
        Point projected_v2 = projected[1]; // counterclockwise

        float initialJumpVelocityX, initialJumpVelocityY;
        if (flip == SDL_FLIP_NONE) {
            initialJumpVelocityX = projected_v1.x;
            initialJumpVelocityY = projected_v1.y;
        }
        else {
            initialJumpVelocityX = projected_v2.x;
            initialJumpVelocityY = projected_v2.y;
        }

        jumpVelocityX = initialJumpVelocityX;
        jumpVelocityY = initialJumpVelocityY;

        projected = findPerpendicularVectors(tmp[1], tmp[0], JUMP_DRAG_ACCELERATION);

        if (flip == SDL_FLIP_NONE) {
            initialJumpDragAccelerationX = projected_v1.x;
            initialJumpDragAccelerationY = projected_v1.y;
        }
        else {
            initialJumpDragAccelerationX = projected_v2.x;
            initialJumpDragAccelerationY = projected_v2.y;
        }

        initialJumpDragAccelerationX *= -1;
        initialJumpDragAccelerationY *= -1;


        gravityVelocityY = 0;
        initialGravityAccelerationY = GRAVITY_ACCELERATION;
    }
}

void Car::setDir(bool newDir) {
    if (spinningClockWise || spinningCounterClockWise || (tilting && collideWithGround == 0)) {
//        std::cerr << "TILTING" << "\n";
        return;
    }
    if (newDir != curDir) {
        if (pointing != curDir) {

        }
        else {
//            std::cerr << (flip == SDL_FLIP_NONE ? "NONE" : "FLIP") << "\n";
            verticalFlip();
//            std::cerr << "vertical flipped" << "\n";
//            std::cerr << (flip == SDL_FLIP_NONE ? "NONE" : "FLIP") << "\n";
        }
    }
    else {
        if (pointing != curDir) {
//            std::cerr << (flip == SDL_FLIP_NONE ? "NONE" : "FLIP") << "\n";
            verticalFlip();
//            std::cerr << "vertical flipped ===================" << "\n";
//            std::cerr << (flip == SDL_FLIP_NONE ? "NONE" : "FLIP") << "\n";
        }
    }
    curDir = newDir;
}

void Car::handleGroundCollision() {
    if (spinningClockWise || spinningCounterClockWise) {
        return;
    }
    correctAngle();
    SDL_FRect carRect = {xPos, yPos, width, height};
    std::vector<Point> tmp = getCoords(carRect, angle);
    float x1 = tmp[0].x, y1 = tmp[0].y;
    float x2 = tmp[1].x, y2 = tmp[1].y;
    float x3 = tmp[2].x, y3 = tmp[2].y;
    float x4 = tmp[3].x, y4 = tmp[3].y;

    float minX = std::min({x1, x2, x3, x4});
    float minY = std::min({y1, y2, y3, y4});
    float maxX = std::max({x1, x2, x3, x4});
    float maxY = std::max({y1, y2, y3, y4});


    collideWithGround = 0;

    if (maxY >= groundY) {
        collideWithGround = 1;
        float delta = maxY - groundY;
        y1 -= delta;
        y2 -= delta;
        y3 -= delta;
        y4 -= delta;
        maxY = std::max({y1, y2, y3, y4});
        yPos -= delta;
    }

    float deltaRotate = 5; /// 20
    if (collideWithGround) {
        tilting = 0;
        if (angle == 0 || angle == 180) {
            curDir = pointing;
        }
        else if (angle > 0 && angle < 90) {
            float diff = angle;
            if (deltaRotate > diff) {
                deltaRotate = diff;
            }
            angle -= deltaRotate;
        }
        else if (angle == 90) {
            if (curDir == 1) {
                angle += deltaRotate;
            }
            else {
                angle -= deltaRotate;
            }
        }
        else if (angle > 90 && angle < 180) {
            float diff = 180 - angle;
            if (deltaRotate > diff) {
                deltaRotate = diff;
            }
            angle += deltaRotate;
        }
        else if (angle > 180 && angle < 270) {
            float diff = angle - 180;
            if (deltaRotate > diff) {
                deltaRotate = diff;
            }
            angle -= deltaRotate;
        }
        else if (angle == 270) {
            if (curDir == 1) {
                angle -= deltaRotate;
            }
            else {
                angle += deltaRotate;
            }
        }
        else if (angle > 270 && angle < 360) {
            float diff = 360 - angle;
            if (deltaRotate > diff) {
                deltaRotate = diff;
            }
            angle += deltaRotate;
        }
        carRect = {xPos, yPos, width, height};
        tmp = getCoords(carRect, angle);
        x1 = tmp[0].x, y1 = tmp[0].y;
        x2 = tmp[1].x, y2 = tmp[1].y;
        x3 = tmp[2].x, y3 = tmp[2].y;
        x4 = tmp[3].x, y4 = tmp[3].y;

        minX = std::min({x1, x2, x3, x4});
        minY = std::min({y1, y2, y3, y4});
        maxX = std::max({x1, x2, x3, x4});
        maxY = std::max({y1, y2, y3, y4});

        if (maxY < groundY) {
            float delta = groundY - maxY;
            y1 += delta;
            y2 += delta;
            y3 += delta;
            y4 += delta;
            maxY = std::max({y1, y2, y3, y4});
            yPos += delta;
        }
    }
    carRect = {xPos, yPos, width, height};
    tmp = getCoords(carRect, angle);
    x1 = tmp[0].x, y1 = tmp[0].y;
    x2 = tmp[1].x, y2 = tmp[1].y;
    x3 = tmp[2].x, y3 = tmp[2].y;
    x4 = tmp[3].x, y4 = tmp[3].y;

    minX = std::min({x1, x2, x3, x4});
    minY = std::min({y1, y2, y3, y4});
    maxX = std::max({x1, x2, x3, x4});
    maxY = std::max({y1, y2, y3, y4});

    if (maxY >= groundY) {
        float delta = maxY - groundY;
        y1 -= delta;
        y2 -= delta;
        y3 -= delta;
        y4 -= delta;
        maxY = std::max({y1, y2, y3, y4});
        yPos -= delta;
    }
}

