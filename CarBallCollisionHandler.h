#ifndef CARBALLCOLLISIONHANDLER_H_INCLUDED
#define CARBALLCOLLISIONHANDLER_H_INCLUDED

#include "Structs.h"
#include "Car.h"
#include "Ball.h"
#include <algorithm>

extern vec2 rotateVec2(const vec2& v, float angle);

extern vec2 nearestPointOnOBB(const obb a, const vec2 b);

extern bool intersectCarBall(const obb a, const sphere b, vec2& collision_normal, vec2& p);

extern obb carToObb(Car car);

extern sphere ballToSphere(Ball ball);

extern float distance(vec2 a, vec2 b);

extern void handleCollisionCarBall(Car& car, Ball& ball);

#endif // CARBALLCOLLISIONHANDLER_H_INCLUDED
