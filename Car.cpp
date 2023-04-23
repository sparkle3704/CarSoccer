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

Car car1(0, 0, 0);
Car car2(0, 0, 0);
Car::Car(float xPos, float yPos, int player):
    xPos(xPos), yPos(yPos), player(player) {

    if (player == 0) {
        return;
    }

    initTextures();

    if (car1_Texture == nullptr) {
        std::cerr << "WTFFF" << "\n";
    }
    if (player == 1) {
        carTexture = car1_Texture;
    }
    else {
        carTexture = car2_Texture;
    }

    int w, h;
    SDL_QueryTexture(carTexture, NULL, NULL, &w, &h);
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

//        std::cerr << maxX << ", " << "WINDOW_WIDTH = " << WINDOW_WIDTH << "\n";
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

//        std::cerr << maxX << ", " << "WINDOW_WIDTH = " << WINDOW_WIDTH << "\n";
    if (maxY >= groundY) {
//            std::cerr << "Hit ground" << "\n";
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
//            flip = SDL_FLIP_NONE;
//            curDir = 1;
        pointing = 0;
//            clockWise = 1;
    }
    else {
//            flip = SDL_FLIP_VERTICAL;
//            curDir = 0;
//            clockWise = 0;
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
    }
    else {
        if (curDir == 0) {
            flip = SDL_FLIP_NONE;
            clockWise = 0;
        }
    }
}

float Car::binS(float sign) { // find biggest so that angle +- ans errorless
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
    if (spinningClockWise == 0 && spinningCounterClockWise == 0) {
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

void Car::tiltUp() {
    if (spinningClockWise == 0 && spinningCounterClockWise == 0) {
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
}

void Car::moveRight() {
//        accelerationX = weight*0 / mass;
//        velocityX += accelerationX * dt;
//        xPos += velocityX * dt;
//        xPos += velocityX;
//        if (velocityX < 0) {
//            velocityX = 0;
//        }
//        else {
//            velocityX = abs(VX);
//        }
    clockWise = 1;
//        xPos += VELOCITY_X_FACTOR;
//        velocityX += VELOCITY_X_FACTOR;
    goingVelocityX = VELOCITY_X_FACTOR;
}

void Car::moveLeft() {
//        accelerationX = weight*0 / mass;
//        velocityX += accelerationX * dt;
//        xPos -= velocityX * dt;
//        if (velocityX > 0) {
//            velocityX = 0;
//        }
//        else {
//            velocityX = -abs(VX);
//        }
    clockWise = 0;
//        xPos -= VELOCITY_X_FACTOR;
    goingVelocityX = -VELOCITY_X_FACTOR;
//        velocityX -= VELOCITY_X_FACTOR;
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
//            playEffect(boostStartSound);
        if (prvSign == -1) {
            playEffectOnce(boostStartSound, boostStartChannel[player]);
            playEffect(boostEndSound, boostEndChannel[player]);
            if (velocityY == 0) {
                boostVelocity = abs(velocityX);
            }
            else {
//                    boostVelocity = 0;
//                    boostVelocity = (velocityX;
//                    boostVelocity = sqrt((velocityX + accelerationX*deltaTime)*(velocityX + accelerationX*deltaTime) + (velocityY + accelerationY*deltaTime)*(velocityY + accelerationY*deltaTime)) - sign*BOOST_ACCELERATION*deltaTime*2;

            }
//                boostVelocity = abs(goingVelocityX);
//                sqrt((velocityX + accelerationX*deltaTime)*(velocityX + accelerationX*deltaTime) + (velocityY + accelerationY*deltaTime)*(velocityY + accelerationY*deltaTime))
//                boostVelocity = sqrt((velocityX + accelerationX*deltaTime)*(velocityX + accelerationX*deltaTime) + (velocityY + accelerationY*deltaTime)*(velocityY + accelerationY*deltaTime)) - sign*BOOST_ACCELERATION*deltaTime*2;
            gravityVelocityY = GRAVITY_ACCELERATION/5;
        }
        else {
//                playEffect(boostEndSound, 0);
            goingVelocityX = 0;
            gravityVelocityY = GRAVITY_ACCELERATION/5;
        }
    }
    else {

        if (prvSign == 1) {
            stopEffect(boostEndChannel[player]);
//                playEffect(boostEndSound);
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
//        gravityVelocityY = 0;

    if (spinningClockWise || spinningCounterClockWise) {
//            boostVelocity = 0;
//            projected = findParallelVector(parallelBeforeDodgeA, parallelBeforeDodgeB, boostVelocity);
    }
    else {
        boostVelocity += sign*BOOST_ACCELERATION * deltaTime;
    }

//        boostVelocity += sign*BOOST_ACCELERATION * deltaTime;
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


//        std::vector<Point> projected = findPerpendicularVectors(tmp[1], tmp[0], JUMP_VELOCITY);
//        //clockwise - counterclockwise
//
//        Point projected_v1 = projected[0]; // clockwise
//        Point projected_v2 = projected[1]; // counterclockwise
//
//        float deltaVx, deltaVy;
//        if (flip == SDL_FLIP_NONE) {
//            deltaVx = projected_v1.x;
//            deltaVy = projected_v1.y;
//        }
//        else {
//            deltaVx = projected_v2.x;
//            deltaVy = projected_v2.y;
//        }
//
//        deltaVx *= -1;
//        deltaVy *= -1;




//        gravityVelocityY = GRAVITY;
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

//        if (onGround) {
//            velocityY = 0;
//        }
//        if (jumping && velocityY == 0 && !onGround) {
//            jumping = 0;
//        }
//        if (jumping) {
//            if (velocityY >= 0) {
//                jumping = 0;
//            }
//        }
//        else {
//            if (maxY >= groundY) {
//                onGround = 1;
//                velocityY = 0;
////                yPos = groundY - height + 1;
//                float delta = maxY - groundY;
//                yPos -= delta;
//                gravityVelocityY = 0;
//            }
//        }

//        xPos += velocityX;
//        yPos += velocityY;
//        velocityX += accelerationX + goingVelocity;
//        velocityY += gravityVelocityY;

//        if (boostVelocity != 0) {
//            goingVelocityX = 0;
////        }
//        if (curSign == 1 && prvSign == 1) {
//            goingVelocityX = 0;
//        }
    if (curSign == 1) {
        if (prvSign == 1) {
//                std::cerr << "====================" << "\n";
//                std::cerr << "====================" << "\n";
//                std::cerr << "====================" << "\n";
//                std::cerr << "====================" << "\n";
//                goingVelocityX = 0;
        }
    }
//        initialGravityAccelerationY = GRAVITY_ACCELERATION;
    if (spinningClockWise) {
        if (spinnedClockWise >= 360) { // 360
            spinningClockWise = 0;
            spinnedClockWise = 0;
//                dodgeVelocityX = 0;
//                dodgeVelocityY = 0;
            dodgingDown = 0;
            dodgingUp = 0;
            initialBoostAccelerationY = GRAVITY_ACCELERATION;
//                std::cerr << "done clock" << "\n";
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
//                dodgeVelocityX = 0;
//                dodgeVelocityY = 0;
            dodgingDown = 0;
            dodgingUp = 0;
            initialBoostAccelerationY = GRAVITY_ACCELERATION;
//                std::cerr << "done counterclock" << "\n";
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
//        std::cerr << "DODGE VELOCITY X = " << dodgeVelocityX << "\n";
//        std::cerr << "DODGE VELOCITY Y = " << dodgeVelocityY << "\n";
//        boostVelocity = std::min(boostVelocity, MAX_BOOST_VELOCITY);
//        boostVelocity = std::max(boostVelocity, 0.0f);

    /// ///////////////////////////////////////////////////////
    velocityX = goingVelocityX + jumpVelocityX + boostVelocityX + dodgeVelocityX + carBallCollisionVelocityX;
    velocityY =  gravityVelocityY + jumpVelocityY + boostVelocityY + dodgeVelocityY + carBallCollisionVelocityY;

    accelerationX = initialJumpDragAccelerationX + carBallCollisionDragAccelerationX + dodgeDragAccelerationX;
    accelerationY = initialGravityAccelerationY + initialJumpDragAccelerationY + carBallCollisionDragAccelerationY + dodgeDragAccelerationY;

    xPos += (velocityX + accelerationX * deltaTime);
    yPos += (velocityY + accelerationY * deltaTime);
//
//        std::cerr << "THIS PLACE? " << xPos << " " << yPos << "\n";
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
//        if (hasCollision()) {
//            dodgeVelocityX = 0;
//            dodgeVelocityY = 0;
//        }


//        if (abs(initialDodgeDragAccelerationX)*deltaTime > abs(dodgeVelocityX)) {
//            dodgeVelocityX = 0;
//            initialDodgeDragAccelerationX = 0;
//        }
//        else {
//            dodgeVelocityX += initialDodgeDragAccelerationX * deltaTime;
//        }
//
//        if (abs(initialDodgeDragAccelerationY)*deltaTime > abs(dodgeVelocityY)) {
//            dodgeVelocityY = 0;
//            initialDodgeDragAccelerationY = 0;
//        }
//        else {
//            dodgeVelocityY += initialDodgeDragAccelerationY * deltaTime;
//        }
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
//
//        if (curSign == -1) {
//            std::cerr << "stopping boost" << "\n";
//            if (abs(initialBoostAccelerationX) * deltaTime > abs(boostVelocityX)) {
//                boostVelocityX = 0;
//                initialBoostAccelerationX = 0;
//            }
//            else {
//                boostVelocityX += initialBoostAccelerationX * deltaTime;
//            }
//
//            if (abs(initialBoostAccelerationY) * deltaTime > abs(boostVelocityY)) {
//                boostVelocityY = 0;
//                initialBoostAccelerationY = 0;
//            }
//            else {
//                boostVelocityY += initialBoostAccelerationY * deltaTime;
//            }
//        }
//        else {
//            boostVelocityX += initialBoostAccelerationX * deltaTime;
//            boostVelocityY += initialBoostAccelerationY * deltaTime;
//        }

//        std::cerr << "boostX = " << boostVelocityX << "\n";
//        std::cerr << "boostY = " << boostVelocityY << "\n";
//        std::cerr << "jump velocity X = " << jumpVelocityX << ", " << "dragX = " << initialJumpDragAccelerationX << "\n";
//        std::cerr << "jump velocity Y = " << jumpVelocityY << ", " << "dragX = " << initialJumpDragAccelerationY << "\n";
    if (jumpVelocityY != 0) {
//            std::cerr << "JUMP VELOCITYX not 0++++++++" << "\n";
    }
    if (abs(initialJumpDragAccelerationX) * deltaTime > abs(jumpVelocityX)) {
//            std::cerr << "resetting jump and drag accer X to 0 ================" << " " << initialJumpDragAccelerationX << " " << jumpVelocityX << "\n";
//            std::cerr << "resetting jump and drag accer X to 0 ================" << " " << initialJumpDragAccelerationX << " " << jumpVelocityX << "\n";
//            std::cerr << "resetting jump and drag accer X to 0 ================" << " " << initialJumpDragAccelerationX << " " << jumpVelocityX << "\n";
//            std::cerr << "resetting jump and drag accer X to 0 ================" << " " << initialJumpDragAccelerationX << " " << jumpVelocityX << "\n";

        jumpVelocityX = 0;
        initialJumpDragAccelerationX = 0;
    }
    else {
        jumpVelocityX += initialJumpDragAccelerationX * deltaTime;
    }

    if (abs(initialJumpDragAccelerationY) * deltaTime > abs(jumpVelocityY)) {
//            std::cerr << "resetting jump and drag accer Y to 0 ================" << " " << initialJumpDragAccelerationY << " " << jumpVelocityY << "\n";
//            std::cerr << "resetting jump and drag accer Y to 0 ================" << " " << initialJumpDragAccelerationY << " " << jumpVelocityY << "\n";
//            std::cerr << "resetting jump and drag accer Y to 0 ================" << " " << initialJumpDragAccelerationY << " " << jumpVelocityY << "\n";
//            std::cerr << "resetting jump and drag accer Y to 0 ================" << " " << initialJumpDragAccelerationY << " " << jumpVelocityY << "\n";
        jumpVelocityY = 0;
        initialJumpDragAccelerationY = 0;
    }
    else {
        jumpVelocityY += initialJumpDragAccelerationY * deltaTime;
    }



//        if (abs(initialGravityAccelerationY) * deltaTime > abs(gravityVelocityY)) {
//            gravityVelocityY = 0;
//            initialGravityAccelerationY = 0;
//        }
//        else {
//            gravityVelocityY +=
//        }

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

//        initialDodgeDragAccelerationX = -dodgeVelocityX;
//        initialDodgeDragAccelerationY = -dodgeVelocityY;

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
//
//        initialDodgeDragAccelerationX = -dodgeVelocityX;
//        initialDodgeDragAccelerationY = -dodgeVelocityY;

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

    //clockwise - counterclockwise

    Point projected_v1 = projected[0]; // clockwise
    Point projected_v2 = projected[1]; // counterclockwise

//        std::cerr << "jump velocity x = " << jumpVelocityX << "\n";
//        std::cerr << "jump velocity y = " << jumpVelocityY << "\n";
    float deltaVx, deltaVy;
    if (flip == SDL_FLIP_NONE) {
        deltaVx = projected_v1.x;
        deltaVy = projected_v1.y;
    }
    else {
        deltaVx = projected_v2.x;
        deltaVy = projected_v2.y;
    }
//        std::cerr << "gravity velocity y = " << gravityVelocityY << "\n";
//        std::cerr << "A(" << tmp[1].x << ", " << tmp[1].y << ")" << " ";
//        std::cerr << "B(" << tmp[0].x << ", " << tmp[0].y << ")" << "\n";
//        std::cerr << "jump VX = " << deltaVx << "\n";
//        std::cerr << "jump VY = " << deltaVy << "\n";
//        int w, h;
//        SDL_QueryTexture(carTexture, NULL, NULL, &w, &h);
//
//        float scalef = 100.0f / w;
//        w *= scale;
//        h *= scale;
//
//        std::cerr << "\n";
//        std::cerr << "car width = " << width << ", " << "car height = " << height << "\n";
//        std::cerr << "======" << "\n";
//        std::cerr << "xPos = " << xPos << ", " << "yPos = " << yPos << "\n";
//        SDL_FPoint center = {width/2, height/2};
    ///
    SDL_FPoint center = { width/2, height/2 };

    SDL_FPoint carCenterPos = {xPos + width/2, yPos + height/2};


    SDL_FPoint trackedPoint = {(tmp[0].x + tmp[3].x)/2, (tmp[0].y + tmp[3].y)/2};
    trackedPoint = {(tmp[0].x + tmp[1].x + tmp[2].x + tmp[3].x)/4, (tmp[0].y + tmp[1].y + tmp[2].y + tmp[3].y)/4};
    headBuffer.push_front(trackedPoint);
    if ((int)headBuffer.size() > MAX_HEAD_TRAIL_LENGTH) {
        headBuffer.pop_back();
    }

    SDL_FPoint __a = {tmp[1].x, tmp[1].y};
    SDL_FPoint __b = {tmp[2].x, tmp[2].y};

//        BoostParticle boostParticle = {__a, __b, flip, angle};
//        BoostParticle boostParticle = {ParticleSystem(renderer, 100, tmp[1], tmp[2])};

    if (curSign == 1) {
        addBoost(4, tmp[1], tmp[2], tmp[1], tmp[0], player);
    }
    else {
//            if ((int)boostBuffer.size() > 0) {
//                boostBuffer.pop_back();
//            }
    }
//        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
//        SDL_RenderClear(renderer);
//        for (int i = 1; i < (int)headBuffer.size(); ++i) {
//            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
//            SDL_RenderDrawLine(renderer, headBuffer[i-1].x, headBuffer[i-1].y, headBuffer[i].x, headBuffer[i].y);
////            SDL_RenderCopyExF(renderer, carTexture, NULL, &carRect, angle, &center, flip);
////            SDL_RenderPresent(renderer);
//        }
    std::vector<Point> perpen = findPerpendicularVectors(tmp[1], tmp[0], JUMP_VELOCITY);
//        std::cerr <<

//        float cosAngle = cos(angle * M_PI / 180.0f);
//        float sinAngle = sin(angle * M_PI / 180.0f);
//        carRect.x += center.x - (center.x * cosAngle - center.y * sinAngle);
//        carRect.y += center.y - (center.y * cosAngle + center.x * sinAngle);

//        SDL_FPoint carRectCenter = { xPos + width/2, yPos + height/2 };
//        for (int i = 0; i < 2; ++i) {
//            for (int j = 0; j < 2; ++j) {
////                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
////                SDL_RenderDrawPoint(renderer, tmp[i][j].x, tmp[i][j].y);
//                std::cerr << "[" << tmp[i][j].x << ",  " << tmp[i][j].y << "]" << "\n";
//            }
//            cout << endl;
//        }
//        std::cerr << "----------------" << "\n";
//        std::cerr << angle << "\n";
//        std::cerr << "correct angle = " << angle << "\n";
    bool testThis = 0;
    if (testThis) {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        for (int i = 0; i < 4; ++i) {
//            std::cerr << "[" << tmp[i].x << ",  " << tmp[i].y << "]" << "\n";
            std::cerr << "<" << i+1 << ">";
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderClear(renderer);

            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_RenderDrawPoint(renderer, tmp[i].x, tmp[i].y);
            SDL_RenderCopyExF(renderer, carTexture, NULL, &carRect, angle, &center, SDL_FLIP_VERTICAL);
            SDL_RenderPresent(renderer);
            SDL_Delay(1000);
        }
        std::cerr << "\n";
    }

//        std::cerr << getAngle(tmp) << "\n";
//        std::cerr << "----------------" << "\n";
//        std::cerr << "\n";
//        for (int i = 0; i < 4; ++i) {
//            for (int j = 0; j < 2; ++j) {
//                std::cerr << "[" << tmp[i][j].x << ",  " << tmp[i][j].y << "]" << "\n";
//            }
//        }
//        SDL_RenderClear(renderer);
//        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
//        std::cerr << (clockWise ? "clockwise" : "notCLockwise") << " ";
//        std::cerr << (curDir ? "going right" : "going left") << " ";
//        std::cerr << (pointing ? "pointing right" : "pointing left");
//        std::cerr << "\n";

//        std::cerr << xPos << " " << yPos << "\n";
//        std::cerr << "******************" << "\n";
//        for (int i = 0; i < 4; ++i) {
//            std::cerr << "[" << tmp[i].x << ",  " << tmp[i].y << "]" << "\n";
//        }
//        std::cerr << "******************" << "\n";
//        std::cerr << "angle = " << angle << "\n";
//        std::cerr << (clockWise ? "clockwise      " : "not CLockwise") << "\n";
//        std::cerr << "curDir = " << curDir << ", " << (curDir ? "going right" : "going left  ") << "\n";
//        std::cerr << "pointing = " << pointing << ", " << (pointing ? "pointing right" : "pointing left  ") << "\n";
//        std::cerr << (flip == SDL_FLIP_VERTICAL ? "vertical flipped" : "                      ") << "\n";
//        std::cerr << "\n";
    SDL_RenderCopyExF(renderer, carTexture, NULL, &carRect, angle, &center, flip);

    for (int i = 1; i < (int)headBuffer.size(); ++i) {
        float maxSpeed = 14.2;
        float curSpeed = sqrt((velocityX*velocityX) + (velocityY*velocityY));
        std::vector<int> getRGB = calculateColor(curSpeed, maxSpeed);
        SDL_SetRenderDrawColor(renderer, getRGB[0], getRGB[1], getRGB[2], 255);
        SDL_RenderDrawLine(renderer, headBuffer[i-1].x, headBuffer[i-1].y, headBuffer[i].x, headBuffer[i].y);
//            SDL_RenderCopyExF(renderer, carTexture, NULL, &carRect, angle, &center, flip);
//            SDL_RenderPresent(renderer);
    }
    updateBoost(player);
    renderBoost(player);

//        SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
//        SDL_RenderDrawLineF(renderer, tmp[1].x, tmp[1].y, tmp[0].x, tmp[0].y);

//        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
//        SDL_RenderDrawPointF(renderer, tmp[0].x, tmp[0].y);

//        SDL_Texture* tmpTexture = NULL;
//        tmpTexture = IMG_LoadTexture(renderer, "D:/gameProject/game/assets/textures/66598-ball.png");
//        SDL_FRect tmpRect = {tmp[0].x - 12/2, tmp[0].y - 12/2, 12, 12};
//        SDL_FPoint tmpCenter = {12, 12};
//        SDL_RenderCopyExF(renderer, tmpTexture, NULL, &tmpRect, 0, &tmpCenter, SDL_FLIP_NONE);
//        SDL_DestroyTexture(tmpTexture);
//        tmpTexture = NULL;
//        drawLine(U, V, RRRR, GGGG, BBBB);
}

void Car::jump() {
//        if (onGround) {
//            std::cerr << "jumping now, was on ground" << "\n";
//            velocityY = -9;
//            onGround = false;
//            jumping = true;
//        }
    if (canJump) {
//            Uint32 startJumpTime = SDL_GetTicks();

        correctPosition();
//            std::cerr << "jumping!" << "\n";
//            velocityY -= 9;
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
        //clockwise - counterclockwise

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

//            std::cerr << "jump delta vx: " << initialJumpVelocityX << "\n";
//            std::cerr << "jump delta vy: " << initialJumpVelocityY << "\n";

        jumpVelocityX = initialJumpVelocityX;
        jumpVelocityY = initialJumpVelocityY;

        projected = findPerpendicularVectors(tmp[1], tmp[0], JUMP_DRAG_ACCELERATION);

//            float initialJumpDragAccelerationX initialJumpDragAccelerationY;
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
//            jumpDragVelocityX += initialJumpDragAccelerationY * deltaTime;
//            jumpDragVelocityY = 0;
//            velocityX += initialJumpVelocityX;
//            velocityY += deltaVy;
    }

}

void Car::setDir(bool newDir) {
//        curDir = newDir;
    if (newDir != curDir) {
        if (pointing != curDir) {

        }
        else {
            verticalFlip();
        }
//            std::cerr << "changed curDir" << "\n";
//            verticalFlip();
        curDir = newDir;
    }
}

void Car::handleGroundCollision() {
    if (spinningClockWise || spinningCounterClockWise) {
        return;
    }
//        int width = 100, height = 50;
//        SDL_QueryTexture(carTexture, NULL, NULL, &width, &height);
//
//        float scale = 100.0f / w;
//        w *= scale;
//        height *= scale;

//        SDL_FPoint center = {w/2, height/2};
//        SDL_FPoint center = { w/2, height/2 };



//        correctAngle();
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

    bool collideWithGround = 0;
    if (maxY >= groundY) {
        collideWithGround = 1;
        float delta = maxY - groundY;
//            std::cerr << "Collapsed with ground, delta=" << delta << "\n";
        y1 -= delta;
        y2 -= delta;
        y3 -= delta;
        y4 -= delta;
        maxY = std::max({y1, y2, y3, y4});
        yPos -= delta;
    }


    float deltaRotate = 20;
    if (collideWithGround) {
        if (angle > 0 && angle < 90) {
//                std::cerr << "<11111111111111111>" << "\n";
            float diff = angle;
            if (deltaRotate > diff) {
                deltaRotate = diff;
            }
            angle -= deltaRotate;
        }
        else if (angle > 90 && angle < 180) {
//                std::cerr << "<22222222222222222>" << "\n";
            float diff = 180 - angle;
            if (deltaRotate > diff) {
                deltaRotate = diff;
            }
            angle += deltaRotate;
        }
        else if (angle > 180 && angle < 270) {
//                std::cerr << "<33333333333333333>" << "\n";
            float diff = angle - 180;
            if (deltaRotate > diff) {
                deltaRotate = diff;
            }
            angle -= deltaRotate;
        }
        else if (angle > 270 && angle < 360) {
//                std::cerr << "<444444444444444444>" << "\n";
            float diff = 360 - diff;
            if (deltaRotate > diff) {
                deltaRotate = diff;
            }
            angle += deltaRotate;
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
//            std::cerr << "Collapsed with ground, delta = " << delta << "\n";
        y1 -= delta;
        y2 -= delta;
        y3 -= delta;
        y4 -= delta;
        maxY = std::max({y1, y2, y3, y4});
        yPos -= delta;
    }
}

