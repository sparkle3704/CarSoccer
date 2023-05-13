#include "PlayerScored.h"
#include <SDl2/SDL.h>
#include <string>
#include <SDL2/SDL_ttf.h>
#include "SDL_Contexts.h"
#include "Structs.h"
#include "Stats.h"
#include "Ball.h"
#include "Screens.h"
#include "Explosion.h"
#include "Sounds.h"
#include "utils.h"

/// Each player's score
SmartText smartScoreA;
SmartText smartScoreB;

/// Print PLAYER SCORED
SmartText smartScored;
LTimer timerScored;

/// 1 Minute Remaining
SmartText smartReminder;
LTimer timerReminder;

/// OVERTIME!
SmartText smartOvertime;
LTimer timerOvertime;

/// Time
SmartText smartTime;

int getFontSize(int score) {
    if (score < 0) {
        score = -score * 10;
    }
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
    bool neg = x < 0;
    x = abs(x);
    while (x != 0) {
        s = char((x % 10) + '0') + s;
        x /= 10;
    }
    if ((int)s.size() == 0) {
        s = "0";
    }
    if (neg) {
        s = "-" + s;
    }
    return s;
}

bool printPlayerScored(int player) { /// 960, 286
    timerScored.start();
    TTF_Font* Font = getFont(100);
    if (player == 1) {
        std::string s = player1_name + " SCORED";
        if (!smartScored.loadFromRenderedText(Font, s, BLUE)) {
            return 0;
        }
    }
    else {
        std::string s = player2_name + " SCORED";
        if (!smartScored.loadFromRenderedText(Font, s, ORANGE)) {
            return 0;
        }
    }
    return 1;
}

bool printReminder() { /// 960, 286
    timerReminder.start();
    TTF_Font* Font = getFont(100);
    std::string s = "1 MINUTE REMAINING";
    if (!smartReminder.loadFromRenderedText(Font, s, RED)) {
        return 0;
    }
    return 1;
}

bool printOvertime() { /// 960, 286
    timerOvertime.start();
    TTF_Font* Font = getFont(100);
    std::string s = "OVERTIME";
    if (!smartOvertime.loadFromRenderedText(Font, s, RED)) {
        return 0;
    }
    return 1;
}

bool displayScore(int score, int player) {
    if ((player == 1 && score != prvScoreA) || (player == 2 && score != prvScoreB)) {
        int fontSize =  getFontSize(score);
        TTF_Font* Font = getFont(fontSize);
        std::string s = toString(score);
        if (player == 1) {
            if (!smartScoreA.loadFromRenderedText(Font, s, WHITE)) {
                return 0;
            }
        } else {
            if (!smartScoreB.loadFromRenderedText(Font, s, WHITE)) {
                return 0;
            }
        }
    }
    if (player == 1) {
        smartScoreA.render(870, 28);
    } else {
        smartScoreB.render(1049, 28);
    }
    return 1;
}

Uint64 lastTime = SDL_GetTicks();
Uint64 currentTime;
int timeElapsed = 0;
int timeLeft = 300;
bool countDown = 1;
bool displayingScored = 0;
bool displayingOvertime = 0;
bool timer = 1;
int minutes, seconds;

LTimer timerExtraExplosion;

void addExtraExplosion() {
    timerExtraExplosion.start();
}

void displayExtraExplosion() {
    if (timerExtraExplosion.getTicks() <= 500) {
        addExplosion(50, 0, WINDOW_HEIGHT*0.5, 1);
        addExplosion(50, WINDOW_WIDTH, WINDOW_HEIGHT*0.5, 2);
        updateExplosion(1);
        renderExplosion(1);
        updateExplosion(2);
        renderExplosion(2);
    }
    else {
        timerExtraExplosion.stop();
    }
}

void setBallMiddle() {
    ball.resetBall();
    ball.xPos = WINDOW_WIDTH*0.5 - ball.radius;
    ball.yPos = groundY - 2*ball.radius;
}

void displayTime() { /// printReminder, Scored, Time
    currentTime = SDL_GetTicks();
    if (timer) {
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
    }

    if (unlTimeMode == 0) {
        if (timeLeft <= 0 && countDown && !inOvertime) {
            if (scoreA != scoreB && noGroundMode) {
                ball.inMidAir = 0;
            }
            if (ball.inMidAir) {
                timeLeft = 0;
                minutes = 0, seconds = 0;
            }
            else {
                if (scoreA != scoreB) {
                    timer = 0;
                    if (noGroundMode) {
                        if (timerScored.getTicks() > 3500) {
                            gameplay.onGameplay = 0;
                            currentState = VICTORY;
                        }
                        if (timerScored.getTicks() <= 20) {
                            if (music != musicVictory) {
                                music = musicVictory;
                                Mix_HaltMusic();
                            }
                        }

                        if (timerScored.getTicks() <= 500) {
                            addExtraExplosion();
                        }
                    }
                    else {
                        if (timerScored.getTicks() > 3500) {
                            gameplay.onGameplay = 0;
                            currentState = VICTORY;
                        }
                        if (timerScored.getTicks() <= 20) {
                            setMusic(musicVictory);
                        }
                        if (timerScored.getTicks() <= 500) {
                            addExtraExplosion();
                        }
                    }
                }
                else {
                    if (noGroundMode == 0 || ball.xPos + ball.radius != WINDOW_WIDTH*0.5) {
                        printOvertime();
                        playEffectOnce(overtimeSound, overtimeChannel);
                        reset(1);
                        countDown = 0;
                        inOvertime = 1;
                    }

                }
            }
        }
        if (inOvertime && scoreA != scoreB) {
            timer = 0;
            setBallMiddle();
            if (noGroundMode) {
                if (timerScored.getTicks() <= 20) {
                    setMusic(musicVictory);
                }
            }
            else {
                setMusic(musicVictory);
            }
            if (timerScored.getTicks() <= 500) {
                addExtraExplosion();
            }
            if (timerScored.getTicks() > 3500) {
                gameplay.onGameplay = 0;
                currentState = VICTORY;
            }
        }
    }
    displayExtraExplosion();
    if (timeLeft <= 0 && timer && countDown) {
        timeLeft = 0;
        minutes = 0;
        seconds = 0;
    }

    std::string timeText = toString(minutes) + ":" + (seconds <= 9 ? "0" : "") + toString(seconds); ///
    TTF_Font* Font = getFont(50);
    if (!smartTime.loadFromRenderedText(Font, timeText, (minutes == 0 && seconds == 0 && countDown) ? RED : WHITE)) {
        return;
    }
    smartTime.render(962, 24);

    if (smartScored.valid() && timerScored.getTicks() <= 3*1000) {
        displayingScored = 1;
        smartScored.render(960, 286);
    }
    else {
        displayingScored = 0;
        if (smartReminder.valid() && timerReminder.getTicks() <= 3*1000) {
            smartReminder.render(960, 286);

        }
        if (smartOvertime.valid() && timerOvertime.getTicks() <= 3*1000) {
            displayingOvertime = 1;
            smartOvertime.render(960, 286);
        }
        else {
            displayingOvertime = 0;
        }
    }
}

bool displayScores() {
    if (!(displayScore(scoreA, 1) && displayScore(scoreB, 2))) {
        return 0;
    }
    prvScoreA = scoreA;
    prvScoreB = scoreB;

    return 1;
}

Uint64 currentInMidAir = -1;
Uint64 startInMidAir = -1;
SmartText smartInMidAir;
SmartText smartInMidAirOutline;
std::string inMidAirText = "0:00";

void displayInMidAir(Ball& ball) {
    SDL_Color textColor = WHITE;
    if (ball.inMidAir || displayingScored) {
        int minutes, seconds, elapsed;
        if (ball.inMidAir) {
            if (startInMidAir == -1) {
                startInMidAir = SDL_GetTicks();
            }
            currentInMidAir = SDL_GetTicks();
            elapsed = (currentInMidAir - startInMidAir) / 1000;
            minutes = elapsed / 60;
            seconds = elapsed % 60;
        }
        /// Time
        if (!displayingScored) {
            inMidAirText = toString(minutes) + ":" + (seconds <= 9 ? "0" : "") + toString(seconds); ///
        }
        else {
            textColor = GREEN;
            startInMidAir = -1;
        }
        if (elapsed != 0 || displayingScored) {
            if (displayingScored) {
                TTF_Font* outlineFont = getFont(25, 1);
                if (!smartInMidAirOutline.loadFromRenderedText(outlineFont, inMidAirText, BLACK)) {
                    return;
                }
                smartInMidAirOutline.render(960, 90);
            }
            TTF_Font* Font = getFont(25);
            if (!smartInMidAir.loadFromRenderedText(Font, inMidAirText, textColor)) {
                return;
            }
            smartInMidAir.render(960, 90);
        }
    }
    else {
        startInMidAir = -1;
    }
}
