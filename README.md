# CarSoccer
A fun and challenging game of car soccer. As the name suggests, players control their car to dribble the ball and score. This is a 2-player game, but you can play alone though :). 
# How to play

## Player 1: Controls the BLUE car.
* Keyboard shortcuts:

1. Go left: A key
2. Go right: D key
3. Jump: K key
4. Boost: J key
5. Tilt down: W key
6. Tilt up: S key

## Player 2: Controls the ORANGE car.
* Keyboard shortcuts:

1. Go left:    ← Key
2. Go right:  → key
3. Jump:         \ key
4. Boost:         ] key
5. Tilt down: ↑ key
6. Tilt up:       ↓key

## Key combinations:

- Holding **up** and then hitting **jump** makes the car **dodge down**
- Holding **down** and then hitting **jump** makes the car **dodge up**
- If the dodge is performed **forward**, i.e **dodge down**, the car will **propel forward** in the **same direction** that it was originally moving, but with a sudden burst of additional speed. 
- If the dodge is performed **backwards**, i.e **dodge up**, the car will **propel backward** in the **opposite direction** that it was originally moving, but with a sudden burst of additional speed. 
- While dodging in the **forward** direction, players can hit the key corresponding to the **opposite direction**, for example,  ← or A key when the car is **dodging down**, and the car will stay in place. Similarly, players can hit  → or D key while the car is **dodging up** to make the it stay in place.

# Screens

## Title Screen
![TitleScreen](https://github.com/sparkle3704/CarSoccer/assets/125803874/e6512b31-359f-440e-9c7a-735b8827bff7)

### Three buttons: **PLAY, OPTIONS, EXIT**
![OptionsWindow](https://github.com/sparkle3704/CarSoccer/assets/125803874/d6c4f108-78d2-4bb9-b7d3-a8440e0a7bef)
* **PLAY** leads to the **GamePlay Screen**.
* **OPTIONS** opens up the **Options Window** 
    * Players can change their names and some options: Unlimited Time, No Ground Mode, Sound Effects, Music.
    * Hitting **OK** saves the settings and closes this window.
* **EXIT** exits the game.
## Gameplay Screen 
![GameplayScreen](https://github.com/sparkle3704/CarSoccer/assets/125803874/b4cca304-d9ff-45fc-af4c-579a14af42ba)

* Players' cars are on two sides, the ball is in the middle.
* **normal mode**:
    * The match's duration is 5 minutes. Players will enter overtime when the time is up and their score are equal. The first person to score in overtime wins the match. 
    * If the time runs out but the ball is still in mid-air, the game will continue until the ball touches the ground or the goal floor. 
* **No Ground Mode**:
   *  The ball will fall through the ground. 
   * If it falls on the right half, i.e player 2's player 2 will receive a point, and vice versa. If it falls perfectly in the middle, no one will receive a point. 
* **Unlimited Time Mode**:
   * The game continues forever. 

Hitting **ESC** brings up three buttons: **RESUME, OPTIONS, EXIT**
![gameplayPause](https://github.com/sparkle3704/CarSoccer/assets/125803874/c5adcc6c-f089-40c8-b0f3-4307186d3c6d)
![gameplayOptionsWindow](https://github.com/sparkle3704/CarSoccer/assets/125803874/04c73b74-9d50-43fc-b856-8e17de1fb037)

## Victory Screen
This screen shows the winner of the match. 
![Player1_wins](https://github.com/sparkle3704/CarSoccer/assets/125803874/de3a8841-ec3f-43eb-9e89-717e30e026ad)
![Player2_wins](https://github.com/sparkle3704/CarSoccer/assets/125803874/88dc54fd-63f5-495d-a980-8c3c2eff82fb)

There are three buttons: **REPLAY, OPTIONS, EXIT**
* **REPLAY** leads to the **Gameplay Screen**
* **Options** opens up the **Options Window**
* **EXIT** exits the game.


# CarSoccer

Done: 2 players, boost, ball can spin, goal explosions, game background, time, score, sound effects, 1 min reminder, separated main.cpp into multiple source files and header files, added title screen, gameplay, options window. Options window now have names assignment, 4 options with 4 tickboxes. Player can enter game by clicking the play button, and midgame, player can pause to reveal the pause menu by hitting esc. There will be 3 buttons, namely resume, options and exit (will change to 'menu'). Hitting the resume button resumes the game, hitting options reveals the options window, where player can change settings which apply immediately, there is a new mode called 'No Ground Mode', where the ball will not bounce of the ground and instead, if the ball falls onto the player1's half, player 2 will receive a point and vice versa, and if the ball falls perfectly in the middle, nothing will happen. Hitting esc will open the previously closed window (midgame), meaning closing the options window will reveal the pause menu, and hitting esc again will unpause the game and players can resume playing.

Bug: unnatural dodges

Todo: Add other features: add more sound effects, music, gamemodes (maybe not), other effects (idk), handle overtime and showing players' score after the game.
