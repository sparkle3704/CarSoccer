#include "Sounds.h"

bool mute = 0;
int explosionChannel = 0;
int ballHitChannel = 1;
int boostStartChannel[3] = {-1, 2, 3};
int boostEndChannel[3] = {-1, 4, 5};
int overtimeChannel = 6;

Mix_Chunk* explosionSound = nullptr;
Mix_Chunk* ballHitSound = nullptr;
Mix_Chunk* boostStartSound = nullptr;
Mix_Chunk* boostEndSound = nullptr;
Mix_Chunk* overtimeSound = nullptr;

void playEffect(Mix_Chunk* effectSound, int channel) {
    if (mute) {
        return;
    }
    Mix_PlayChannel(channel, effectSound, -1);
}

void stopEffect(int channel) {
    if (mute) {
        return;
    }
    Mix_HaltChannel(channel);
}

void playEffectOnce(Mix_Chunk* effectSound, int channel) {
    if (mute) {
        return;
    }
    if (Mix_Playing(channel)) {
        stopEffect(channel);
    }
    Mix_PlayChannel(channel, effectSound, 0);
}
