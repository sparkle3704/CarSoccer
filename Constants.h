#ifndef CONSTANTS_H_INCLUDED
#define CONSTANTS_H_INCLUDED
#pragma once
#include "Structs.h"

extern const int WINDOW_WIDTH;
extern const int WINDOW_HEIGHT;
extern const float RESTITUTION;

extern const vec2 topLeft1;
extern const vec2 topRight1;
extern const vec2 botLeft1;
extern const vec2 botRight1;
extern const vec2 ceilBackGoal1;
extern const vec2 floorBackGoal1;

extern const vec2 topLeft2;
extern const vec2 topRight2;
extern const vec2 botLeft2;
extern const vec2 botRight2;
extern const vec2 ceilBackGoal2;
extern const vec2 floorBackGoal2;

/// car
extern const float FRICTION;
extern const float VELOCITY_X_FACTOR;
extern const float ROTATE_FACTOR;
extern const float deltaTime;

extern const float GRAVITY_ACCELERATION;

extern const float JUMP_VELOCITY;
extern const float JUMP_DRAG_ACCELERATION;

extern const float MAX_BOOST_VELOCITY;
extern const float MAX_BOOST_LENGTH;
extern const float BOOST_ACCELERATION;

extern const float MAX_HEAD_TRAIL_LENGTH;

extern const float DODGE_SPIN_FACTOR;
extern const float DODGE_VELOCITY;

extern float groundY;
/// ball

#endif // CONSTANTS_H_INCLUDED
