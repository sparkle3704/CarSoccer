#ifndef SOUNDS_H_INCLUDED
#define SOUNDS_H_INCLUDED
#pragma once
#include <SDL2/SDL_mixer.h>

extern bool muteSFX;
extern bool muteMusic;
extern int explosionChannel;
extern int ballHitChannel;
extern int boostStartChannel[3];
extern int boostEndChannel[3];
extern int overtimeChannel;
extern int victoryChannel;
extern int clickChannel;

extern Mix_Chunk* explosionSound;
extern Mix_Chunk* ballHitSound;
extern Mix_Chunk* boostStartSound;
extern Mix_Chunk* boostEndSound;
extern Mix_Chunk* overtimeSound;
extern Mix_Chunk* victorySound;
extern Mix_Chunk* clickSound;

extern Mix_Music* music;
extern Mix_Music* musicTitle;
extern Mix_Music* musicGameplay;
extern Mix_Music* musicVictory;
extern Mix_Music* musicVictory0;

void playEffect(Mix_Chunk* effectSound, int channel);

void stopEffect(int channel);

void playEffectOnce(Mix_Chunk* effectSound, int channel);

void playMusic();
#endif // SOUNDS_H_INCLUDED
