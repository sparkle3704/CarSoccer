#include "Sounds.h"
#include "Screens.h"

bool muteSFX = 0;
bool muteMusic = 0;
int explosionChannel = 0;
int ballHitChannel = 1;
int boostStartChannel[3] = {-1, 2, 3};
int boostEndChannel[3] = {-1, 4, 5};
int overtimeChannel = 6;
int victoryChannel = 7;
int clickChannel = 2;

Mix_Chunk* explosionSound = nullptr;
Mix_Chunk* ballHitSound = nullptr;
Mix_Chunk* boostStartSound = nullptr;
Mix_Chunk* boostEndSound = nullptr;
Mix_Chunk* overtimeSound = nullptr;
Mix_Chunk* victorySound = nullptr;
Mix_Chunk* clickSound = nullptr;

Mix_Music* music = nullptr;
Mix_Music* musicTitle = nullptr;
Mix_Music* musicGameplay = nullptr;
Mix_Music* musicVictory = nullptr;
Mix_Music* musicVictory0 = nullptr;


void playEffect(Mix_Chunk* effectSound, int channel) {
    if (muteSFX) {
        return;
    }
    Mix_PlayChannel(channel, effectSound, -1);
}

void stopEffect(int channel) {
    if (muteSFX) {
        return;
    }
    Mix_HaltChannel(channel);
}

void playEffectOnce(Mix_Chunk* effectSound, int channel) {
    if (muteSFX) {
        return;
    }
    if (Mix_Playing(channel)) {
        stopEffect(channel);
    }
    Mix_PlayChannel(channel, effectSound, 0);
}

void playMusic() {
    if (muteMusic) {
        if (Mix_PausedMusic() == 0) {
            Mix_PauseMusic();
        }
    }
    else {
        if (Mix_PausedMusic() == 1) {
            Mix_ResumeMusic();
        }
        else {
            if (Mix_PlayingMusic() == 0) {
                Mix_PlayMusic(music, -1);
            }
        }
    }
}

void setMusic(Mix_Music* newMusic) {
    if (music != newMusic) {
        music = newMusic;
        Mix_HaltMusic();
    }
}
