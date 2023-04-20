#include "Constants.h"

const int WINDOW_WIDTH = 1920;
const int WINDOW_HEIGHT = 1080;
const float RESTITUTION = 0.5;

//vec2 topLeft1, topRight1, botLeft1, botRight1, floorBackGoal1, ceilBackGoal1;
//vec2 topLeft2, topRight2, botLeft2, botRight2, floorBackGoal2, ceilBackGoal2;
//vec2 topRight1 = vec2(120, 238);
//vec2 botRight1 = vec2(120, 470);
//vec2 topLeft1 = vec2(24, 216);
//vec2 botLeft1 = vec2(24, 490);
//vec2 floorBackGoal1 = vec2(-3000, 490);
//vec2 ceilBackGoal1 = vec2(-3000, 216);
//
//vec2 topRight2 = vec2(1256, 216);
//vec2 botRight2 = vec2(1256, 490);
//vec2 topLeft2 = vec2(1160, 238);
//vec2 botLeft2 = vec2(1160, 470);
//vec2 floorBackGoal2 = vec2(3000, 490);
//vec2 ceilBackGoal2 = vec2(3000, 216);

const vec2 topLeft1 = vec2(46, 335);
const vec2 topRight1 = vec2(173, 365);
const vec2 botLeft1 = vec2(46, 728);
const vec2 botRight1 = vec2(174, 701);
const vec2 ceilBackGoal1 = vec2(-3000, 335);
const vec2 floorBackGoal1 = vec2(-3000, 728);

const vec2 topLeft2 = vec2(1746, 365);
const vec2 topRight2 = vec2(1873, 335);
const vec2 botLeft2 = vec2(1745, 701);
const vec2 botRight2 = vec2(1873, 728);
const vec2 ceilBackGoal2 = vec2(3000, 335);
const vec2 floorBackGoal2 = vec2(3000, 728);

/// car
const float FRICTION = 0.1f;
const float VELOCITY_X_FACTOR = 4;
const float ROTATE_FACTOR = 2; // 3
const float deltaTime = 0.025;

const float GRAVITY_ACCELERATION = 5.0f; // 5.0f;

const float JUMP_VELOCITY = 7;
const float JUMP_DRAG_ACCELERATION = 100;


const float MAX_BOOST_VELOCITY = 8;
const float MAX_BOOST_LENGTH = 60;
const float BOOST_ACCELERATION = 6;

const float MAX_HEAD_TRAIL_LENGTH = 60;

const float DODGE_SPIN_FACTOR = 7.5; // 9 // 7.89
const float DODGE_VELOCITY = 8.6; // 7

float groundY = WINDOW_HEIGHT*29/30;
