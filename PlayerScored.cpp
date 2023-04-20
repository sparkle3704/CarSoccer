#include "PlayerScored.h"
#include <SDl2/SDL.h>
#include <string>
#include <SDL2/SDL_ttf.h>
#include "SDL_Contexts.h"
#include "Structs.h"
#include "Stats.h"
#pragma once

/// Each player's score
SDL_Texture* scoreTextureA = nullptr;
SDL_Texture* scoreTextureB = nullptr;
SDL_Surface* surfaceMessage = nullptr;
int messageTextWidth[] = {0, 0, 0};
int messageTextHeight[] = {0, 0, 0};

/// Print PLAYER SCORED
Uint64 scoredBeginTime;
SDL_Surface* scoredSurface = nullptr;
SDL_Texture* scoredTexture = nullptr;
SDL_Rect scoredRect;

/// 1 Minute Remaining
Uint64 reminderBeginTime;
SDL_Surface* reminderSurface = nullptr;
SDL_Texture* reminderTexture = nullptr;
SDL_Rect reminderRect;

/// Time
SDL_Texture* timeTexture = nullptr;
SDL_Surface* textSurface = nullptr;

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

std::string toString(int x) {
    std::string s;
    while (x != 0) {
        s = char((x % 10) + '0') + s;
        x /= 10;
    }
    if ((int)s.size() == 0) {
        s = "0";
    }
    return s;
}


void printPlayerScored(int player) {
    scoredBeginTime = SDL_GetTicks();
    std::string s;
    Font = TTF_OpenFont(fontPath.c_str(), 100); ///
    if (player == 1) {
        s = player1_name + " SCORED";
        scoredSurface = TTF_RenderText_Blended(Font, s.c_str(), BLUE); ///
    }
    else {
        s = player2_name + " SCORED";
        scoredSurface = TTF_RenderText_Blended(Font, s.c_str(), ORANGE); ///
    }

    scoredTexture = SDL_CreateTextureFromSurface(renderer, scoredSurface); ///

    int text_width = scoredSurface->w; ///
    int text_height = scoredSurface->h; ///

    vec2 center = vec2(960, 286);
    scoredRect = {center.x - text_width/2, center.y - text_height/2, text_width, text_height};

    Font = nullptr; ///
    TTF_CloseFont(Font); ///
//    SDL_FreeSurface(scoredSurface);
    scoredSurface = nullptr;
//    std::cerr << s << "\n";
}

void printReminder() {
    reminderBeginTime = SDL_GetTicks();
    std::string s;
    Font = TTF_OpenFont(fontPath.c_str(), 100); ///
    s = "1 MINUTE REMAINING";
    reminderSurface = TTF_RenderText_Blended(Font, s.c_str(), RED); ///

    reminderTexture = SDL_CreateTextureFromSurface(renderer, reminderSurface); ///

    int text_width = reminderSurface->w; ///
    int text_height = reminderSurface->h; ///

    vec2 center = vec2(960, 286);
    reminderRect = {center.x - text_width/2, center.y - text_height/2, text_width, text_height};

    Font = nullptr; ///
    TTF_CloseFont(Font); ///
//    SDL_FreeSurface(reminderSurface);
    reminderSurface = nullptr;
}

std::pair<SDL_FRect, float> scorePlayer(int player, int w, int h) {
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

void displayScore(int score, int player) {
    if ((player == 1 && score != prvScoreA) || (player == 2 && score != prvScoreB)) {
        int fontSize =  getFontSize(score);
        Font = TTF_OpenFont(fontPath.c_str(), fontSize);
        std::string s = toString(score);
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
//        SDL_FreeSurface(surfaceMessage);
    }

    std::pair<SDL_FRect, float> tmp = scorePlayer(player, messageTextWidth[player], messageTextHeight[player]);
    SDL_FRect Message_rect = tmp.first;
    if (player == 1) {
        SDL_RenderCopyExF(renderer, scoreTextureA, NULL, &Message_rect, tmp.second, NULL, SDL_FLIP_NONE);
    } else {
        SDL_RenderCopyExF(renderer, scoreTextureB, NULL, &Message_rect, tmp.second, NULL, SDL_FLIP_NONE);
    }
}

Uint64 lastTime = SDL_GetTicks();
Uint64 currentTime;
int timeElapsed = 0;
int timeLeft = 300;
bool countDown = 1;
void displayTime() { /// printReminder, Scored, Time
    currentTime = SDL_GetTicks();
    int minutes, seconds;
    if (countDown == 1) {
        if (currentTime > lastTime + 1000)
        {
            timeLeft--;
            lastTime = currentTime;
        }

        minutes = timeLeft / 60;
        seconds = timeLeft % 60;

        if (minutes == 1 && seconds == 0) {
            printReminder();
        }
    }
    else {
        if (currentTime > lastTime + 1000) {
            timeElapsed++;
            lastTime = currentTime;
        }

        minutes = timeElapsed / 60;
        seconds = timeElapsed % 60;
    }
    /// Time
    std::string timeText = toString(minutes) + ":" + (seconds <= 9 ? "0" : "") + toString(seconds); ///
    Font = TTF_OpenFont(fontPath.c_str(), 50); ///
    textSurface = TTF_RenderText_Blended(Font, timeText.c_str(), { 255, 255, 255 }); ///
    Font = nullptr; ///
    TTF_CloseFont(Font); ///

    timeTexture = SDL_CreateTextureFromSurface(renderer, textSurface); ///

    int text_width = textSurface->w; ///
    int text_height = textSurface->h; ///

    SDL_Rect renderQuad = { 962 - text_width/ 2, 24 - text_height/ 2, text_width, text_height };

    SDL_RenderCopy(renderer, timeTexture, NULL, &renderQuad);


    if (scoredTexture != nullptr && currentTime - scoredBeginTime <= 3*1000) {
        SDL_RenderCopy(renderer, scoredTexture, NULL, &scoredRect);
    }
    else {
        if (reminderTexture != nullptr && currentTime - reminderBeginTime <= 3*1000) {
            SDL_RenderCopy(renderer, reminderTexture, NULL, &reminderRect);
        }
    }
}

void displayScores() {
    displayScore(scoreA, 1);
    displayScore(scoreB, 2);

    prvScoreA = scoreA;
    prvScoreB = scoreB;
}
