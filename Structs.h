#ifndef STRUCTS_H_INCLUDED
#define STRUCTS_H_INCLUDED

#include <math.h>

struct Particle {
    float x, y;     // position
    float vx, vy;   // velocity
    float size;     // size
    float alpha;    // alpha value (transparency)
    float dragX;
    float dragY;

    Particle(float x, float y, float vx, float vy, float size, float alpha, float dragX, float dragY)
        : x(x), y(y), vx(vx), vy(vy), size(size), alpha(alpha), dragX(dragX), dragY(dragY)
    {}
};

struct Point {
    float x, y;
};

struct vec2 {
    float x;
    float y;

    vec2(float x = 0, float y = 0) : x(x), y(y) {}
    vec2 operator+(const vec2 &v) const {
        return {x + v.x, y + v.y};
    }

    vec2 operator-(const vec2 &v) const {
        return {x - v.x, y - v.y};
    }

    vec2 operator*(const float s) const {
        return {x * s, y * s};
    }

    float dot(const vec2 &v) const {
        return x * v.x + y * v.y;
    }

    float length() const {
        return sqrtf(dot(*this));
    }

    vec2 normalized() const {
        if (length() != 0) {
            return *this * (1.0f / length());
        }
        return {0.0f, 0.0f};
    }

    vec2 perpendicular() const {
        return {-y, x};
    }
};

struct sphere {
    vec2 center;
    float radius;
};

struct obb {
    vec2 center;
    float halfWidth;
    float halfHeight;
    float angle;

    obb(vec2 center, float halfWidth, float halfHeight, float angle) : center(center), halfWidth(halfWidth), halfHeight(halfHeight), angle(angle) {}
};




#endif // STRUCTS_H_INCLUDED
