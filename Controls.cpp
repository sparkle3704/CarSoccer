#include <string>
#include "Controls.h"

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
SDL_Keycode player1_sym_pause = SDLK_ESCAPE;


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
SDL_Keycode player2_sym_pause = SDLK_ESCAPE;
///

bool enable_player2 = 1; // red
bool enable_player1 = 1; // blue
