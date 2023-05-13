#ifndef BALL_H_INCLUDED
#define BALL_H_INCLUDED

#pragma once
#include <deque>
#include <SDL2/SDL.h>
#include "Structs.h"
#include "Constants.h"

class Ball {
public:
    Ball() {};
    Ball(float xPos, float yPos, float radius);
    float xPos;
    float yPos;
    float radius;
    float velocityX = 0;
    float velocityY = 0;
    float mass = 50;
    float omega = 0;
    bool inMidAir = 0;

    void moveBall();
    void draw();
    void resetBall();

private:
    float accelerationX = 0;
    float accelerationY = 0;
    float angle = 0;
    float restitution = 0.9;
    float carBallCollisionVelocityX = 0;
    float carBallCollisionVelocityY = 0;
    float carBallCollisionDragAccelerationX = 0;
    float carBallCollisionDragAccelerationY = 0;
    float gravityVelocityY = 0;
    float initialGravityAccelerationY = GRAVITY_ACCELERATION;
    float dampening = 0.99;
    bool actuallyTouchingDown = 0;
    bool actuallyTouchingUp = 0;
    float gravityAcc = 0;
    float prvXpos;
    float prvYpos;

    int droppedOn = 0;
    std::deque<SDL_FPoint> centerBuffer;

    void draw(SDL_Renderer* renderer);

    void handleSpin();

    bool touchLine(vec2 _A, vec2 _B);

    bool touchLineDown1();

    bool touchFloorBackGoal1();

    bool touchLineUp1();

    bool touchCeilBackGoal1();

    bool touchLineDown2();

    bool touchFloorBackGoal2();

    bool touchLineUp2();

    bool touchCeilBackGoal2();

    vec2 getIntersection(vec2 A, vec2 B, vec2 P);

    int orientation(vec2 p1, vec2 p2, vec2 p3);

    bool Chk2(vec2 A, vec2 B, bool down);

};

extern bool noGroundMode;
extern void createBall();
extern Ball ball;

#endif // BALL_H_INCLUDED
