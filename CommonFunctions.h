#ifndef COMMONFUNCTIONS_H_INCLUDED
#define COMMONFUNCTIONS_H_INCLUDED
#include <vector>
#include "Structs.h"
#include <array>
#include <SDL2/SDL.h>

extern std::vector<int> calculateColor(double power, double maxPower);

extern std::vector<Point> getCoords(SDL_FRect rect, float angle_degrees);

void drawLine(vec2 u, vec2 v, int r, int g, int b);

extern Point findParallelVector(const Point& a, const Point& b, const float V);

extern std::vector<Point> findPerpendicularVectors(const Point& a, const Point& b, const float V);

extern std::array<int, 3> boostColor(double value, int player);

extern float distance(float x1, float y1, float x2, float y2);

#endif // COMMONFUNCTIONS_H_INCLUDED
