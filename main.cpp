#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_Events.h>
#include <SDL2/SDL_image.h>
#include <math.h>
#include <vector>
#include <stdlib.h>
#include <windows.h>
#include <cmath>
#include <SDL2/SDL_ttf.h>

#include <bits/stdc++.h>
using namespace std;

float player2_curSign, player1_curSign, player2_prvSign = -1, player1_prvSign = -1;
const int WINDOW_WIDTH = 1920;
const int WINDOW_HEIGHT = 1080;
bool fullScreen = 0;

float groundY = WINDOW_HEIGHT*29/30;
SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
SDL_Surface* screenSurface = NULL;
int prvScoreA = -1;
int prvScoreB = -1;
int scoreA = 0;
int scoreB = 0;

void PrintPoint(SDL_FPoint& point) {
//    std::cerr << "-----------" << " " << point.x << " " << point.y << "\n";
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDrawPoint(renderer, point.x, point.y);

    SDL_Texture* tmpTexture = NULL;
    tmpTexture = IMG_LoadTexture(renderer, "D:/gameProject/game/assets/66598-ball.png");
    SDL_FRect tmpRect = {point.x - 12 / 2, point.y - 12 / 2, 12, 12};
    SDL_FPoint tmpCenter = {12, 12};
    SDL_RenderCopyExF(renderer, tmpTexture, NULL, &tmpRect, 0, &tmpCenter, SDL_FLIP_NONE);
    SDL_DestroyTexture(tmpTexture);
}

float maxSpeed = 0;
void ClearScreen()
{
COORD cursorPosition;	cursorPosition.X = 0;	cursorPosition.Y = 0;	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), cursorPosition);
}

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
    clamped.x = max(clamped.x, -a.halfWidth);
    clamped.x = min(clamped.x, a.halfWidth);
    clamped.y = max(clamped.y, -a.halfHeight);
    clamped.y = min(clamped.y, a.halfHeight);

    vec2 unrotated(clamped.x * c + clamped.y * s, -clamped.x * s + clamped.y * c);

    return a.center + unrotated;
}


vec2 U, V;
int RRRR, GGGG, BBBB;
bool prvDir = 0;

void drawLine(vec2 u, vec2 v, int r, int g, int b) {
    SDL_SetRenderDrawColor(renderer, r, g, b, 255);
    SDL_RenderDrawLine(renderer, u.x, u.y, v.x, v.y);

    U = u;
    V = v;
    RRRR = r;
    GGGG = g;
    BBBB = b;
}



bool initEverything() {
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0){
		std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
		return 0;
	}
    window = SDL_CreateWindow("Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    if (window == nullptr){
        std::cerr<< "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 0;
    }
    // SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (renderer == nullptr) {
        SDL_DestroyWindow(window);
        std::cerr << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 0;
    }
    if (TTF_Init() != 0) {
        std::cerr << "TTF_Init Error : " << SDL_GetError() << "\n";
        return 0;
    }
    return 1;
}

SDL_Surface* load_and_optimize_surface(std::string path) {
    SDL_Surface* optimizedSurface = NULL;
    SDL_Surface* loadedSurface = IMG_Load(path.c_str());

    if (loadedSurface == NULL) {
        std::cerr << "Unable to load image: " << path << ", SDL_image error: " << IMG_GetError() << "\n";
    }
    else {
        optimizedSurface = SDL_ConvertSurface(loadedSurface, screenSurface->format, 0);
        if (optimizedSurface == NULL) {
            std::cerr << "Unable to optimize image " << path << ", error: " << SDL_GetError() << "\n";
        }
        SDL_FreeSurface(loadedSurface);
    }
    return optimizedSurface;
}

SDL_Surface* mediaSurface(std::string path) {
    SDL_Surface* surface = SDL_LoadBMP(path.c_str());
    if (surface == NULL) {
        std::cerr << "Unable to load image: " << path << ", error: " << SDL_GetError() << "\n";
    }
    return surface;
}

SDL_Texture* loadTexture(const char* path) {
    SDL_Texture* newTexture = NULL;
    SDL_Surface* loadedSurface = IMG_Load(path);
    if (loadedSurface == NULL) {
        std::cerr << "Unable to load image: " << path << ", error: " << SDL_GetError() << "\n";
    }
    else {
        SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "2");
        newTexture = SDL_CreateTextureFromSurface(renderer, loadedSurface);
        if (newTexture == NULL) {
            std::cerr << "Unable to create texture from " << path << ", error: " << SDL_GetError() << "\n";
        }
        SDL_FreeSurface(loadedSurface);
    }
    return newTexture;
}

void showSurface(SDL_Surface* surface) {
    if (surface == NULL) {
        std::cerr << "Failed to show surface" << "\n";
    }
    else {
        SDL_BlitSurface(surface, NULL, screenSurface, NULL);
        SDL_UpdateWindowSurface(window);
    }
}




std::vector<int> calculateColor(double power, double maxPower) {
//    power = min(power, maxPower);
//    power = maxPower - power;
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


bool quit = false;
//SDL_Texture* carTexture = NULL;
SDL_Texture* ballTexture = NULL;
SDL_Rect carRect;

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

struct Point {
    float x, y;
};

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


float getAngle(const std::vector<Point>& points) {
    // Compute the center of the rectangle
    float cx = 0.0f;
    float cy = 0.0f;
    for (const Point& p : points) {
        cx += p.x;
        cy += p.y;
    }
    cx /= 4.0f;
    cy /= 4.0f;

    // Compute the angle of rotation
    float dx = points[1].x - points[0].x;
    float dy = points[1].y - points[0].y;
    float angle_radians = atan2(dy, dx);
    float angle_degrees = angle_radians * 180.0f / M_PI;


    if (!(angle_degrees >= 0 && angle_degrees <= 359)) {
        if (angle_degrees >= 0) {
            angle_degrees = fmod(angle_degrees, 360);
        }
        else {
            angle_degrees = 360 - fmod(-angle_degrees, 360);
        }
        angle_degrees = fmod(angle_degrees, 360);
    }

    return angle_degrees;
}
SDL_Texture* boostTexture = NULL;
SDL_Texture* explosionTexture = NULL;

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

std::random_device rd;
std::mt19937 gen(rd());

vector<Particle> boostParticles[3];
vector<Particle> explosionParticles[3];

array<int, 3> boostColor(double value, int player) {
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

void addBoost(int numParticles, Point a, Point b, Point c, Point d, int player) {

//    std::uniform_real_distribution<float> v_gen(1, 1);
//    std::uniform_real_distribution<float> size_gen(1, 20);
//    std::uniform_real_distribution<float> alpha_gen(1, 1);
//    std::uniform_real_distribution<float> x_gen(min(a.x, b.x), max(a.x, b.x));
//    std::uniform_real_distribution<float> y_gen(min(a.y, b.y), max(a.y, b.y));

//    std::uniform_real_distribution<float> v_gen(1, 1);
    std::uniform_real_distribution<float> size_gen(2, 15);
    std::uniform_real_distribution<float> alpha_gen(256, 256);
    std::uniform_real_distribution<float> x_gen(min(a.x, b.x), max(a.x, b.x));
    std::uniform_real_distribution<float> y_gen(min(a.y, b.y), max(a.y, b.y));
//    std::uniform_real_distribution<float> v_gen(5, 15);
    std::uniform_real_distribution<float> v_gen(1, 20);

    vec2 horvec = vec2({c.x - d.x, c.y - d.y}).normalized();
    for (int i = 1; i <= numParticles; ++i) {
//        float x = x_gen(gen);
//        float y = y_gen(gen);
//        float vx = vx_gen(gen);
//        float vy = vy_gen(gen);

        float v = v_gen(gen);
        vec2 vxy = horvec*v;

        float x = x_gen(gen);
        float y = y_gen(gen);
        float vx = vxy.x;
        float vy = vxy.y;

        float size = size_gen(gen);
        float alpha = alpha_gen(gen);
        boostParticles[player].push_back(Particle(x, y, vx, vy, size, alpha, 0, 0));
    }
    boostTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 300, 300);
    SDL_DestroyTexture(boostTexture);
}

void updateBoost(int player) {
    for (auto& particle: boostParticles[player]) {
        particle.x += particle.vx;
        particle.y += particle.vy;
        particle.alpha -= 20.0f;
    }
//    SDL_DestroyTexture(boostTexture);
    boostParticles[player].erase(remove_if(boostParticles[player].begin(), boostParticles[player].end(), [](const Particle& p) {
        return p.alpha <= 0.0f;
    }), boostParticles[player].end());
}


void renderBoost(int player) {
    // Set blending mode for boostParticles
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    // Set texture as render target
    SDL_SetRenderTarget(renderer, boostTexture);

    // Clear texture with transparent color
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);

    for (auto& particle: boostParticles[player]) {
        SDL_FRect boostRect = {particle.x - particle.size/2.0f, particle.y - particle.size/2.0f, particle.size, particle.size};
        array<int, 3> rgb = boostColor(particle.alpha, player);
        SDL_SetRenderDrawColor(renderer, rgb[0], rgb[1], rgb[2], 255);
        SDL_RenderFillRectF(renderer, &boostRect);
//        SDL_DestroyTexture(boostTexture);
    }
    // Reset render target to default
    SDL_SetRenderTarget(renderer, nullptr);

    // Draw texture to screen
    SDL_RenderCopy(renderer, boostTexture, nullptr, nullptr);

}

std::random_device rd2;
std::mt19937 gen2(rd2());

void addExplosion(int numParticles, float startX, float startY, int player) {
    std::uniform_real_distribution<float> size_gen(1, 15);
    std::uniform_real_distribution<float> alpha_gen(0, 256);
    std::uniform_real_distribution<float> angle_gen(0, 2 * M_PI);
    std::uniform_real_distribution<float> speed_gen(1, 30);
    for (int i = 1; i <= numParticles; ++i) {
        float particleAngle = angle_gen(gen2);
        float speed = speed_gen(gen2);
        float vx = speed * cos(particleAngle);
        float vy = speed * sin(particleAngle);
        float size = size_gen(gen2);
        float alpha = alpha_gen(gen2);
        float dragX = 1;
        if (vx < 0) {
            dragX *= -1;
        }
        float dragY = 0.99;
        if (vy < 0) {
            dragY *= -1;
        }
        explosionParticles[player].push_back(Particle(startX, startY, vx, vy, size, alpha, dragX, dragY));
    }
    explosionTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 300, 300);
    SDL_DestroyTexture(explosionTexture);
}

void updateExplosion(int player) {
//    float deltaTime = 0.0001;
//    float deltaChange = 90;
//    float smallVx = 0.0001;
//    float smallVy = 0.0001;

//    float deltaTime = 0.2;
//    float deltaChange = 0.01;
//    float smallVx = 4;
//    float smallVy = 4;

    float deltaTime = 0.2;
    float deltaChange = 0.01;
    float smallVx = 2;
    float smallVy = 2;
    for (auto& particle: explosionParticles[player]) {
        particle.x += particle.vx - particle.dragX * deltaTime;
        particle.y += particle.vy - particle.dragY * deltaTime;
        particle.alpha -= 4*deltaTime;

        if (abs(particle.vx) > smallVx) {
            if (abs(particle.dragX) * deltaTime > abs(particle.vx)) {
                particle.vx = 0;
                particle.dragX = 0;
            }
            else {
                particle.vx -= particle.dragX * deltaTime;
            }
            if (particle.vx < 0) {
                particle.dragX -= deltaChange;
            }
            else {
                particle.dragX += deltaChange;
            }
        }
        else {
            if (particle.vx != 0) {
//                particle.vx *= -1;
                particle.vx = (particle.vx/abs(particle.vx)) * -1 * (abs(particle.vx) + 0.1*abs(particle.dragX));
            }
        }
        if (abs(particle.vy) > smallVy) {
            if (abs(particle.dragY) * deltaTime > abs(particle.vy)) {
                particle.vy = 0;
                particle.dragY = 0;
            }
            else {
                particle.vy -= particle.dragY * deltaTime;
            }
            if (particle.vy < 0) {
                particle.dragY -= deltaChange;
            }
            else {
                particle.dragY += deltaChange;
            }
        }
        else {

            if (particle.vy != 0) {
//                particle.vy *= -1;
                particle.vy = (particle.vy/abs(particle.vy)) * -1 * (abs(particle.vy) + 0.1*abs(particle.dragY));
            }
        }
    }
    explosionParticles[player].erase(remove_if(explosionParticles[player].begin(), explosionParticles[player].end(), [](const Particle& p) {
        return p.alpha <= 0.0f;
    }), explosionParticles[player].end());
}

void renderExplosion(int player) {
    // Set blending mode for boostParticles
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    // Set texture as render target
    SDL_SetRenderTarget(renderer, explosionTexture);

    // Clear texture with transparent color
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);

    for (auto& particle: explosionParticles[player]) {
        SDL_FRect boostRect = {particle.x - particle.size/2.0f, particle.y - particle.size/2.0f, particle.size, particle.size};
        array<int, 3> rgb = boostColor(particle.alpha, player);
        SDL_SetRenderDrawColor(renderer, rgb[0], rgb[1], rgb[2], 255);
        SDL_RenderFillRectF(renderer, &boostRect);
//        SDL_DestroyTexture(explosionTexture);
    }
    // Reset render target to default
    SDL_SetRenderTarget(renderer, nullptr);

    // Draw texture to screen
    SDL_RenderCopy(renderer, explosionTexture, nullptr, nullptr);

}



class Car {
public:
    float mass = 200;
    float velocityX;
    float velocityY;
    float accelerationX;
    float accelerationY;
    float xPos;
    float yPos;
    bool onGround;
    bool jumping;
    float angle;
    bool dir;
    SDL_RendererFlip flip;
    bool clockWise;
    bool pointing;
    float width;
    float height;
    bool canJump = 1;
    float goingVelocityX = 0;
    float gravityVelocityY;
    float jumpVelocityX = 0;
    float jumpVelocityY = 0;
    float initialJumpDragAccelerationX = 0;
    float initialJumpDragAccelerationY = 0;
    float initialGravityAccelerationY = 0;
    float boostVelocityX = 0;
    float boostVelocityY = 0;
    float boostVelocity = 0;
    float initialBoostAccelerationX = 0;
    float initialBoostAccelerationY = 0;
    bool spinningClockWise = 0;
    float spinnedClockWise = 0;
    bool spinningCounterClockWise = 0;
    float spinnedCounterClockWise = 0;
    float dodgeVelocityX = 0;
    float dodgeVelocityY = 0;
    bool dodgingDown = 0;
    bool dodgingUp = 0;

    Point parallelBeforeDodgeA;
    Point parallelBeforeDodgeB;

    float carBallCollisionVelocityX = 0;
    float carBallCollisionVelocityY = 0;
    float carBallCollisionDragAccelerationX = 0;
    float carBallCollisionDragAccelerationY = 0;
    SDL_Texture* carTexture = NULL;
    float prvSign = -1, curSign;
    bool prvDir = 0;
    bool curDir;
    int player;

    float dodgeDragAccelerationX = 0;
    float dodgeDragAccelerationY = 0;

//    vector<vector<Point>> corner{2, vector<Point>(2)};

    Car(float mass, float velocityX, float velocityY, float accelerationX, float accelerationY, float xPos, float yPos, bool onGround, bool jumping, float angle, bool dir, SDL_RendererFlip flip, bool clockWise, bool pointing, float width, SDL_Texture* carTexture, int player) :
        mass(mass), velocityX(velocityX), velocityY(velocityY), accelerationX(accelerationX), accelerationY(accelerationY), xPos(xPos), yPos(yPos), onGround(onGround), jumping(jumping), angle(angle), dir(dir), flip(flip), clockWise(clockWise), pointing(pointing), width(width), carTexture(carTexture), player(player) {}


    bool hasCollision() {
        SDL_FRect carRect = {xPos, yPos, width, height};
        vector<Point> tmp = getCoords(carRect, angle);
        float x1 = tmp[0].x, y1 = tmp[0].y;
        float x2 = tmp[1].x, y2 = tmp[1].y;
        float x3 = tmp[2].x, y3 = tmp[2].y;
        float x4 = tmp[3].x, y4 = tmp[3].y;

        float minX = min({x1, x2, x3, x4});
        float minY = min({y1, y2, y3, y4});
        float maxX = max({x1, x2, x3, x4});
        float maxY = max({y1, y2, y3, y4});

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

    bool hasCollisionWithGround() {
        SDL_FRect carRect = {xPos, yPos, width, height};
        vector<Point> tmp = getCoords(carRect, angle);
        float x1 = tmp[0].x, y1 = tmp[0].y;
        float x2 = tmp[1].x, y2 = tmp[1].y;
        float x3 = tmp[2].x, y3 = tmp[2].y;
        float x4 = tmp[3].x, y4 = tmp[3].y;

        float minX = min({x1, x2, x3, x4});
        float minY = min({y1, y2, y3, y4});
        float maxX = max({x1, x2, x3, x4});
        float maxY = max({y1, y2, y3, y4});

//        std::cerr << maxX << ", " << "WINDOW_WIDTH = " << WINDOW_WIDTH << "\n";
        if (maxY >= groundY) {
//            std::cerr << "Hit ground" << "\n";
            return 1;
        }
        return 0;
    }
    void correctAngle() {

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
//            dir = 1;
            pointing = 0;
//            clockWise = 1;
        }
        else {
//            flip = SDL_FLIP_VERTICAL;
//            dir = 0;
//            clockWise = 0;
            pointing = 1;
        }
        if (angle == 90) {
            if (dir == 0) {
                pointing = 0;
            }
            else {
                pointing = 1;
            }
        }
        if (angle == 270) {
            if (dir == 0) {
                pointing = 0;
            }
            else {
                pointing = 1;
            }
        }
        if (pointing == 1) {
            if (dir == 1) {
                flip = SDL_FLIP_VERTICAL;
                clockWise = 1;
            }
        }
        else {
            if (dir == 0) {
                flip = SDL_FLIP_NONE;
                clockWise = 0;
            }
        }
    }

    float binS(float sign) { // find biggest so that angle +- ans errorless
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
    void tiltDown() {
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

    void tiltUp() {
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

    void verticalFlip() {
        if (angle <= 180) {
            angle = 90 - angle + 90;
        }
        else {
            angle = 270 - angle + 270;
        }
    }

    void moveRight() {
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

    void moveLeft() {
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

    void correctPosition() {
        SDL_FRect carRect = {xPos, yPos, width, height};
        vector<Point> tmp = getCoords(carRect, angle);
        float x1 = tmp[0].x, y1 = tmp[0].y;
        float x2 = tmp[1].x, y2 = tmp[1].y;
        float x3 = tmp[2].x, y3 = tmp[2].y;
        float x4 = tmp[3].x, y4 = tmp[3].y;

        float minX = min({x1, x2, x3, x4});
        float minY = min({y1, y2, y3, y4});
        float maxX = max({x1, x2, x3, x4});
        float maxY = max({y1, y2, y3, y4});

        if (maxY > groundY) {
            float delta = maxY - groundY;
            y1 -= delta;
            y2 -= delta;
            y3 -= delta;
            y4 -= delta;
            yPos -= delta;
            minX = min({x1, x2, x3, x4});
            minY = min({y1, y2, y3, y4});
            maxX = max({x1, x2, x3, x4});
            maxY = max({y1, y2, y3, y4});
        }

        if (minY < 0) {
            float delta = -minY;
            y1 += delta;
            y2 += delta;
            y3 += delta;
            y4 += delta;
            yPos += delta;
            minX = min({x1, x2, x3, x4});
            minY = min({y1, y2, y3, y4});
            maxX = max({x1, x2, x3, x4});
            maxY = max({y1, y2, y3, y4});
        }

        if (maxX > WINDOW_WIDTH) {
            float delta = maxX - WINDOW_WIDTH;
            x1 -= delta;
            x2 -= delta;
            x3 -= delta;
            x4 -= delta;
            xPos -= delta;
            minX = min({x1, x2, x3, x4});
            minY = min({y1, y2, y3, y4});
            maxX = max({x1, x2, x3, x4});
            maxY = max({y1, y2, y3, y4});
        }

        if (minX < 0) {
            float delta = -minX;
            x1 += delta;
            x2 += delta;
            x3 += delta;
            x4 += delta;
            xPos += delta;
            minX = min({x1, x2, x3, x4});
            minY = min({y1, y2, y3, y4});
            maxX = max({x1, x2, x3, x4});
            maxY = max({y1, y2, y3, y4});
        }
    }

    void applyGravity() {
        if (onGround == 0) {
            initialGravityAccelerationY = GRAVITY_ACCELERATION;
//            std::cerr << "not on ground anymore, aY = gravity" << "\n";
//            ;
        }
    }


    void boost(float sign) {

        SDL_FRect carRect = {xPos, yPos, width, height};
        vector<Point> tmp = getCoords(carRect, angle);
        float x1 = tmp[0].x, y1 = tmp[0].y;
        float x2 = tmp[1].x, y2 = tmp[1].y;
        float x3 = tmp[2].x, y3 = tmp[2].y;
        float x4 = tmp[3].x, y4 = tmp[3].y;

        float minX = min({x1, x2, x3, x4});
        float minY = min({y1, y2, y3, y4});
        float maxX = max({x1, x2, x3, x4});
        float maxY = max({y1, y2, y3, y4});

        if (sign == 1) {
            if (prvSign == -1) {

                if (velocityY == 0) {
                    boostVelocity = abs(velocityX);
                }
                else {
//                    boostVelocity = 0;
                }
//                boostVelocity = abs(goingVelocityX);
//                sqrt((velocityX + accelerationX*deltaTime)*(velocityX + accelerationX*deltaTime) + (velocityY + accelerationY*deltaTime)*(velocityY + accelerationY*deltaTime))
//                boostVelocity = sqrt((velocityX + accelerationX*deltaTime)*(velocityX + accelerationX*deltaTime) + (velocityY + accelerationY*deltaTime)*(velocityY + accelerationY*deltaTime)) - sign*BOOST_ACCELERATION*deltaTime*2;
                gravityVelocityY = GRAVITY_ACCELERATION/5;
            }
            else {
                goingVelocityX = 0;
                gravityVelocityY = GRAVITY_ACCELERATION/5;
            }
        }
        else {
            if (prvSign == 1) {
                if (velocityY == 0) {
                    if (goingVelocityX != 0) {
                        boostVelocity -= sign*BOOST_ACCELERATION * deltaTime;
                    }
                }
            }
        }
        if (dir != prvDir) {
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
        boostVelocity = min(boostVelocity, MAX_BOOST_VELOCITY);
        boostVelocity = max(boostVelocity, 0.0f);

        Point projected = findParallelVector(tmp[1], tmp[0], boostVelocity);

        if (spinningClockWise || spinningCounterClockWise) {
            projected = findParallelVector(parallelBeforeDodgeA, parallelBeforeDodgeB, boostVelocity);
        }

        boostVelocityX = projected.x;
        boostVelocityY = projected.y;
    }

    void moveCar() {
        SDL_FRect carRect = {xPos, yPos, width, height};
        vector<Point> tmp = getCoords(carRect, angle);
        float x1 = tmp[0].x, y1 = tmp[0].y;
        float x2 = tmp[1].x, y2 = tmp[1].y;
        float x3 = tmp[2].x, y3 = tmp[2].y;
        float x4 = tmp[3].x, y4 = tmp[3].y;

        float minX = min({x1, x2, x3, x4});
        float minY = min({y1, y2, y3, y4});
        float maxX = max({x1, x2, x3, x4});
        float maxY = max({y1, y2, y3, y4});


//        vector<Point> projected = findPerpendicularVectors(tmp[1], tmp[0], JUMP_VELOCITY);
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
            maxY = max({y1, y2, y3, y4});
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
//        boostVelocity = min(boostVelocity, MAX_BOOST_VELOCITY);
//        boostVelocity = max(boostVelocity, 0.0f);

        /// ///////////////////////////////////////////////////////
        velocityX = goingVelocityX + jumpVelocityX + boostVelocityX + dodgeVelocityX + carBallCollisionVelocityX;
        velocityY =  gravityVelocityY + jumpVelocityY + boostVelocityY + dodgeVelocityY + carBallCollisionVelocityY;

        accelerationX = initialJumpDragAccelerationX + carBallCollisionDragAccelerationX + dodgeDragAccelerationX;
        accelerationY = initialGravityAccelerationY + initialJumpDragAccelerationY + carBallCollisionDragAccelerationY + dodgeDragAccelerationY;

        xPos += (velocityX + accelerationX * deltaTime);
        yPos += (velocityY + accelerationY * deltaTime);
//
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
            if (dir != prvDir) {
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

        minX = min({x1, x2, x3, x4});
        minY = min({y1, y2, y3, y4});
        maxX = max({x1, x2, x3, x4});
        maxY = max({y1, y2, y3, y4});

        if (maxY >= groundY) {
            float delta = maxY - groundY;
            y1 -= delta;
            y2 -= delta;
            y3 -= delta;
            y4 -= delta;
            yPos -= delta;
            maxY = max({y1, y2, y3, y4});
        }
    }


    void dodgeDown() {
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
        vector<Point> tmp = getCoords(carRect, angle);
        float x1 = tmp[0].x, y1 = tmp[0].y;
        float x2 = tmp[1].x, y2 = tmp[1].y;
        float x3 = tmp[2].x, y3 = tmp[2].y;
        float x4 = tmp[3].x, y4 = tmp[3].y;

        float minX = min({x1, x2, x3, x4});
        float minY = min({y1, y2, y3, y4});
        float maxX = max({x1, x2, x3, x4});
        float maxY = max({y1, y2, y3, y4});

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

    void dodgeUp() {

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
        vector<Point> tmp = getCoords(carRect, angle);
        float x1 = tmp[0].x, y1 = tmp[0].y;
        float x2 = tmp[1].x, y2 = tmp[1].y;
        float x3 = tmp[2].x, y3 = tmp[2].y;
        float x4 = tmp[3].x, y4 = tmp[3].y;

        float minX = min({x1, x2, x3, x4});
        float minY = min({y1, y2, y3, y4});
        float maxX = max({x1, x2, x3, x4});
        float maxY = max({y1, y2, y3, y4});
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

    void initSize() {
        int w, h;
        SDL_QueryTexture(carTexture, NULL, NULL, &w, &h);
        float scale = width / float(w);
        height = float(h) * scale;
    }

    deque<SDL_FPoint> headBuffer;


    void draw(SDL_Renderer* renderer) {
        maxSpeed = max(maxSpeed, sqrt((velocityX*velocityX) + (velocityY*velocityY)));
        correctAngle();
        correctPosition();

        SDL_FRect carRect = {xPos, yPos, width, height};
        vector<Point> tmp = getCoords(carRect, angle);
        float x1 = tmp[0].x, y1 = tmp[0].y;
        float x2 = tmp[1].x, y2 = tmp[1].y;
        float x3 = tmp[2].x, y3 = tmp[2].y;
        float x4 = tmp[3].x, y4 = tmp[3].y;

        float minX = min({x1, x2, x3, x4});
        float minY = min({y1, y2, y3, y4});
        float maxX = max({x1, x2, x3, x4});
        float maxY = max({y1, y2, y3, y4});

        vector<Point> projected = findPerpendicularVectors(tmp[1], tmp[0], JUMP_VELOCITY);

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
        vector<Point> perpen = findPerpendicularVectors(tmp[1], tmp[0], JUMP_VELOCITY);
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
//        std::cerr << (dir ? "going right" : "going left") << " ";
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
//        std::cerr << "dir = " << dir << ", " << (dir ? "going right" : "going left  ") << "\n";
//        std::cerr << "pointing = " << pointing << ", " << (pointing ? "pointing right" : "pointing left  ") << "\n";
//        std::cerr << (flip == SDL_FLIP_VERTICAL ? "vertical flipped" : "                      ") << "\n";
//        std::cerr << "\n";
        SDL_RenderCopyExF(renderer, carTexture, NULL, &carRect, angle, &center, flip);

        for (int i = 1; i < (int)headBuffer.size(); ++i) {
            float maxSpeed = 14.2;
            float curSpeed = sqrt((velocityX*velocityX) + (velocityY*velocityY));
            vector<int> getRGB = calculateColor(curSpeed, maxSpeed);
            SDL_SetRenderDrawColor(renderer, getRGB[0], getRGB[1], getRGB[2], 255);
            SDL_RenderDrawLine(renderer, headBuffer[i-1].x, headBuffer[i-1].y, headBuffer[i].x, headBuffer[i].y);
//            SDL_RenderCopyExF(renderer, carTexture, NULL, &carRect, angle, &center, flip);
//            SDL_RenderPresent(renderer);
        }
        updateBoost(player);
        renderBoost(player);

        SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
        SDL_RenderDrawLineF(renderer, tmp[1].x, tmp[1].y, tmp[0].x, tmp[0].y);

//        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
//        SDL_RenderDrawPointF(renderer, tmp[0].x, tmp[0].y);

        SDL_Texture* tmpTexture = NULL;
        tmpTexture = IMG_LoadTexture(renderer, "D:/gameProject/game/assets/66598-ball.png");
        SDL_FRect tmpRect = {tmp[0].x - 12/2, tmp[0].y - 12/2, 12, 12};
        SDL_FPoint tmpCenter = {12, 12};
        SDL_RenderCopyExF(renderer, tmpTexture, NULL, &tmpRect, 0, &tmpCenter, SDL_FLIP_NONE);
        SDL_DestroyTexture(tmpTexture);
//        tmpTexture = NULL;
        drawLine(U, V, RRRR, GGGG, BBBB);
    }

    void jump() {
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
            vector<Point> tmp = getCoords(carRect, angle);
            float x1 = tmp[0].x, y1 = tmp[0].y;
            float x2 = tmp[1].x, y2 = tmp[1].y;
            float x3 = tmp[2].x, y3 = tmp[2].y;
            float x4 = tmp[3].x, y4 = tmp[3].y;

            float minX = min({x1, x2, x3, x4});
            float minY = min({y1, y2, y3, y4});
            float maxX = max({x1, x2, x3, x4});
            float maxY = max({y1, y2, y3, y4});

            vector<Point> projected = findPerpendicularVectors(tmp[1], tmp[0], JUMP_VELOCITY);
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

    void setDir(bool newDir) {
//        dir = newDir;
        if (newDir != dir) {
            if (pointing != dir) {

            }
            else {
                verticalFlip();
            }
//            std::cerr << "changed dir" << "\n";
//            verticalFlip();
            dir = newDir;
        }
    }

    void handleGroundCollision() {
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
        vector<Point> tmp = getCoords(carRect, angle);
        float x1 = tmp[0].x, y1 = tmp[0].y;
        float x2 = tmp[1].x, y2 = tmp[1].y;
        float x3 = tmp[2].x, y3 = tmp[2].y;
        float x4 = tmp[3].x, y4 = tmp[3].y;

        float minX = min({x1, x2, x3, x4});
        float minY = min({y1, y2, y3, y4});
        float maxX = max({x1, x2, x3, x4});
        float maxY = max({y1, y2, y3, y4});

        bool collideWithGround = 0;
        if (maxY >= groundY) {
            collideWithGround = 1;
            float delta = maxY - groundY;
//            std::cerr << "Collapsed with ground, delta=" << delta << "\n";
            y1 -= delta;
            y2 -= delta;
            y3 -= delta;
            y4 -= delta;
            maxY = max({y1, y2, y3, y4});
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

        minX = min({x1, x2, x3, x4});
        minY = min({y1, y2, y3, y4});
        maxX = max({x1, x2, x3, x4});
        maxY = max({y1, y2, y3, y4});

        if (maxY >= groundY) {
            float delta = maxY - groundY;
//            std::cerr << "Collapsed with ground, delta = " << delta << "\n";
            y1 -= delta;
            y2 -= delta;
            y3 -= delta;
            y4 -= delta;
            maxY = max({y1, y2, y3, y4});
            yPos -= delta;
        }
    }

//    void handleWallCollision() {
//// Assume that the car has the following properties:
//// - x1, y1, x2, y2, x3, y3, x4, y4 are the coordinates of the 4 corners of the rectangle
//// - angle is the current angle of the car (in degrees)
//// - weight, mass, velocityX, velocityY, accelerationX, accelerationY are the properties of the car
//// - screenWidth and screenHeight are the dimensions of the screen
//// - wallAngle is the angle of the wall with respect to the x-axis
//
//// Check if any corner of the car lies outside the screen
////        int width = 100, height = 50;
////        SDL_QueryTexture(carTexture, NULL, NULL, &width, &height);
////
////        float scale = 100.0f / width;
////        width *= scale;
////        height *= scale;
////        SDL_FPoint center = {width/2, height/2};
////        SDL_FPoint center = { width/2, height/2 };
//
//
//        SDL_FRect carRect = {xPos, yPos, width, height};
//        vector<Point> tmp = getCoords(carRect, angle);
//        float x1 = tmp[0].x, y1 = tmp[0].y;
//        float x2 = tmp[1].x, y2 = tmp[1].y;
//        float x3 = tmp[2].x, y3 = tmp[2].y;
//        float x4 = tmp[3].x, y4 = tmp[3].y;
//
//        float minX = min({x1, x2, x3, x4});
//        float minY = min({y1, y2, y3, y4});
//        float maxX = max({x1, x2, x3, x4});
//        float maxY = max({y1, y2, y3, y4});
//
//        bool isOutsideScreen = false;
//        if (x1 <= 0 || x2 <= 0 || x3 <= 0 || x4 <= 0 ||
//            x1 >= WINDOW_WIDTH || x2 >= WINDOW_WIDTH || x3 >= WINDOW_WIDTH || x4 >= WINDOW_WIDTH ||
//            y1 <= 0 || y2 <= 0 || y3 <= 0 || y4 <= 0 ||
//            y1 >= WINDOW_HEIGHT || y2 >= WINDOW_HEIGHT || y3 >= WINDOW_HEIGHT || y4 >= WINDOW_HEIGHT) {
//            isOutsideScreen = true;
//        }
//
//        float wallAngle;
//        if (minX <= 0) {
//            wallAngle = -M_PI/2;
//        }
//        if (maxX >= WINDOW_WIDTH) {
//            wallAngle = M_PI/2;
//        }
//        if (minY <= 0) {
//            wallAngle = M_PI;
//        }
//        if (maxY >= groundY) {
//            wallAngle = 0;
//        }


//        if (isOutsideScreen) {
////            std::cerr << "OUTSIDEEEEEEEEEEEEEEEEEE" << " " << velocityY << " " << velocityX << "\n";
//            // Calculate the angle between the velocity vector of the car and the normal vector of the wall
//            double angleBetween = atan2(velocityY, velocityX) - wallAngle;
//            if (angleBetween < -M_PI) {
//                angleBetween += 2*M_PI;
//            } else if (angleBetween > M_PI) {
//                angleBetween -= 2*M_PI;
//            }
////            std::cerr << "angle between: " << angleBetween << "\n";
//            if (angleBetween < 0) {
//                // Apply a force to slow down the car and eventually stop it
//                double force = mass * accelerationX;
//                velocityX += force * cos(wallAngle);
//                velocityY += force * sin(wallAngle);
//
//                // Calculate the angle of rotation required to make the car parallel to the wall
//                double requiredAngle = wallAngle + 90;
//
//                // Apply a torque to rotate the car towards the required angle
//                double torque = weight * accelerationX * (requiredAngle - angle);
//                angle += torque;
//
//                // Calculate the penetration depth of the car into the wall
//                double penetrationDepth = 0;
//                if (wallAngle == -M_PI/2) {
//                    penetrationDepth = x1;
//                } else if (wallAngle == M_PI/2) {
//                    penetrationDepth = WINDOW_WIDTH - x1;
//                } else if (wallAngle == 0) {
//                    penetrationDepth = WINDOW_HEIGHT - y1;
//                } else if (wallAngle == M_PI) {
//                    penetrationDepth = y1;
//                }
//                penetrationDepth /= cos(angle - wallAngle);
//
//                // Move the car back by the penetration depth
//                x1 -= penetrationDepth * cos(angle);
//                y1 -= penetrationDepth * sin(angle);
//                x2 -= penetrationDepth * cos(angle);
//                y2 -= penetrationDepth * sin(angle);
//                x3 -= penetrationDepth * cos(angle);
//                y3 -= penetrationDepth * sin(angle);
//                x4 -= penetrationDepth * cos(angle);
//                y4 -= penetrationDepth * sin(angle);
//
//                x1 += velocityX;
//                y1 += velocityY;
//                x2 += velocityX;
//                y2 += velocityY;
//                x3 += velocityX;
//                y3 += velocityY;
//                x4 += velocityX;
//                y4 += velocityY;
//
//                vector<Point> manh = {{x1, y1}, {x2, y2}, {x3, y3}, {x4, y4}};
//                std::cerr << "                                        current angle: " << angle << "\n";
//                angle = getAngle(manh);
//                std::cerr << "                                        changing angleeeee to: " << angle << "\n";
//            }
//        }
//    }
};


const float RESTITUTION = 0.5;
class Ball {
public:
    float mass = 50;
    float velocityX = 0;
    float velocityY = 0;
    float accelerationX = 0;
    float accelerationY = 0;
    float xPos;
    float yPos;
    float radius;
    float angle = 0;
    float restitution = 0.9;
    float carBallCollisionVelocityX = 0;
    float carBallCollisionVelocityY = 0;
    float carBallCollisionDragAccelerationX = 0;
    float carBallCollisionDragAccelerationY = 0;
    float gravityVelocityY = 0;
    float initialGravityAccelerationY = GRAVITY_ACCELERATION;
    const float dampening = 0.99;
    float omega = 0;


    Ball(float mass, float velocityX, float velocityY, float accelerationX, float accelerationY, float xPos, float yPos, float radius) :
         mass(mass), velocityX(velocityX), velocityY(velocityY), accelerationX(accelerationX), accelerationY(accelerationY), xPos(xPos), yPos(yPos), radius(radius) {}

    float prvXpos = xPos;
    float prvYpos = yPos;

    deque<SDL_FPoint> centerBuffer;


    void draw(SDL_Renderer* renderer) {
//        std::cerr << "velocityX = " << velocityX << ", " << "velocityY = " << velocityY << "\n";
//        correctAngle();
//        correctPosition();
        SDL_FPoint ballCenter = {xPos + radius, yPos + radius};

        SDL_FPoint center = {radius, radius};
        SDL_FPoint trackedPoint = ballCenter;
        centerBuffer.push_front(trackedPoint);
        if ((int)centerBuffer.size() > MAX_HEAD_TRAIL_LENGTH) {
            centerBuffer.pop_back();
        }

        for (int i = 1; i < (int)centerBuffer.size(); ++i) {

            float maxSpeed = 14.2;
            float curSpeed = sqrt((velocityX*velocityX) + (velocityY*velocityY));
            vector<int> getRGB = calculateColor(curSpeed, maxSpeed);
            SDL_SetRenderDrawColor(renderer, getRGB[0], getRGB[1], getRGB[2], 255);
            SDL_RenderDrawLine(renderer, centerBuffer[i-1].x, centerBuffer[i-1].y, centerBuffer[i].x, centerBuffer[i].y);
//            SDL_RenderCopyExF(renderer, carTexture, NULL, &carRect, angle, &center, flip);
//            SDL_RenderPresent(renderer);
        }

        SDL_FRect ballRect = {xPos, yPos, radius*2, radius*2};
        vector<Point> tmp = getCoords(ballRect, angle);
        float x1 = tmp[0].x, y1 = tmp[0].y;
        float x2 = tmp[1].x, y2 = tmp[1].y;
        float x3 = tmp[2].x, y3 = tmp[2].y;
        float x4 = tmp[3].x, y4 = tmp[3].y;

        float minX = min({x1, x2, x3, x4});
        float minY = min({y1, y2, y3, y4});
        float maxX = max({x1, x2, x3, x4});
        float maxY = max({y1, y2, y3, y4});

        SDL_RenderCopyExF(renderer, ballTexture, NULL, &ballRect, angle, &center, SDL_FLIP_NONE);
        updateExplosion(1);
        renderExplosion(1);
        updateExplosion(2);
        renderExplosion(2);

    }
    bool actuallyTouchingDown = 0;
    bool actuallyTouchingUp = 0;

    float gravityAcc = 0;
    void handleSpin() {
        vec2 ballCenter = {xPos + radius, yPos + radius};
        vec2 direction = {ballCenter.x + velocityX, ballCenter.y + velocityY};
        vec2 normalizedDirection = direction.normalized();
        vec2 velocity = {velocityX, velocityY};

        float stiff = 2;
        if (yPos + 2*radius >= groundY || actuallyTouchingDown) { /// touchDown
            float deltaX = (xPos - prvXpos);
            omega = deltaX*stiff;
            if (actuallyTouchingDown) {
                std::cerr << "adding Omega" << " " << omega << "======================\n";
            }
        }
        else if (xPos <= 0 && (!(yPos >= topLeft1.y && yPos + 2*radius <= botLeft1.y))) { /// touchLeft
            float deltaY = yPos - prvYpos;
            omega = deltaY*stiff;
        }
        else if (xPos + 2*radius >= WINDOW_WIDTH && (!(yPos >= topLeft1.y && yPos + 2*radius <= botLeft1.y))) { /// touchRight
//            std::cerr << "right wall" << "\n";
            float deltaY = yPos - prvYpos;
            omega = -deltaY*stiff;
        }
        else if (yPos <= 0 || actuallyTouchingUp) { /// touchUp
            float deltaX = (xPos - prvXpos);
//            angle += deltaX;
            omega = deltaX*stiff;
        }
        else { /// midAir
            omega *= dampening;
        }
//        std::cerr << "omega = " << omega << "\n";
        angle += omega;
        prvXpos = xPos;
        prvYpos = yPos;
    }

    float distance(float x1, float y1, float x2, float y2) {
        return sqrtf((float)((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1)));
    }


struct Circle {
    Point center;
    double radius;
};

struct LineSegment {
    Point start;
    Point end;
};

double distanceBetweenPoints(Point a, Point b) {
    return std::sqrt(std::pow(a.x - b.x, 2) + std::pow(a.y - b.y, 2));
}


bool touchLine(vec2 _A, vec2 _B) {
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

vec2 topLeft1 = vec2(46, 335);
vec2 topRight1 = vec2(173, 365);
vec2 botLeft1 = vec2(46, 728);
vec2 botRight1 = vec2(174, 701);
vec2 ceilBackGoal1 = vec2(-3000, 335);
vec2 floorBackGoal1 = vec2(-3000, 728);

vec2 topLeft2 = vec2(1746, 365);
vec2 topRight2 = vec2(1873, 335);
vec2 botLeft2 = vec2(1745, 701);
vec2 botRight2 = vec2(1873, 728);
vec2 ceilBackGoal2 = vec2(3000, 335);
vec2 floorBackGoal2 = vec2(3000, 728);

void initLines() {
    return;
    if (fullScreen) {
        topLeft1 = vec2(46, 335);
        topRight1 = vec2(173, 365);
        botLeft1 = vec2(46, 728);
        botRight1 = vec2(174, 701);
        ceilBackGoal1 = vec2(-3000, 216);
        floorBackGoal1 = vec2(-3000, 335);

        topLeft2 = vec2(1746, 365);
        topRight2 = vec2(1873, 335);
        botLeft2 = vec2(1745, 701);
        botRight2 = vec2(1873, 728);
        ceilBackGoal2 = vec2(3000, 335);
        floorBackGoal2 = vec2(3000, 728);
    }
    else {
        topRight1 = vec2(120, 238);
        botRight1 = vec2(120, 470);
        topLeft1 = vec2(24, 216);
        botLeft1 = vec2(24, 490);
        floorBackGoal1 = vec2(-3000, 490);
        ceilBackGoal1 = vec2(-3000, 216);

        topRight2 = vec2(1256, 216);
        botRight2 = vec2(1256, 490);
        topLeft2 = vec2(1160, 238);
        botLeft2 = vec2(1160, 470);
        floorBackGoal2 = vec2(3000, 490);
        ceilBackGoal2 = vec2(3000, 216);
    }
}
bool touchLineDown1() {
    return Chk2(botLeft1, botRight1, 1);
}

bool touchFloorBackGoal1() {
    return Chk2(floorBackGoal1, botLeft1, 1);
}

bool touchLineUp1() {
    return Chk2(topLeft1, topRight1, 0);
}

bool touchCeilBackGoal1() {
    return Chk2(ceilBackGoal1, topLeft1, 0);
}
///
bool touchLineDown2() {
    return Chk2(botLeft2, botRight2, 1);
}

bool touchFloorBackGoal2() {
    return Chk2(botRight2, floorBackGoal2, 1);
}

bool touchLineUp2() {
    return Chk2(topLeft2, topRight2, 0);
}

bool touchCeilBackGoal2() {
    Chk2(topRight2, ceilBackGoal2, 0);
}


void resetBall() {
    xPos = WINDOW_WIDTH / 2 - radius;
    yPos = groundY - radius - 100;
    velocityX = 0;
    velocityY = 0;
    omega = 0;
}

vec2 getIntersection(vec2 A, vec2 B, vec2 P) {
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

int orientation(vec2 p1, vec2 p2, vec2 p3) {
    float val = (p2.y - p1.y) * (p3.x - p2.x)
              - (p2.x - p1.x) * (p3.y - p2.y);

    if (val == 0)
        return 0; // collinear

    return (val < 0) ? -1 : 1; // clockwise, counterclockwise
    /// clockwise:         -1
    /// counterclockwise:   1
}


bool Chk2(vec2 A, vec2 B, bool down) {
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
//    std::cerr << orientation(A, B, center) << " " << (orientation(A, B, center) == 1 ? "clockwise" : "counterclockwise") << "\n";
    if (d1 + d2 - dAB == 0) {
        float dist = distance(center.x, center.y, pointOfContact.x, pointOfContact.y);
        if (dist <= radius) {

            if (orientation(A, B, center) == type && type*velocityY >= 0) {
                std::cerr << "NOT CLOCKWISE, OKE" << "\n";
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

//    }
    void moveBall() {
        drawLine(topRight1, botRight1, 255, 4, 242);
        drawLine(botRight1, botLeft1, 255, 4, 242);
        drawLine(botLeft1, topLeft1, 255, 4, 242);
        drawLine(topLeft1, topRight1, 255, 4, 242);

        drawLine(topRight2, botRight2, 255, 4, 242);
        drawLine(botRight2, botLeft2, 255, 4, 242);
        drawLine(botLeft2, topLeft2, 255, 4, 242);
        drawLine(topLeft2, topRight2, 255, 4, 242);

        drawLine(floorBackGoal1, botLeft1, 255, 4, 242);
        drawLine(botRight2, floorBackGoal2, 255, 4, 242);
        drawLine(ceilBackGoal1, topLeft1, 255, 4, 242);
        drawLine(topRight2, ceilBackGoal2, 255, 4, 242);

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

        if (Chk2(botLeft1, botRight1, 1) || Chk2(floorBackGoal1, botLeft1, 1) || Chk2(botLeft2, botRight2, 1) || Chk2(botRight2, floorBackGoal2, 1)) {
            velocityY *= -RESTITUTION;
            actuallyTouchingDown = 1;
        }
        if (Chk2(topLeft1, topRight1, 0) || Chk2(ceilBackGoal1, topLeft1, 0) || Chk2(topLeft2, topRight2, 0) || Chk2(topRight2, ceilBackGoal2, 0)) {
            velocityY *= -RESTITUTION;
            actuallyTouchingUp = 1;
        }

//        if (Chk2(botLeft1, botRight1, 1)) {
//            velocityY *= -RESTITUTION;
//            actuallyTouchingDown = 1;
//        }
//        else if (Chk2(floorBackGoal1, botLeft1, 1)) {
//            velocityY *= -RESTITUTION;
//            actuallyTouchingDown = 1;
//        }
//
//        if (Chk2(topLeft1, topRight1, 0)) {
//            velocityY *= -RESTITUTION;
//            actuallyTouchingUp = 1;
//        }
//        else if (Chk2(ceilBackGoal1, topLeft1, 0)) {
//            velocityY *= -RESTITUTION;
//            actuallyTouchingUp = 1;
//        }
//
//        if (Chk2(botLeft2, botRight2, 1)) {
//            velocityY *= -RESTITUTION;
//            actuallyTouchingDown = 1;
//        }
//        else if (Chk2(botRight2, floorBackGoal2, 1)) {
//            velocityY *= -RESTITUTION;
//            actuallyTouchingDown = 1;
//        }
//
//        if (Chk2(topLeft2, topRight2, 0)) {
//            velocityY *= -RESTITUTION;
//            actuallyTouchingUp = 1;
//        }
//        else if (Chk2(topRight2, ceilBackGoal2, 0)) {
//            velocityY *= -RESTITUTION;
//            actuallyTouchingUp = 1;
//        }
//        if (touchLineDown1() || touchLineDown2()) {
//            if (prvTouchLineDown == 0) {
//                firstTouchDownY = yPos;
//            }
//        }
//        prvTouchLineDown = (touchLineDown1() || touchLineDown2());
//        if (xPos + radius < botLeft1.x) { /// ben trai botleft1.x
//            if (touchFloorBackGoal1()) {
//                std::cerr << "touchFloorBackgoal" << "\n";
//                if (velocityY >= 0) {
//                    if (yPos + 2*radius >= botLeft1.y) {
//                        yPos = botLeft1.y - 2*radius;
//                    }
//                    velocityY *= -RESTITUTION*2;
//                    restitution = -RESTITUTION;
//                    if (xPos + 2*radius < botLeft1.x) {
//                        xPos = botLeft1.x - 2*radius;
//                        restitution = RESTITUTION;
//                        velocityX *= -RESTITUTION*3;
//                        velocityY *= -RESTITUTION*3;
//                        std::cerr << "EXPLOSIONNNNN!!!!!!!!!!!!!!" << "\n";
//                        addExplosion(1000, xPos, yPos, 2);
//                    }
//                }
//            }
//            if (yPos >= topLeft1.y && yPos + 2*radius <= botLeft1.y && xPos + 2*radius < botLeft1.x) {
//                xPos = botLeft1.x - 2*radius;
//                restitution = RESTITUTION;
//                velocityX *= -RESTITUTION*3;
//                velocityY *= -RESTITUTION*3;
//                std::cerr << "EXPLOSIONNNNN!!!!!!!!!!!!!!" << "\n";
//                addExplosion(1000, xPos, yPos, 2);
//            }
//        }
//        else {
//            if (touchLineDown1()) {
//                if (velocityY >= 0) {
//                    if (yPos >= firstTouchDownY) {
//                        yPos = firstTouchDownY;
//                        velocityY *= -RESTITUTION*2;
//                        restitution = -RESTITUTION;
//                    }
//                }
//            }
//        }

        if (!((((orientation(topLeft1, topRight1, vec2(xPos + radius, yPos)) != 1) && /// -1                    1
           (orientation(botLeft1, botRight1, vec2(xPos + radius, yPos + 2*radius)) != -1) && /// 1              2
           (orientation(floorBackGoal1, botLeft1, vec2(xPos + radius, yPos + 2*radius)) != -1) && /// 1         3
           (orientation(ceilBackGoal1, topLeft1, vec2(xPos + radius, yPos)) != 1)) || /// -1                    4
           ((orientation(topLeft2, topRight2, vec2(xPos + radius, yPos)) != 1) && /// -1                        5
           (orientation(botLeft2, botRight2, vec2(xPos + radius, yPos + 2*radius)) != -1) && /// 1              6
           (orientation(topRight2, ceilBackGoal2, vec2(xPos + radius, yPos)) != 1) && /// -1                    7
           (orientation(botRight2, floorBackGoal2, vec2(xPos + radius, yPos + 2*radius)) != -1)) ||  /// 1      8
            actuallyTouchingDown || actuallyTouchingUp))) { /// 1
                std::cerr << "OUTSIDE GOAL BOX ----------------------------------------------------" << "\n";
                int result1 = orientation(topLeft1, topRight1, vec2(xPos + radius, yPos));
                int result2 = orientation(botLeft1, botRight1, vec2(xPos + radius, yPos + 2*radius));
                int result3 = orientation(floorBackGoal1, botLeft1, vec2(xPos + radius, yPos + 2*radius));
                int result4 = orientation(ceilBackGoal1, topLeft1, vec2(xPos + radius, yPos));
                int result5 = orientation(topLeft2, topRight2, vec2(xPos + radius, yPos));
                int result6 = orientation(botLeft2, botRight2, vec2(xPos + radius, yPos + 2*radius));
                int result7 = orientation(topRight2, ceilBackGoal2, vec2(xPos + radius, yPos));
                int result8 = orientation(botRight2, floorBackGoal2, vec2(xPos + radius, yPos + 2*radius));
                int result9 = actuallyTouchingDown ? 1 : 0;
                int result10 = actuallyTouchingUp ? 1 : 0;

                std::cout << result1 << " " << result2 << " " << result3 << " " << result4 << " " << result5 << " " << result6 << " " << result7 << " " << result8 << " " << result9 << " " << result10 << std::endl;
                if (xPos <= 0) {
                    xPos = 0;
                    restitution = RESTITUTION;
                    velocityX *= -RESTITUTION;
                }
                if (xPos + 2*radius >= WINDOW_WIDTH) {
                    restitution = -RESTITUTION;
                    xPos = WINDOW_WIDTH - 2*radius;
                    velocityX *= -RESTITUTION;
                }
                if (yPos + 2*radius >= groundY) {
                    yPos = groundY - 2*radius;
                    velocityY *= -RESTITUTION*2;
                    restitution = -RESTITUTION;
                }
                if (yPos <= 0) {
                    restitution = RESTITUTION;
                    yPos = 0;
                    velocityY *= -RESTITUTION;
                }
        }
        else {


            std::cerr << "WITHIN GOAL BOX" << "\n";
            if (xPos + 2*radius < botLeft1.x) {
                xPos = botLeft1.x - 2*radius;
                restitution = RESTITUTION;
                velocityX *= -RESTITUTION*3;
                velocityY *= -RESTITUTION*3;
                std::cerr << "EXPLOSIONNNNN!!!!!!!!!!!!!!" << "\n"; /// p2 scores
                ++scoreB;
                addExplosion(1000, xPos, yPos, 2);
            }
            if (xPos > topRight2.x) {
                xPos = topRight2.x;
                restitution = -RESTITUTION;
                velocityX *= -RESTITUTION*3;
                velocityY *= -RESTITUTION*3;
                std::cerr << "EXPLOSIONNNNN!!!!!!!!!!!!!!" << "\n"; /// p1 scores
                ++scoreA;
                addExplosion(1000, xPos, yPos, 1);
            }
        }


//        if (!(yPos >= topLeft1.y && yPos + 2*radius <= botLeft1.y)) {
//            if (xPos <= 0) {
//                xPos = 0;
//                restitution = RESTITUTION;
//                velocityX *= -RESTITUTION;
//            }
//            if (xPos + 2*radius >= WINDOW_WIDTH) {
//                restitution = -RESTITUTION;
//                xPos = WINDOW_WIDTH - 2*radius;
//                velocityX *= -RESTITUTION;
//            }
//            if (yPos + 2*radius >= groundY) {
//                yPos = groundY - 2*radius;
//                velocityY *= -RESTITUTION*2;
//                restitution = -RESTITUTION;
//            }
//            if (yPos <= 0) {
//                restitution = RESTITUTION;
//                yPos = 0;
//                velocityY *= -RESTITUTION;
//            }
//        }
//        else {
//            if (xPos + 2*radius < botLeft1.x) {
//                xPos = botLeft1.x - 2*radius;
//                restitution = RESTITUTION;
//                velocityX *= -RESTITUTION*3;
//                velocityY *= -RESTITUTION*3;
//                std::cerr << "EXPLOSIONNNNN!!!!!!!!!!!!!!" << "\n";
//                addExplosion(1000, xPos, yPos, 2);
//            }
//            if (xPos > topRight2.x) {
//                xPos = topRight2.x;
//                restitution = -RESTITUTION;
//                velocityX *= -RESTITUTION*3;
//                velocityY *= -RESTITUTION*3;
//                std::cerr << "EXPLOSIONNNNN!!!!!!!!!!!!!!" << "\n";
//                addExplosion(1000, xPos, yPos, 1);
//            }
//        }


        std::cerr << xPos << " " << yPos << "\n";

//        if ((xPos + radius >= botLeft1.x && touchLineDown1()) ||
//            (xPos + radius <= botRight2.x && touchLineDown2()) ||
//            (xPos + radius < botLeft1.x && yPos + 2*radius <= botLeft1.y && touchFloorBackGoal1()) ||
//            (xPos + radius > botRight2.x && yPos + 2*radius <= botRight2.y && touchFloorBackGoal2())) {
//                if (prvTouchDown == 0) {
//                    beforeTouchingLineDown = yPos;
//                }
//        }
//        else {
////            beforeTouchingLineDown = yPos;
//        }
//        prvTouchDown = ((xPos + radius >= botLeft1.x && touchLineDown1()) ||
//            (xPos + radius <= botRight2.x && touchLineDown2()) ||
//            (xPos + radius < botLeft1.x && yPos + 2*radius <= botLeft1.y && touchFloorBackGoal1()) ||
//            (xPos + radius > botRight2.x && yPos + 2*radius <= botRight2.y && touchFloorBackGoal2()));
//
//        std::cerr << "beforeTouching = " << beforeTouchingLineDown << "\n";
//
//        if ((xPos + radius >= botLeft1.x && touchLineDown1()) ||
//            (xPos + radius <= botRight2.x && touchLineDown2()) ||
//            (xPos + radius < botLeft1.x && yPos + 2*radius <= botLeft1.y && touchFloorBackGoal1()) ||
//            (xPos + radius > botRight2.x && yPos + 2*radius <= botRight2.y && touchFloorBackGoal2())) {
//                actuallyTouchingDown = 1;
//                restitution = RESTITUTION;
//                velocityY *= -RESTITUTION*2;
//                if (yPos >= beforeTouchingLineDown) {
//                    yPos = beforeTouchingLineDown;
//                }
//        }
//        else {
//            actuallyTouchingDown = 0;
//        }
//        if (yPos + 2*radius > groundY) { /// ground
//            velocityY *= -RESTITUTION*2;
//            restitution = -RESTITUTION;
//            yPos = groundY - 2*radius;
//
//        }
//        std::cerr << "yPos = " << yPos << "\n";
//
//
//        if (((yPos >= topLeft1.y) && (yPos + 2*radius <= botLeft1.y)) || (yPos <= beforeTouchingLineDown) ||
//            ((xPos + radius >= botLeft1.x && touchLineDown1()) ||
//            (xPos + radius <= botRight2.x && touchLineDown2()) ||
//            (xPos + radius < botLeft1.x && yPos + 2*radius <= botLeft1.y && touchFloorBackGoal1()) ||
//            (xPos + radius > botRight2.x && yPos + 2*radius <= botRight2.y && touchFloorBackGoal2()))) {
//            if (xPos + 2*radius < topLeft1.x) {
//                xPos = topLeft1.x - 2*radius;
//                restitution = RESTITUTION;
//                velocityX *= -RESTITUTION*3;
//                velocityY *= -RESTITUTION*3;
//                std::cerr << "EXPLOSIONNNNN!!!!!!!!!!!!!!" << "\n";
//                addExplosion(1000, xPos, yPos, 2);
////                resetBall();
//            }
//            if (xPos > topRight2.x) {
//                xPos = topRight2.x;
//                restitution = -RESTITUTION;
//                velocityX *= -RESTITUTION*3;
//                velocityY *= -RESTITUTION*3;
//                std::cerr << "EXPLOSIONNNNN!!!!!!!!!!!!!!" << "\n";
//                addExplosion(1000, xPos, yPos, 1);
//            }
//        }
//        else {
//
//            std::cerr << "LOOK AT ME: " << yPos+2*radius << " " << botLeft1.y << "\n";
//            if (xPos < 0) {
//                std::cerr << "meh, prolly incorrect ===============================================" << " " << yPos + 2*radius << " " << botLeft1.x << "\n";
//                restitution = RESTITUTION;
//                xPos = 0;
//                velocityX *= -RESTITUTION;
//            }
//
//            if (xPos + 2*radius > WINDOW_WIDTH) {
//                restitution = -RESTITUTION;
//                xPos = WINDOW_WIDTH - 2*radius;
//                velocityX *= -RESTITUTION;
//            }
//        }

//        if (yPos >= topRight2.y && yPos + 2*radius <= botRight2.y) {
//            if (xPos > topRight2.x) {
//                xPos = topRight2.x;
//                restitution = -RESTITUTION;
//                velocityX *= -RESTITUTION*3;
//                velocityY *= -RESTITUTION*3;
//                std::cerr << "EXPLOSIONNNNN!!!!!!!!!!!!!!" << "\n";
//                addExplosion(1000, xPos, yPos, 1);
////                resetBall();
//            }
//        }
//        else if (xPos + 2*radius > WINDOW_WIDTH) {
//            restitution = -RESTITUTION;
//            xPos = WINDOW_WIDTH - 2*radius;
//            velocityX *= -RESTITUTION;
//        }
//        if (xPos + 2*radius > WINDOW_WIDTH || touchLineRight2()) { /// right
//            restitution = -RESTITUTION;
//            if (touchLineRight2()) {
//                std::cerr << "EXPLOSIONNNNN!!!!!!!!!!!!!!" << "\n";
//                addExplosion(1000, xPos, yPos, 1);
//            }
//            if (xPos + 2*radius > WINDOW_WIDTH) {
//                xPos = WINDOW_WIDTH - 2*radius;
//            }
//            velocityX *= -RESTITUTION;
//        }
//        if (yPos < 0 || touchLineUp1() || touchLineUp2()) { /// up
//            restitution = RESTITUTION;
////            yPos = 0;
//            if (yPos < 0) {
//                yPos = 0;
//            }
//            velocityY *= -RESTITUTION;
//        }


    }

};

bool intersectCarBall(const obb a, const sphere b, vec2& collision_normal, vec2& p) {
    p = nearestPointOnOBB(a, b.center);
    vec2 v = p - b.center;
    if (v.length() <= b.radius) {
        drawLine(p, b.center, 0, 128, 0);
        collision_normal = v.normalized();
        return 1;
    }
    else {
        drawLine(p, b.center, 255, 0, 0);
        return 0;
    }
}

//vec2 nearestPointOnOBB(Car car, const vec2 point) {
//    SDL_FRect carRect = {car.xPos, car.yPos, car.width, car.height};
//    vector<Point> tmp = getCoords(carRect, car.angle);
//    float x1 = tmp[0].x, y1 = tmp[0].y;
//    float x2 = tmp[1].x, y2 = tmp[1].y;
//    float x3 = tmp[2].x, y3 = tmp[2].y;
//    float x4 = tmp[3].x, y4 = tmp[3].y;
//
//    float minX = min({x1, x2, x3, x4});
//    float minY = min({y1, y2, y3, y4});
//    float maxX = max({x1, x2, x3, x4});
//    float maxY = max({y1, y2, y3, y4});
//
//    float centerX = (x1 + x2 + x3 + x4) / 4.0f;
//    float centerY = (y1 + y2 + y3 + y4) / 4.0f;
//
//    vec2 center = (centerX, centerY);
//    float angle = car.angle;
//    float width = car.width;
//    float height = car.height;
//
//
//
//    // Step 1: Calculate the local coordinate system (u, v) using the OBB's angle.
//    vec2 u(cos(angle), sin(angle));
//    vec2 v(-sin(angle), cos(angle));
//
//    // Step 2: Calculate the point's coordinates relative to the OBB's center in the local coordinate system.
//    vec2 localPoint = point - center;
//    vec2 relativePoint(dot(localPoint, u), dot(localPoint, v));
//
//    // Step 3: Project the relative point onto the u and v axes to find its coordinates in the OBB's local space.
//    vec2 localProjection(clamp(relativePoint.x, -width / 2.0f, width / 2.0f),
//                         clamp(relativePoint.y, -height / 2.0f, height / 2.0f));
//
//    // Step 4: Clamp the projected coordinates to the OBB's half-width and half-height to get the point's local coordinates on the OBB's edge.
//    vec2 clampedLocalPoint = localProjection - relativePoint;
//
//    // Step 5: Transform the local point back into world space by applying the OBB's angle and center position.
//    vec2 worldPoint = center + dot(clampedLocalPoint, vec2(u.x, v.x), vec2(u.y, v.y));
//
//    return worldPoint;
//}

obb carToObb(Car car) {
    SDL_FRect carRect = {car.xPos, car.yPos, car.width, car.height};
    vector<Point> tmp = getCoords(carRect, car.angle);
    float x1 = tmp[0].x, y1 = tmp[0].y;
    float x2 = tmp[1].x, y2 = tmp[1].y;
    float x3 = tmp[2].x, y3 = tmp[2].y;
    float x4 = tmp[3].x, y4 = tmp[3].y;

    float minX = min({x1, x2, x3, x4});
    float minY = min({y1, y2, y3, y4});
    float maxX = max({x1, x2, x3, x4});
    float maxY = max({y1, y2, y3, y4});

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

    drawLine(vec2(x1, y1), vec2(x2, y2), 0, 255, 0);
    drawLine(vec2(x2, y2), vec2(x3, y3), 0, 255, 0);
    drawLine(vec2(x3, y3), vec2(x4, y4), 0, 255, 0);
    drawLine(vec2(x4, y4), vec2(x1, y1), 0, 255, 0);

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
    vector<Point> tmp = getCoords(carRect, car.angle);
    float x1 = tmp[0].x, y1 = tmp[0].y;
    float x2 = tmp[1].x, y2 = tmp[1].y;
    float x3 = tmp[2].x, y3 = tmp[2].y;
    float x4 = tmp[3].x, y4 = tmp[3].y;

    float minX = min({x1, x2, x3, x4});
    float minY = min({y1, y2, y3, y4});
    float maxX = max({x1, x2, x3, x4});
    float maxY = max({y1, y2, y3, y4});

    float center_x = (x1 + x2 + x3 + x4) / 4.0f;
    float center_y = (y1 + y2 + y3 + y4) / 4.0f;

    obb car_obb = carToObb(car);

    sphere ball_sphere = ballToSphere(ball);

    vec2 collision_normal;
    vec2 p;
    if (intersectCarBall(car_obb, ball_sphere, collision_normal, p)) {
//        std::cerr << "car and ball intersect" << "\n";
//        float carCenterX = car.xPos + (car.width / 2.0f) * cos(car.angle) - (car.height / 2.0f) * sin(car.angle);
//        float carCenterY = car.yPos + (car.height / 2.0f) * cos(car.angle) + (car.width / 2.0f) * sin(car.angle);
//
        float carCenterX = center_x;
        float carCenterY = center_y;

        // Calculate the vector from the point of contact to the center of the ball
        vec2 contactPoint = nearestPointOnOBB(car_obb, ball_sphere.center);
//        vec2 normalizedContactVector = (vec2(carCenterX, carCenterY) - contactPoint).normalized(); //
        vec2 normalizedContactVector = (ball_sphere.center - contactPoint).normalized(); //
//        vec2 normalizedContactVector = (contactPoint - ball_sphere.center).normalized(); //

//        vec2 curVelocity = {car.velocityX, car.velocityY};
//
//        vec2 newVelocity = curVelocity - normalizedContactVector*2*(curVelocity.dot(normalizedContactVector));
//
//        car.carBallCollisionVelocityX = -0.8f * newVelocity.x;
//        car.carBallCollisionVelocityY = -0.8f * newVelocity.y;
//
//        car.carBallCollisionDragAccelerationX = -0.1f * car.carBallCollisionVelocityX;
//        car.carBallCollisionDragAccelerationY = -0.1f * car.carBallCollisionVelocityY;


//        car.velocityX = newVelocity[0];
//        car.velocityY = newVelocity[1];


//        xPos += normal.x * (ball_sphere.radius - norm(ball_sphere.center - point_of_contact));
//        yPos += normal.y * (ball_sphere.radius - norm(ball_sphere.center - point_of_contact));

            vec2 ballVelocity = vec2(ball.velocityX, ball.velocityY);
            vec2 carVelocity = vec2(car.velocityX, car.velocityY);

            vec2 relativeVelocity = ballVelocity - carVelocity;

            float relativeSpeed = relativeVelocity.dot(normalizedContactVector);

        if (relativeSpeed < 0.0f) {
//            float impulseMagnitude = (-(1.0f + ball.restitution) * relativeSpeed) / ((1.0f / ball.mass) + (1.0f / car.mass));

            /// BALL
//            vec2 ballImpulse = normalizedContactVector * impulseMagnitude * 0.2;
////            ball.carBallCollisionVelocityX = ballImpulse.x;
////            ball.carBallCollisionVelocityY = ballImpulse.y;//
//            ball.velocityX = ballImpulse.x;
//            ball.velocityY = ballImpulse.y;

            // calculate the collision impulse
            float j = -(1.0f + RESTITUTION) * relativeSpeed;
            j /= 1.0f / ball.mass + 1.0f / car.mass;

            // apply impulse
            vec2 impulse = normalizedContactVector*j;
            vec2 ballImpulse = impulse * (1.0f / ball.mass);
            vec2 carImpulse = impulse * (1.0f / car.mass);
            ball.velocityX += ballImpulse.x;
            ball.velocityY += ballImpulse.y;
            car.velocityX -= carImpulse.x;
            car.velocityY -= carImpulse.y;

            ball.omega = ballImpulse.x * 0.5;
            // Update the ball's position to avoid overlap
//            vec2 separation = normalizedContactVector * (ball.radius - distance(contactPoint, vec2(carCenterX, carCenterY)));
//            vec2 newBallPosition = vec2(ball.xPos, ball.yPos) - separation;
//            ball.xPos = newBallPosition.x;
//            ball.yPos = newBallPosition.y;

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


void testRender() {
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

  // Clear the entire screen with black color
  SDL_RenderClear(renderer);

  // Set the rendering color to green
  SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);

  // Create a SDL_Rect structure with the coordinates for the rectangle
  SDL_Rect rect;
  rect.x = 0;
  rect.y = 380;
  rect.w = 640;
  rect.h = 100;

  // Render the filled rectangle on the screen
  SDL_RenderFillRect(renderer, &rect);

  // Update the screen
  SDL_RenderPresent(renderer);

  // Wait for a few seconds before quitting
  SDL_Delay(3000);

  // Destroy the renderer and window
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
}

void renderGround() {
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    SDL_FRect ground;
    ground.x = 0;
    ground.y = groundY;
    ground.w = WINDOW_WIDTH;
    ground.h = WINDOW_HEIGHT - groundY;

//    ground.x = 0;
//    ground.y = 380;
//    ground.w = 640;
//    ground.h = 100;
    SDL_RenderFillRectF(renderer, &ground);
}

void closeEverything() {
//    SDL_DestroyTexture(carB);

    SDL_DestroyRenderer(renderer);
    renderer = NULL;

    SDL_DestroyWindow(window);
    window = NULL;

//    SDL_DestroyTexture(explosionTexture);
//    boostTexture = NULL;

//    SDL_DestroyTexture(car1_Texture);
//    car1_Texture = NULL;
//
//    SDL_DestroyTexture(car2_Texture);
//    car2_Texture = NULL;

    SDL_DestroyTexture(ballTexture);
    ballTexture = NULL;

    IMG_Quit();
    SDL_Quit();
}

SDL_FPoint abcxyz = {WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2};
float tmp_delta = 0;
int tmp_sign = 1;
void UpdatePrintPoint(SDL_FPoint& point, Ball& ball) {
    vec2 tmp = {point.x, point.y};
    float dist = 50;

    vec2 perpen = vec2(ball.velocityX, ball.velocityY).perpendicular().normalized();
    tmp = tmp + perpen * dist * 0.2 * tmp_sign;
    tmp_delta += dist * 0.2 * tmp_sign;
    if (tmp_delta >= dist) {
        tmp_sign *= -1;
    }
    if (tmp_delta <= 0) {
        tmp_sign *= -1;
    }
    point = {tmp.x, tmp.y};
}
SDL_Surface* surfaceMessage = NULL;
SDL_Texture* Message = NULL;
//TTF_Font* Font = NULL;

string toString(int x) {
    string s;
    while (x != 0) {
        s = char((x % 10) + '0') + s;
        x /= 10;
    }
    if ((int)s.size() == 0) {
        s = "0";
    }
    return s;
}
pair<SDL_FRect, float> scorePlayer(int player, int w, int h) {
    float angle;
    SDL_FRect rect;
    vec2 center;
    rect.w = w;
    rect.h = h;
    if (player == 1) {
        angle = -4.5;
        center = vec2(870, 28);
    }
    else {
        angle = 4.5;
        center = vec2(1049, 28);
    }
    rect.x = center.x - w/2;
    rect.y = center.y - h/2;

    angle = 0;
    return {rect, angle};
}

SDL_Color White = {255, 255, 255};
SDL_Texture* scoreTextureA = nullptr;
SDL_Texture* scoreTextureB = nullptr;
TTF_Font* Font = nullptr;
int messageTextWidth[] = {0, 0, 0};
int messageTextHeight[] = {0, 0, 0};

int getFontSize(int score) {
    if (score <= 9) {
        return 50;
    }
    else if (score <= 99) {
        return 45;
    }
    else {
        return 30;
    }
}

void displayScore(int score, int player) {
    if ((player == 1 && score != prvScoreA) || (player == 2 && score != prvScoreB)) {
        int fontSize =  getFontSize(score);
        Font = TTF_OpenFont("D:/gameProject/game/assets/Fonts/usethis/sans.ttf", fontSize);
        string s = toString(score);
//        surfaceMessage = TTF_RenderText_Solid(Font, s.c_str(), White);
        surfaceMessage = TTF_RenderText_Blended(Font, s.c_str(), White);
        Font = nullptr;
        TTF_CloseFont(Font);
        messageTextWidth[player] = surfaceMessage->w;
        messageTextHeight[player] = surfaceMessage->h;

        if (player == 1) {
            if (scoreTextureA) SDL_DestroyTexture(scoreTextureA);
            scoreTextureA = SDL_CreateTextureFromSurface(renderer, surfaceMessage);
            prvScoreA = score;
        } else {
            if (scoreTextureB) SDL_DestroyTexture(scoreTextureB);
            scoreTextureB = SDL_CreateTextureFromSurface(renderer, surfaceMessage);
            prvScoreB = score;
        }
        SDL_FreeSurface(surfaceMessage);
    }

    pair<SDL_FRect, float> tmp = scorePlayer(player, messageTextWidth[player], messageTextHeight[player]);
    SDL_FRect Message_rect = tmp.first;
    if (player == 1) {
        SDL_RenderCopyExF(renderer, scoreTextureA, NULL, &Message_rect, tmp.second, NULL, SDL_FLIP_NONE);
    } else {
        SDL_RenderCopyExF(renderer, scoreTextureB, NULL, &Message_rect, tmp.second, NULL, SDL_FLIP_NONE);
    }
}

int main(int argc, char* argv[]) {
    if (WINDOW_WIDTH == 1920) {
        fullScreen = 1;
    }
    else {
        fullScreen = 0;
    }
    if (!initEverything()) {
        return 1;
    }

    SDL_Texture* backgroundTexture = IMG_LoadTexture(renderer, "D:/gameProject/game/assets/usable_bg.png");
    SDL_Texture* frontPart = IMG_LoadTexture(renderer, "D:/gameProject/game/assets/front_part.png");
    SDL_Texture* score = IMG_LoadTexture(renderer, "D:/gameProject/game/assets/score2.png");

//    ballTexture = IMG_LoadTexture(renderer, "D:/gameProject/game/assets/soccer_ball.png");
    ballTexture = IMG_LoadTexture(renderer, "D:/gameProject/game/assets/ggg.png");
    SDL_Texture* car1_Texture = IMG_LoadTexture(renderer, "D:/gameProject/game/assets/spr_casualcar_0.png");
    SDL_Texture* car2_Texture = IMG_LoadTexture(renderer, "D:/gameProject/game/assets/car1_red.png");

    vector<vector<Point>> tmp(2, vector<Point>(2));
    Car car2(100, 0, 0, 0, 0, WINDOW_WIDTH / 2 + 300 / 2, groundY - 32, 1, 0, 0, 0, SDL_FLIP_NONE, 0, 0, 100, car2_Texture, 2);
    Car car1(100, 0, 0, 0, 0, WINDOW_WIDTH / 2 - 300 / 2, groundY - 32, 1, 0, 0, 0, SDL_FLIP_NONE, 0, 0, 100, car1_Texture, 1);
    car2.initSize();
    car2.yPos = groundY - car2.height;

    car1.initSize();
    car1.yPos = groundY - car1.height;



//    Ball ball(10, 0, 0, 0, 0, WINDOW_WIDTH/4, groundY - 2*25, 25);
    Ball ball(10, 0, 0, 0, 0, WINDOW_WIDTH/2 - 5, WINDOW_HEIGHT/3, 25);

    bool isRunning = true;

    std::string player2_prvLeftRight = "";
    std::string player1_prvLeftRight = "";

    /// player1 controls
    const int player1_scancode_up = SDL_SCANCODE_W;
    const int player1_scancode_down = SDL_SCANCODE_S;
    const int player1_scancode_left = SDL_SCANCODE_A;
    const int player1_scancode_right = SDL_SCANCODE_D;
    const int player1_scancode_boost = SDL_SCANCODE_G;
    const int player1_scancode_jump = SDL_SCANCODE_H;

    SDL_Keycode player1_sym_up = SDLK_w;
    SDL_Keycode player1_sym_down = SDLK_s;
    SDL_Keycode player1_sym_left = SDLK_a;
    SDL_Keycode player1_sym_right = SDLK_d;
    SDL_Keycode player1_sym_boost = SDLK_g;
    SDL_Keycode player1_sym_jump = SDLK_h;
    ///

    /// player2 controls
    const int player2_scancode_up = SDL_SCANCODE_UP;
    const int player2_scancode_down = SDL_SCANCODE_DOWN;
    const int player2_scancode_left = SDL_SCANCODE_LEFT;
    const int player2_scancode_right = SDL_SCANCODE_RIGHT;
    const int player2_scancode_boost = SDL_SCANCODE_RIGHTBRACKET; // boost
    const int player2_scancode_jump = SDL_SCANCODE_BACKSLASH; // jump

    SDL_Keycode player2_sym_up = SDLK_UP;
    SDL_Keycode player2_sym_down = SDLK_DOWN;
    SDL_Keycode player2_sym_left = SDLK_LEFT;
    SDL_Keycode player2_sym_right = SDLK_RIGHT;
    SDL_Keycode player2_sym_boost = SDLK_RIGHTBRACKET;
    SDL_Keycode player2_sym_jump = SDLK_BACKSLASH;
    ///

    bool enable_player2 = 1; // red
    bool enable_player1 = 1; // blue

    int timeLeft = 300; // 5 minutes in seconds
    Uint32 lastTime = SDL_GetTicks();

    SDL_Texture* timeTexture = NULL;
    SDL_Surface* textSurface = NULL;

    while (isRunning) {
        ///
        ball.initLines();

        SDL_Event event;
        const Uint8* state = SDL_GetKeyboardState(NULL);

        /// player2
        if (enable_player2) {
            std::string player2_stateLeft = (state[player2_scancode_left] ? "1" : "0");
            std::string player2_stateRight = (state[player2_scancode_right] ? "1" : "0");
            std::string player2_curLeftRight = player2_stateLeft + player2_stateRight;
            if (state[player2_scancode_left] && state[player2_scancode_right]) {
                if (player2_prvLeftRight == "10") {
                    car2.moveRight();
                    car2.setDir(1);
                }
                else {
                    car2.moveLeft();
                    car2.setDir(0);
                }
            }
            else {
                if (state[player2_scancode_left]) {
                    car2.moveLeft();
                    car2.setDir(0);
                }
                else if (state[player2_scancode_right]) {
                    car2.moveRight();
                    car2.setDir(1);
                }
                else {
                    car2.goingVelocityX = 0;
                }
            }

            while (SDL_PollEvent(&event)) {
                switch (event.type) {
                    case SDL_QUIT:
                        isRunning = false;
                        break;
                    case SDL_KEYDOWN:
                        if (event.key.keysym.sym == player2_sym_jump) { // jump
                            car2.jump();
                            car2.canJump = 0;
                            if (state[SDL_SCANCODE_DOWN]) {

                                car2.dodgeUp();
                            }
                            if (state[SDL_SCANCODE_UP]) {
                                car2.dodgeDown();
                            }
                        }

                        if (event.key.keysym.sym == player1_sym_jump) { // jump
//                            std::cerr << "player1 hit jump" << "\n";
                            car1.jump();
                            car1.canJump = 0;
                            if (state[player1_scancode_down]) {
                                car1.dodgeUp();
                            }
                            if (state[player1_scancode_up]) {
                                car1.dodgeDown();
                            }
                        }

                        break;
                    case SDL_KEYUP:
                        if (event.key.keysym.sym == player2_sym_jump) {
                            car2.canJump = 1;
                        }
                        if (event.key.keysym.sym == player1_sym_jump) {
                            car1.canJump = 1;
                        }
                }
            }

            if (player2_curLeftRight == "10" || player2_curLeftRight == "01") {
                player2_prvLeftRight = player2_curLeftRight;
            }

            if (state[player2_scancode_up]) {
                car2.tiltUp();
            }
            else if (state[player2_scancode_down]) {
                car2.tiltDown();
            }

            if (state[player2_scancode_boost]) {
                car2.curSign = 1.0;
                car2.boost(1.0);
            }
            else {
                car2.curSign = -1.0;
                car2.boost(-1.0);
            }
        }


        /// player1
        if (enable_player1) {
            std::string player1_stateLeft = (state[player1_scancode_left] ? "1" : "0");
            std::string player1_stateRight = (state[player1_scancode_right] ? "1" : "0");
            std::string player1_curLeftRight = player1_stateLeft + player1_stateRight;
            if (state[player1_scancode_left] && state[player1_scancode_right]) {
                if (player1_prvLeftRight == "10") {
                    car1.moveRight();
                    car1.setDir(1);
                }
                else {
                    car1.moveLeft();
                    car1.setDir(0);
                }
            }
            else {
                if (state[player1_scancode_left]) {
                    car1.moveLeft();
                    car1.setDir(0);
                }
                else if (state[player1_scancode_right]) {
                    car1.moveRight();
                    car1.setDir(1);
                }
                else {
                    car1.goingVelocityX = 0;
                }
            }

            while (SDL_PollEvent(&event)) {
                switch (event.type) {
                    case SDL_QUIT:
                        isRunning = false;
                        break;
                    case SDL_KEYDOWN:
                        if (event.key.keysym.sym == player1_sym_jump) { // jump
                            std::cerr << "player1 hit jump" << "\n";
                            car1.jump();
                            car1.canJump = 0;
                            if (state[player1_scancode_down]) {
                                car1.dodgeUp();
                            }
                            if (state[player1_scancode_up]) {
                                car1.dodgeDown();
                            }
                        }
                        break;
                    case SDL_KEYUP:
                        if (event.key.keysym.sym == player1_sym_jump) {
                            car1.canJump = 1;
                        }
                }
            }

            if (player1_curLeftRight == "10" || player1_curLeftRight == "01") {
                player1_prvLeftRight = player1_curLeftRight;
            }

            if (state[player1_scancode_up]) {
                car1.tiltUp();
            }
            else if (state[player1_scancode_down]) {
                car1.tiltDown();
            }

            if (state[player1_scancode_boost]) {
                car1.curSign = 1.0;
                car1.boost(1.0);
            }
            else {
                car1.curSign = -1.0;
                car1.boost(-1.0);
            }
        }
        ///
        Uint32 currentTime = SDL_GetTicks();
        if (currentTime > lastTime + 1000)
        {
            timeLeft--;
            lastTime = currentTime;
        }

        int minutes = timeLeft / 60;
        int seconds = timeLeft % 60;

        std::string timeText = std::to_string(minutes) + ":" + (seconds < 10 ? "0" : "") + std::to_string(seconds);
        Font = TTF_OpenFont("D:/gameProject/game/assets/Fonts/usethis/sans.ttf", 50);
        textSurface = TTF_RenderText_Blended(Font, timeText.c_str(), { 255, 255, 255 });
        Font = nullptr;
        TTF_CloseFont(Font);

        timeTexture = SDL_CreateTextureFromSurface(renderer, textSurface);

        int text_width = textSurface->w;
        int text_height = textSurface->h;

        screenSurface = SDL_GetWindowSurface(window);

        SDL_Rect renderQuad = { 962 - text_width/ 2, 24 - text_height/ 2, text_width, text_height };
        ///


        SDL_SetRenderDrawColor(renderer, 221, 160, 221, 255);
        SDL_RenderClear(renderer);

        SDL_Rect dstRect = { 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT }; // Set destination rectangle to cover entire window
        SDL_Rect srcRect = { 0, 0, 1920, 1080 }; // Example: render the entire texture

        SDL_RenderCopy(renderer, backgroundTexture,
                       &srcRect,
                       &dstRect);



//        renderGround();

        car2.applyGravity();
        car1.applyGravity();

        car2.handleGroundCollision();
        car1.handleGroundCollision();

        car2.moveCar();
        car1.moveCar();
        ball.moveBall();
//
        handleCollisionCarBall(car2, ball);
        handleCollisionCarBall(car1, ball);

        ball.handleSpin();
        ball.draw(renderer);

//        UpdatePrintPoint(abcxyz, ball);
//        PrintPoint(abcxyz);

        ///
        if (ball.yPos + ball.radius*2 <= ball.botLeft1.y + 1e-3) {
           SDL_RenderCopy(renderer, frontPart,
                           &srcRect,
                           &dstRect);
        }


        car2.draw(renderer);

        car1.draw(renderer);

        SDL_RenderCopy(renderer, score,
                       &srcRect,
                       &dstRect);

//        displayScore(scoreA, 1);
////        displayScore(scoreB, 2);
////        if (scoreA != prvScoreA) {
//            displayScore(scoreA, 1, Font, scoreTextures);
////        }
////        if (scoreB != prvScoreB) {
//            displayScore(scoreB, 2, Font, scoreTextures);
////        }

//        if (scoreA != prvScoreA) {
            displayScore(scoreA, 1);
//        }
//        if (scoreB != prvScoreB) {
            displayScore(scoreB, 2);
//        }
        SDL_RenderCopy(renderer, timeTexture, NULL, &renderQuad);

        SDL_RenderPresent(renderer);
//
//        std::cerr << "maxSpeed = " << maxSpeed << "\n";


//        SDL_Delay(200);
//        ClearScreen();
//        bool gg = 1;
//        for (int i = 1; gg&&i <= 50; ++i) {
//            for (int j = 1; j <= 50; ++j) {
//                std::cerr << " ";
//            }
//            std::cerr << "\n";
//        }

//        ClearScreen();

//		Uint32 currentTime = SDL_GetTicks();
//
//        // Update countdown value based on elapsed time
//        countdownValue = 300 - (currentTime - startTime) / 1000;
//
//        int minutes = countDownValue / 60;
//        int seconds = countDownValue % 60;
//
//        std::string timeText = std::to_string(minutes) + ":" + std::to_string(seconds);




        car2.prvSign = car2.curSign;
        car2.prvDir = car2.dir;
        car1.prvSign = car1.curSign;
        car1.prvDir = car1.dir;
        prvScoreA = scoreA;
        prvScoreB = scoreB;
//        std::cerr << (int)boostParticles.size() << "\n";

    }
    SDL_FreeSurface(surfaceMessage);
    SDL_DestroyTexture(Message);
    closeEverything();
    return 0;
}
