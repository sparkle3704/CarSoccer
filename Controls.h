#ifndef CONTROLS_H_INCLUDED
#define CONTROLS_H_INCLUDED
#pragma once
#include <SDL2/SDL.h>
extern std::string player2_prvLeftRight;
extern std::string player1_prvLeftRight;

/// player1 controls
extern const int player1_scancode_up;
extern const int player1_scancode_down;
extern const int player1_scancode_left;
extern const int player1_scancode_right;
extern const int player1_scancode_boost;
extern const int player1_scancode_jump;

extern SDL_Keycode player1_sym_up;
extern SDL_Keycode player1_sym_down;
extern SDL_Keycode player1_sym_left;
extern SDL_Keycode player1_sym_right;
extern SDL_Keycode player1_sym_boost;
extern SDL_Keycode player1_sym_jump;
///

/// player2 controls
extern const int player2_scancode_up;
extern const int player2_scancode_down;
extern const int player2_scancode_left;
extern const int player2_scancode_right;
extern const int player2_scancode_boost;
extern const int player2_scancode_jump;

extern SDL_Keycode player2_sym_up;
extern SDL_Keycode player2_sym_down;
extern SDL_Keycode player2_sym_left;
extern SDL_Keycode player2_sym_right;
extern SDL_Keycode player2_sym_boost;
extern SDL_Keycode player2_sym_jump;
///

extern bool enable_player2;
extern bool enable_player1;

#endif // CONTROLS_H_INCLUDED
