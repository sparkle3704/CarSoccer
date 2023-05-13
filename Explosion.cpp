#include "Explosion.h"
#include <windows.h>
#include "CommonFunctions.h"
#include <random>
#include "SDL_Contexts.h"
#include <algorithm>
#include <array>

std::random_device rd2;
std::mt19937 gen2(rd2());
std::vector<Particle> explosionParticles[3];
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
}

void updateExplosion(int player) {
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
    SDL_SetRenderDrawBlendMode(renderer.get(), SDL_BLENDMODE_BLEND);

    // Clear texture with transparent color
    SDL_SetRenderDrawColor(renderer.get(), 0, 0, 0, 0);

    for (auto& particle: explosionParticles[player]) {
        SDL_FRect boostRect = {particle.x - particle.size/2.0f, particle.y - particle.size/2.0f, particle.size, particle.size};
        std::array<int, 3> rgb = boostColor(particle.alpha, player);
        SDL_SetRenderDrawColor(renderer.get(), rgb[0], rgb[1], rgb[2], 255);
        SDL_RenderFillRectF(renderer.get(), &boostRect);
    }
    // Reset render target to default
    SDL_SetRenderTarget(renderer.get(), nullptr);

    // Draw texture to screen
    SDL_RenderCopy(renderer.get(), nullptr, nullptr, nullptr);
}
