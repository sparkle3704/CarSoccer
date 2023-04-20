#include "Boost.h"
#include <windows.h>
#include "CommonFunctions.h"
#include <random>
#include <vector>
#include "Structs.h"
#include "SDL_Contexts.h"
#include <algorithm>
#include <array>

std::random_device rd;
std::mt19937 gen(rd());

std::vector<Particle> boostParticles[3];
SDL_Texture* boostTexture = nullptr;
void addBoost(int numParticles, Point a, Point b, Point c, Point d, int player) {

//    std::uniform_real_distribution<float> v_gen(1, 1);
//    std::uniform_real_distribution<float> size_gen(1, 20);
//    std::uniform_real_distribution<float> alpha_gen(1, 1);
//    std::uniform_real_distribution<float> x_gen(min(a.x, b.x), max(a.x, b.x));
//    std::uniform_real_distribution<float> y_gen(min(a.y, b.y), max(a.y, b.y));

//    std::uniform_real_distribution<float> v_gen(1, 1);
    std::uniform_real_distribution<float> size_gen(2, 15);
    std::uniform_real_distribution<float> alpha_gen(256, 256);
    std::uniform_real_distribution<float> x_gen(std::min(a.x, b.x), std::max(a.x, b.x));
    std::uniform_real_distribution<float> y_gen(std::min(a.y, b.y), std::max(a.y, b.y));
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
        std::array<int, 3> rgb = boostColor(particle.alpha, player);
        SDL_SetRenderDrawColor(renderer, rgb[0], rgb[1], rgb[2], 255);
        SDL_RenderFillRectF(renderer, &boostRect);
//        SDL_DestroyTexture(boostTexture);
    }
    // Reset render target to default
    SDL_SetRenderTarget(renderer, nullptr);

    // Draw texture to screen
    SDL_RenderCopy(renderer, boostTexture, nullptr, nullptr);

}
