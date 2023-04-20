#ifndef SOUNDS_H_INCLUDED
#define SOUNDS_H_INCLUDED
#include <SDL2/SDL_mixer.h>

extern bool mute;
extern int explosionChannel;
extern int ballHitChannel;
extern int boostStartChannel[3];
extern int boostEndChannel[3];

extern Mix_Chunk* explosionSound;
extern Mix_Chunk* ballHitSound;
extern Mix_Chunk* boostStartSound;
extern Mix_Chunk* boostEndSound;

void playEffect(Mix_Chunk* effectSound, int channel);

void stopEffect(int channel);

void playEffectOnce(Mix_Chunk* effectSound, int channel);

#endif // SOUNDS_H_INCLUDED
