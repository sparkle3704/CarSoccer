#include <vector>
#include "Structs.h"
#include <SDL2/SDL.h>
#include "SDL_Contexts.h"
#include "CommonFunctions.h"
#include <array>

std::vector<int> calculateColor(double power, double maxPower) {
    std::vector<int> color(3, 0); // Initialize color vector to 0

    // Calculate the RGB values based on the power value and maxPower
    double ratio = power / maxPower;
    if (ratio <= 0.5) {
        color[0] = 255;
        color[1] = static_cast<int>(ratio * 2 * 255);
    } else {
        color[0] = static_cast<int>((1 - ratio) * 2 * 255);
        color[1] = 255;
    }
    color[2] = 0;

    return color; // Return the color vector
}

std::vector<Point> getCoords(SDL_FRect rect, float angle_degrees) {
    float angle_radians = angle_degrees * M_PI / 180.0f;
    float cos_angle = cos(angle_radians);
    float sin_angle = sin(angle_radians);

    // Compute the center of the rectangle
    float cx = rect.x + rect.w / 2;
    float cy = rect.y + rect.h / 2;

    // Compute the coordinates of the corners of the unrotated rectangle
    float x1 = rect.x;
    float y1 = rect.y;
    float x2 = rect.x + rect.w;
    float y2 = rect.y;
    float x3 = rect.x + rect.w;
    float y3 = rect.y + rect.h;
    float x4 = rect.x;
    float y4 = rect.y + rect.h;

    // Translate the coordinates so that the center of the rectangle is at the origin
    x1 -= cx;
    y1 -= cy;
    x2 -= cx;
    y2 -= cy;
    x3 -= cx;
    y3 -= cy;
    x4 -= cx;
    y4 -= cy;

    // Rotate the coordinates of the corners
    float rotated_x1 = cos_angle * x1 - sin_angle * y1;
    float rotated_y1 = sin_angle * x1 + cos_angle * y1;
    float rotated_x2 = cos_angle * x2 - sin_angle * y2;
    float rotated_y2 = sin_angle * x2 + cos_angle * y2;
    float rotated_x3 = cos_angle * x3 - sin_angle * y3;
    float rotated_y3 = sin_angle * x3 + cos_angle * y3;
    float rotated_x4 = cos_angle * x4 - sin_angle * y4;
    float rotated_y4 = sin_angle * x4 + cos_angle * y4;

    // Translate the coordinates back to their original position
    rotated_x1 += cx;
    rotated_y1 += cy;
    rotated_x2 += cx;
    rotated_y2 += cy;
    rotated_x3 += cx;
    rotated_y3 += cy;
    rotated_x4 += cx;
    rotated_y4 += cy;

    // Pack the coordinates into a vector of Points
    std::vector<Point> coords = {
        {rotated_x1, rotated_y1},
        {rotated_x2, rotated_y2},
        {rotated_x3, rotated_y3},
        {rotated_x4, rotated_y4}
    };

    return coords;
}

//vec2 U, V;
//int RRRR, GGGG, BBBB;

void drawLine(vec2 u, vec2 v, int r, int g, int b) {
    SDL_SetRenderDrawColor(renderer.get(), r, g, b, 255);
    SDL_RenderDrawLine(renderer.get(), u.x, u.y, v.x, v.y);

//    U = u;
//    V = v;
//    RRRR = r;
//    GGGG = g;
//    BBBB = b;
}

Point findParallelVector(const Point& a, const Point& b, const float V) {
    // Calculate vector u going from point a to point b
    Point u = { b.x - a.x, b.y - a.y };

    // Calculate the magnitude of u
    float mag_u = std::sqrt(u.x * u.x + u.y * u.y);

    // Calculate the unit vector of u
    Point u_unit = { u.x / mag_u, u.y / mag_u };

    // Calculate the x and y components of v with magnitude V
    Point actual_v = { u_unit.x * V, u_unit.y * V };

    Point projected_v = { actual_v.x, actual_v.y };
    // Return the x and y components of projected_v
    return projected_v;
}

std::vector<Point> findPerpendicularVectors(const Point& a, const Point& b, const float V) {
    // Calculate vector u going from point a to point b
    Point u = { b.x - a.x, b.y - a.y };

    // Calculate the magnitude of u
    float mag_u = std::sqrt(u.x * u.x + u.y * u.y);

    // Calculate the unit vector of u
    Point u_unit = { u.x / mag_u, u.y / mag_u };

    // Calculate v1 and v2, which are perpendicular to u
    Point v1 = { u_unit.y, -u_unit.x };
    Point v2 = { -u_unit.y, u_unit.x };

    // Calculate the x and y components of v1 and v2 with magnitude V
    Point actual_v1 = { v1.x * V, v1.y * V };
    Point actual_v2 = { v2.x * V, v2.y * V };

    // Project actual_v1 and actual_v2 onto the x and y axes
    Point projected_v1 = { -actual_v1.x, -actual_v1.y };
    Point projected_v2 = { -actual_v2.x, -actual_v2.y };

    // Return the x and y components of projected_v1 and projected_v2
    std::vector<Point> projected = { {projected_v1.x, projected_v1.y}, {projected_v2.x, projected_v2.y} };
    return projected;
}

std::array<int, 3> boostColor(double value, int player) {
    double maxValue = 255; // set the maximum value
    int r, g, b;
    if (player == 2) {
        if (value <= maxValue / 10) {
            r = 255;
            g = 255;
            b = 255 * 0;
        } else if (value <= maxValue * 2 / 10) {
            r = 255 * (value - maxValue / 4) / (maxValue / 4);
            g = 255 * (value - maxValue / 4) / (maxValue / 4);
            b = 0 * 255 * (value - maxValue / 4) / (maxValue / 4);
        } else if (value <= maxValue * 4 / 10) {
            r = 255;
            g = 255 * (value - maxValue / 2) / (maxValue / 4);
            b = 0;
        } else {
            r = 255;
            g = 255 * (maxValue - value) / (maxValue / 4);
            b = 0;
        }
    }
    else {
        if (value <= maxValue / 10) {
            r = 255 * 0;
            g = 255;
            b = 255;
        } else if (value <= maxValue * 2 / 10) {
            r = 255 * 0 * (value - maxValue / 4) / (maxValue / 4);
            g = 255 * (value - maxValue / 4) / (maxValue / 4);
            b = 255 * (value - maxValue / 4) / (maxValue / 4);
        } else if (value <= maxValue * 4 / 10) {
            r = 0;
            g = 255 * (value - maxValue / 2) / (maxValue / 4);
            b = 255;
        } else {
            r = 0;
            g = 255 * (maxValue - value) / (maxValue / 4);
            b = 255;
        }
    }
    return {r, g, b};
}

float distance(float x1, float y1, float x2, float y2) {
    return sqrtf((float)((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1)));
}

