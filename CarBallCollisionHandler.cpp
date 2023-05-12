#include "CarBallCollisionHandler.h"
#include "CommonFunctions.h"
#include <vector>
#include "Constants.h"
#include "Sounds.h"
#include "Structs.h"

vec2 rotateVec2(const vec2& v, float angle) {
    float s = sin(angle);
    float c = cos(angle);
    return vec2{v.x * c - v.y * s, v.x * s + v.y * c};
}

vec2 nearestPointOnOBB(const obb a, const vec2 b) {
    vec2 closest = b - a.center;

    float angle = -a.angle;
    float s = sin(angle);
    float c = cos(angle);

    vec2 rotated(closest.x * c - closest.y * s, closest.x * s + closest.y * c);

    vec2 clamped(rotated.x, rotated.y);
    clamped.x = std::max(clamped.x, -a.halfWidth);
    clamped.x = std::min(clamped.x, a.halfWidth);
    clamped.y = std::max(clamped.y, -a.halfHeight);
    clamped.y = std::min(clamped.y, a.halfHeight);

    vec2 unrotated(clamped.x * c + clamped.y * s, -clamped.x * s + clamped.y * c);

    return a.center + unrotated;
}


bool intersectCarBall(const obb a, const sphere b, vec2& collision_normal, vec2& p) {
    p = nearestPointOnOBB(a, b.center);
    vec2 v = p - b.center;
    if (v.length() <= b.radius) {
//        drawLine(p, b.center, 0, 128, 0);
        collision_normal = v.normalized();
        return 1;
    }
    else {
//        drawLine(p, b.center, 255, 0, 0);
        return 0;
    }
}

obb carToObb(Car car) {
    SDL_FRect carRect = {car.xPos, car.yPos, car.width, car.height};
    std::vector<Point> tmp = getCoords(carRect, car.angle);
    float x1 = tmp[0].x, y1 = tmp[0].y;
    float x2 = tmp[1].x, y2 = tmp[1].y;
    float x3 = tmp[2].x, y3 = tmp[2].y;
    float x4 = tmp[3].x, y4 = tmp[3].y;

    float minX = std::min({x1, x2, x3, x4});
    float minY = std::min({y1, y2, y3, y4});
    float maxX = std::max({x1, x2, x3, x4});
    float maxY = std::max({y1, y2, y3, y4});

    float center_x = (x1 + x2 + x3 + x4) / 4.0f;
    float center_y = (y1 + y2 + y3 + y4) / 4.0f;

    float theta = car.angle * M_PI / 180.0f;

    float half_width = car.width / 2.0f;
    float half_height = car.height / 2.0f;

    obb result((0, 0), 0, 0, 0);
    result.center.x = center_x;
    result.center.y = center_y;
    result.halfWidth = half_width;
    result.halfHeight = half_height;
    result.angle = theta;

//    drawLine(vec2(x1, y1), vec2(x2, y2), 0, 255, 0);
//    drawLine(vec2(x2, y2), vec2(x3, y3), 0, 255, 0);
//    drawLine(vec2(x3, y3), vec2(x4, y4), 0, 255, 0);
//    drawLine(vec2(x4, y4), vec2(x1, y1), 0, 255, 0);

    return result;
}

sphere ballToSphere(Ball ball) {
    sphere ball_sphere;
    ball_sphere.center = vec2{ball.xPos + ball.radius, ball.yPos + ball.radius};
    ball_sphere.radius = ball.radius;
    return ball_sphere;
}

float distance(vec2 a, vec2 b) {
    return (a - b).length();
}

void handleCollisionCarBall(Car& car, Ball& ball) {
    SDL_FRect carRect = {car.xPos, car.yPos, car.width, car.height};
    std::vector<Point> tmp = getCoords(carRect, car.angle);
    float x1 = tmp[0].x, y1 = tmp[0].y;
    float x2 = tmp[1].x, y2 = tmp[1].y;
    float x3 = tmp[2].x, y3 = tmp[2].y;
    float x4 = tmp[3].x, y4 = tmp[3].y;

    float minX = std::min({x1, x2, x3, x4});
    float minY = std::min({y1, y2, y3, y4});
    float maxX = std::max({x1, x2, x3, x4});
    float maxY = std::max({y1, y2, y3, y4});

    float center_x = (x1 + x2 + x3 + x4) / 4.0f;
    float center_y = (y1 + y2 + y3 + y4) / 4.0f;

    obb car_obb = carToObb(car);

    sphere ball_sphere = ballToSphere(ball);

    vec2 collision_normal;
    vec2 p;
    if (intersectCarBall(car_obb, ball_sphere, collision_normal, p)) {
//
        float carCenterX = center_x;
        float carCenterY = center_y;

        // Calculate the std::vector from the point of contact to the center of the ball
        vec2 contactPoint = nearestPointOnOBB(car_obb, ball_sphere.center);
//        vec2 normalizedContactVector = (vec2(carCenterX, carCenterY) - contactPoint).normalized(); //
        vec2 normalizedContactVector = (ball_sphere.center - contactPoint).normalized(); //

            vec2 ballVelocity = vec2(ball.velocityX, ball.velocityY);
            vec2 carVelocity = vec2(car.velocityX, car.velocityY);

            vec2 relativeVelocity = ballVelocity - carVelocity;

            float relativeSpeed = relativeVelocity.dot(normalizedContactVector);

        if (relativeSpeed < 0.0f) {

            /// BALL
            // calculate the collision impulse
            float j = -(1.0f + RESTITUTION) * relativeSpeed;
            j /= 1.0f / ball.mass + 1.0f / car.mass;

            if (abs(j) > 100) {
                playEffectOnce(ballHitSound, ballHitChannel);
            }
//            std::cerr << "j = " << j << "\n";
            // apply impulse
            vec2 impulse = normalizedContactVector*j;
            vec2 ballImpulse = impulse * (1.0f / ball.mass);
            vec2 carImpulse = impulse * (1.0f / car.mass);
            ball.velocityX += ballImpulse.x;
            ball.velocityY += ballImpulse.y;
            car.velocityX -= carImpulse.x;
            car.velocityY -= carImpulse.y;

            ball.omega = ballImpulse.x * 0.5;

            /// CAR - no impact
            // Apply the impulse to the car
//            vec2 carImpulse = ballImpulse*(-(1.0f / car.mass));
//            car.carBallCollisionVelocityX = carImpulse.x;
//            car.carBallCollisionVelocityY = carImpulse.y;
//            car.velocityX = carImpulse.x;
//            car.velocityY = carImpulse.y;
        }
//            vec2 separation = normalizedContactVector * (ball.radius - distance(contactPoint, vec2(carCenterX, carCenterY)));
//            vec2 newBallPosition = vec2(ball.xPos, ball.yPos) - separation;
//            ball.xPos = newBallPosition.x;
//            ball.yPos = newBallPosition.y;

        float dist = distance(vec2(ball_sphere.center), contactPoint);
        if (dist < ball.radius) {
            float delta = (ball.radius - dist);
            vec2 seperate = normalizedContactVector*delta;
            vec2 newPos = vec2(ball.xPos, ball.yPos) + seperate;
            ball.xPos = newPos.x;
            ball.yPos = newPos.y;
        }
    }
}
