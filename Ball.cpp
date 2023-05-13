#include "Ball.h"
#include <vector>>
#include <SDL2/SDL.h>
#include "CommonFunctions.h"
#include "SDL_Contexts.h"
#include <algorithm>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "Explosion.h"
#include <iostream>
#include "Sounds.h"
#include "PlayerScored.h"
#include "Stats.h"

bool noGroundMode = 0;
Ball ball;
Ball::Ball(float xPos, float yPos, float radius):
    xPos(xPos), yPos(yPos), radius(radius) {

    if (radius == 0) {
        return;
    }

    resetBall();
    prvXpos = xPos;
    prvYpos = yPos;
}

void createBall() {
    initTextures();
    ball = Ball(WINDOW_WIDTH/2 - 5, WINDOW_HEIGHT/3, 25);
}

void Ball::draw() {
    SDL_FPoint ballCenter = {xPos + radius, yPos + radius};

    SDL_FPoint center = {radius, radius};

    SDL_FRect ballRect = {xPos, yPos, radius*2, radius*2};
    std::vector<Point> tmp = getCoords(ballRect, angle);
    float x1 = tmp[0].x, y1 = tmp[0].y;
    float x2 = tmp[1].x, y2 = tmp[1].y;
    float x3 = tmp[2].x, y3 = tmp[2].y;
    float x4 = tmp[3].x, y4 = tmp[3].y;

    float minX = std::min({x1, x2, x3, x4});
    float minY = std::min({y1, y2, y3, y4});
    float maxX = std::max({x1, x2, x3, x4});
    float maxY = std::max({y1, y2, y3, y4});

    SDL_RenderCopyExF(renderer.get(), ballTexture.get(), NULL, &ballRect, angle, &center, SDL_FLIP_NONE);
    updateExplosion(1);
    renderExplosion(1);
    updateExplosion(2);
    renderExplosion(2);
}

void Ball::handleSpin() {
        vec2 ballCenter = {xPos + radius, yPos + radius};
        vec2 direction = {ballCenter.x + velocityX, ballCenter.y + velocityY};
        vec2 normalizedDirection = direction.normalized();
        vec2 velocity = {velocityX, velocityY};

        float stiff = 2;
        if (yPos + 2*radius >= groundY || actuallyTouchingDown) { /// touchDown
            float deltaX = (xPos - prvXpos);
            omega = deltaX*stiff;
            if (actuallyTouchingDown) {

            }
        }
        else if (xPos <= 0 && (!(yPos >= topLeft1.y && yPos + 2*radius <= botLeft1.y))) { /// touchLeft
            float deltaY = yPos - prvYpos;
            omega = deltaY*stiff;
        }
        else if (xPos + 2*radius >= WINDOW_WIDTH && (!(yPos >= topLeft1.y && yPos + 2*radius <= botLeft1.y))) { /// touchRight
            float deltaY = yPos - prvYpos;
            omega = -deltaY*stiff;
        }
        else if (yPos <= 0 || actuallyTouchingUp) { /// touchUp
            float deltaX = (xPos - prvXpos);
            omega = deltaX*stiff;
        }
        else { /// midAir
            omega *= dampening;
        }
        angle += omega;
        prvXpos = xPos;
        prvYpos = yPos;
}

bool Ball::touchLine(vec2 _A, vec2 _B) {
    Point A = {_A.x, _A.y};
    Point B = {_B.x, _B.y};
    Point C = {xPos + radius, yPos + radius};
    float r = radius;

    double distSq = ((C.x - A.x) * (B.y - A.y) - (C.y - A.y) * (B.x - A.x)) *
                    ((C.x - A.x) * (B.y - A.y) - (C.y - A.y) * (B.x - A.x)) /
                    (pow(B.x - A.x, 2) + pow(B.y - A.y, 2));

    // Check if the squared distance is less than or equal to the square of the circle's radius
    if (distSq <= r * r && ((C.x - A.x) * (C.x - B.x) + (C.y - A.y) * (C.y - B.y)) <= 0) {
        return true; // Line segment intersects with circle
    } else {
        return false; // Line segment does not intersect with circle
    }
}

bool Ball::touchLineDown1() {
    return Chk2(botLeft1, botRight1, 1);
}

bool Ball::touchFloorBackGoal1() {
    return Chk2(floorBackGoal1, botLeft1, 1);
}

bool Ball::touchLineUp1() {
    return Chk2(topLeft1, topRight1, 0);
}

bool Ball::touchCeilBackGoal1() {
    return Chk2(ceilBackGoal1, topLeft1, 0);
}

bool Ball::touchLineDown2() {
    return Chk2(botLeft2, botRight2, 1);
}

bool Ball::touchFloorBackGoal2() {
    return Chk2(botRight2, floorBackGoal2, 1);
}

bool Ball::touchLineUp2() {
    return Chk2(topLeft2, topRight2, 0);
}

bool Ball::touchCeilBackGoal2() {
    return Chk2(topRight2, ceilBackGoal2, 0);
}

void Ball::resetBall() {
    xPos = WINDOW_WIDTH*0.5 - radius;
    yPos = WINDOW_HEIGHT*0.5 - radius;
    velocityX = 0;
    velocityY = 0;
    accelerationX = 0;
    accelerationY = 0;
    omega = 0;
    droppedOn = 0;
}

vec2 Ball::getIntersection(vec2 A, vec2 B, vec2 P) {
    double x1 = A.x, y1 = A.y;
    double x2 = B.x, y2 = B.y;
    double x3 = P.x, y3 = P.y;

    double px = x2 - x1;
    double py = y2 - y1;
    double dAB = px * px + py * py;

    double u = ((x3 - x1) * px + (y3 - y1) * py) / dAB;
    double x = x1 + u * px;
    double y = y1 + u * py;

    return vec2(x, y);
}

int Ball::orientation(vec2 p1, vec2 p2, vec2 p3) {
    float val = (p2.y - p1.y) * (p3.x - p2.x)
              - (p2.x - p1.x) * (p3.y - p2.y);

    if (val == 0)
        return 0; // collinear

    return (val < 0) ? -1 : 1; // clockwise, counterclockwise
    /// clockwise:         -1
    /// counterclockwise:   1
}


bool Ball::Chk2(vec2 A, vec2 B, bool down) {
    int type;
    if (down == 1) {
        type = 1;
    }
    else {
        type = -1;
    }
    vec2 center = {xPos + radius, yPos + radius};
    vec2 pointOfContact = getIntersection(A, B, center);
//    drawLine(center, pointOfContact, 255, 7, 244);
//    drawLine(B, center, 255, 255, 244);
//    drawLine(A, center, 255, 255, 244);
//    drawLine(pointOfContact, center, 255, 255, 244);
//    drawLine(B, center, 255, 255, 244);

    float d1 = distance(A.x, A.y, pointOfContact.x, pointOfContact.y);
    float d2 = distance(B.x, B.y, pointOfContact.x, pointOfContact.y);
    float dAB = distance(A.x, A.y, B.x, B.y);
    if (d1 + d2 - dAB == 0) {
        float dist = distance(center.x, center.y, pointOfContact.x, pointOfContact.y);
        if (dist <= radius) {

            if (orientation(A, B, center) == type && type*velocityY >= 0) {
                float delta = radius - dist;
                vec2 newPos = {xPos, yPos};
                newPos = newPos + (center - pointOfContact).normalized()*delta*type;
                xPos = newPos.x;
                yPos = newPos.y;
                return 1;
            }
        }
    }
    return 0;
}

void Ball::moveBall() {
//    drawLine(topRight1, botRight1, 255, 4, 242);
//    drawLine(botRight1, botLeft1, 255, 4, 242);
//    drawLine(botLeft1, topLeft1, 255, 4, 242);
//    drawLine(topLeft1, topRight1, 255, 4, 242);
//
//    drawLine(topRight2, botRight2, 255, 4, 242);
//    drawLine(botRight2, botLeft2, 255, 4, 242);
//    drawLine(botLeft2, topLeft2, 255, 4, 242);
//    drawLine(topLeft2, topRight2, 255, 4, 242);
//
//    drawLine(floorBackGoal1, botLeft1, 255, 4, 242);
//    drawLine(botRight2, floorBackGoal2, 255, 4, 242);
//    drawLine(ceilBackGoal1, topLeft1, 255, 4, 242);
//    drawLine(topRight2, ceilBackGoal2, 255, 4, 242);

    accelerationX = carBallCollisionDragAccelerationX*0;
    accelerationY = carBallCollisionDragAccelerationY*0 + initialGravityAccelerationY;

    xPos += (velocityX + accelerationX * deltaTime);
    yPos += (velocityY + accelerationY * deltaTime);


    velocityX += accelerationX*deltaTime;
    velocityY += GRAVITY_ACCELERATION*deltaTime;


    velocityX *= dampening;
    velocityY *= dampening;

    /// ////////////////////////////////////////////////////////

    actuallyTouchingDown = 0;
    actuallyTouchingUp = 0;

    inMidAir = 1;
    if (Chk2(botLeft1, botRight1, 1) || Chk2(floorBackGoal1, botLeft1, 1) || Chk2(botLeft2, botRight2, 1) || Chk2(botRight2, floorBackGoal2, 1)) {
        velocityY *= -RESTITUTION;
        actuallyTouchingDown = 1;
        if (abs(velocityY) > 0.16) {
            playEffectOnce(ballHitSound, ballHitChannel);
        }
        inMidAir = 0;
    }
    if (Chk2(topLeft1, topRight1, 0) || Chk2(ceilBackGoal1, topLeft1, 0) || Chk2(topLeft2, topRight2, 0) || Chk2(topRight2, ceilBackGoal2, 0)) {
        velocityY *= -RESTITUTION;
        actuallyTouchingUp = 1;
        if (abs(velocityY) > 0.16) {
            playEffectOnce(ballHitSound, ballHitChannel);
        }
    }

    if (!((((orientation(topLeft1, topRight1, vec2(xPos + radius, yPos)) != 1) && /// -1                    1
       (orientation(botLeft1, botRight1, vec2(xPos + radius, yPos + 2*radius)) != -1) && /// 1              2
       (orientation(floorBackGoal1, botLeft1, vec2(xPos + radius, yPos + 2*radius)) != -1) && /// 1         3
       (orientation(ceilBackGoal1, topLeft1, vec2(xPos + radius, yPos)) != 1)) || /// -1                    4
       ((orientation(topLeft2, topRight2, vec2(xPos + radius, yPos)) != 1) && /// -1                        5
       (orientation(botLeft2, botRight2, vec2(xPos + radius, yPos + 2*radius)) != -1) && /// 1              6
       (orientation(topRight2, ceilBackGoal2, vec2(xPos + radius, yPos)) != 1) && /// -1                    7
       (orientation(botRight2, floorBackGoal2, vec2(xPos + radius, yPos + 2*radius)) != -1)) ||  /// 1      8
        actuallyTouchingDown || actuallyTouchingUp))) { /// 1

            if (xPos < 0) { /// left
                xPos = 0;
                restitution = RESTITUTION;
                velocityX *= -RESTITUTION;
                playEffectOnce(ballHitSound, ballHitChannel);
            }
            if (xPos + 2*radius > WINDOW_WIDTH) { /// right
                restitution = -RESTITUTION;
                xPos = WINDOW_WIDTH - 2*radius;
                velocityX *= -RESTITUTION;
                playEffectOnce(ballHitSound, ballHitChannel);
            }

            if (noGroundMode) {
                if (yPos > WINDOW_HEIGHT) {
                    if (xPos + radius > WINDOW_WIDTH*0.5) {
                        droppedOn = 2;
                        scoreB += 1;
                    }
                    else if (xPos + radius < WINDOW_WIDTH*0.5) {
                        droppedOn = 1;
                        scoreA += 1;
                    }
                    else {
                        droppedOn = 0;
                    }
                    resetBall();
                    inMidAir = 0;
                }
            }
            else {
                if (yPos + 2*radius > groundY) { /// down
                    yPos = groundY - 2*radius;
                    velocityY *= -RESTITUTION*2;
                    restitution = -RESTITUTION;
                    if (abs(velocityY) > 0.16) {
                        playEffectOnce(ballHitSound, ballHitChannel);
                    }
                    inMidAir = 0;
                }
            }
            if (yPos < 0) { /// up
                restitution = RESTITUTION;
                yPos = 0;
                velocityY *= -RESTITUTION;
                playEffectOnce(ballHitSound, ballHitChannel);
            }
    }
    else {
        if (xPos + 2*radius < botLeft1.x) {
            xPos = botLeft1.x - 2*radius;
            restitution = RESTITUTION;
            velocityX *= -RESTITUTION*3;
            velocityY *= -RESTITUTION*3;
            playEffectOnce(explosionSound, explosionChannel);
            ball.inMidAir = 0;
            printPlayerScored(2);
            ++scoreB;
            addExplosion(1000, xPos, yPos, 2);
            if (inOvertime || (timeLeft <= 0 && countDown && scoreA != scoreB)) {
                addExplosion(1000, 1920 - xPos, yPos, 1);
                resetBall();
                xPos = WINDOW_WIDTH*0.5 - radius;
                yPos = groundY - 2*radius;
            }
            else {
                resetBall();
            }

        }
        if (xPos > topRight2.x) {
            xPos = topRight2.x;
            restitution = -RESTITUTION;
            velocityX *= -RESTITUTION*3;
            velocityY *= -RESTITUTION*3;
            playEffectOnce(explosionSound, explosionChannel);
            ++scoreA;
            ball.inMidAir = 0;
            addExplosion(1000, xPos, yPos, 1);
            if (inOvertime || (timeLeft <= 0 && countDown && scoreA != scoreB)) {
                addExplosion(1000, 1920 - xPos, yPos, 2);
                resetBall();
                xPos = WINDOW_WIDTH*0.5 - radius;
                yPos = groundY - 2*radius;
            }
            else {
                resetBall();
            }
            printPlayerScored(1);

        }
    }

    handleSpin();
}
