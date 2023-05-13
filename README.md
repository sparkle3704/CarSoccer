# CarSoccer
A fun and challenging game of car soccer. As the name suggests, players control their car to dribble the ball and score. This is a 2-player game, but you can play alone though :). 
# How to play

## Player 1: Controls the BLUE car
* Keyboard shortcuts:

1. Go left: A key
2. Go right: D key
3. Jump: K key
4. Boost: J key
5. Tilt downwards: W key
6. Tilt upwards: S key

## Player 2: Controls the ORANGE car
* Keyboard shortcuts:

1. Go left: ← Key
2. Go right: → key
3. Jump: \ key
4. Boost: ] key
5. Tilt downwards: ↑ key
6. Tilt upwards: ↓ key

## Key combinations:

- Holding **up** and then hitting **jump** makes the car **dodge downwards**.
- Holding **down** and then hitting **jump** makes the car **dodge upwards**.
- If the dodge is performed **forwards**, i.e. **dodge downwards**, the car will **propel forwards** in the **same direction** that it was originally moving, but with a sudden burst of additional speed. 
- If the dodge is performed **backwards**, i.e. **dodge upwards**, the car will **propel backwards** in the **opposite direction** that it was originally moving, but with a sudden burst of additional speed. 
- While dodging in one direction, players can press the key that corresponds to the **opposite direction** to prevent the car from moving. For instance, if the car is **dodging downwards**, players can hit the ← or A key to keep the car stationary. Similarly, if the car is **dodging upwards**, players can press the → or D key to keep it stationary.

# Screens

## Title Screen
![TitleScreen](https://github.com/sparkle3704/CarSoccer/assets/125803874/e6512b31-359f-440e-9c7a-735b8827bff7)

### Three buttons: **PLAY, OPTIONS, EXIT**
![OptionsWindow](https://github.com/sparkle3704/CarSoccer/assets/125803874/d6c4f108-78d2-4bb9-b7d3-a8440e0a7bef)
* **PLAY** leads to the **GamePlay Screen**.
* **OPTIONS** opens up the **Options Window** 
    * Players can change their names and some options: Unlimited Time, No Ground Mode, Sound Effects, and Music.
    * Hitting **OK** saves the settings and closes this window.
* **EXIT** exits the game.
## Gameplay Screen 
![GameplayScreen](https://github.com/sparkle3704/CarSoccer/assets/125803874/b4cca304-d9ff-45fc-af4c-579a14af42ba)

* Players' cars are on two sides, the ball is in the middle.
* **normal mode**:
    * The match's duration is 5 minutes. Players will enter overtime when the time is up and their scores are equal. The first person to score in overtime wins the match. 
    * If the time runs out but the ball is still in mid-air, the game will continue until the ball touches the ground or the goal floor. 
* **No Ground Mode**:
   *  The ball will fall through the ground. 
   * If the ball falls on the right half, i.e. player 2's player 2 will receive a point, and vice versa. If it falls perfectly in the middle, no one will receive a point. 
* **Unlimited Time Mode**:
   * The game continues forever. 

Hitting **ESC** brings up three buttons: **RESUME, OPTIONS, EXIT**
![gameplayPause](https://github.com/sparkle3704/CarSoccer/assets/125803874/c5adcc6c-f089-40c8-b0f3-4307186d3c6d)
* **Resume** resumes the game.
* **OPTIONS** opens up the **Options Window** 
* **EXIT** returns to the **Title Screen**.

![gameplayOptionsWindow](https://github.com/sparkle3704/CarSoccer/assets/125803874/7f93243c-4848-4d8c-bdc1-ecb5e60a0718)


## Victory Screen
This screen shows the winner of the match. 
![Player1_wins](https://github.com/sparkle3704/CarSoccer/assets/125803874/de3a8841-ec3f-43eb-9e89-717e30e026ad)
![Player2_wins](https://github.com/sparkle3704/CarSoccer/assets/125803874/88dc54fd-63f5-495d-a980-8c3c2eff82fb)

There are three buttons: **REPLAY, OPTIONS, EXIT**
* **REPLAY** leads to the **Gameplay Screen**.
* **Options** opens up the **Options Window**.
* **EXIT** exits the game.

# How to run the game
* Run the exe file in `bin/Debug`.

<details>
<summary>Just sharing my thoughts</summary>
<br>
&emsp; This is probably the last time I will continue coding this game. I might call it a day now. It took me so much time to try and figure out how to prevent memory leaks from SDL_Surface and SDL_Texture. Unfortunately, after so many attempts and repeatedly replacing large amounts of code, I haven't been successful. This game will be left with that issue. If there's anyone reading this who has lots of time, could you do me a favor and fix the memory leaking issue? To describe the issue, if I don't use SDL_FreeSurface, I only rarely see flickers of some texture on the screen. But if I use SDL_FreeSurface, it happens very frequently. Also, there is a very small chance that you might get the car to flip repetitively, which is of course undesirable. I will not come back to fix those issues, at least not in the meantime. Besides, I should mention that my game is not very optimized, as there is still a lot of room for improvement. 
<br/>
<br/>
&emsp; This game took me so long to code, probably because I didn’t learn all the basics of SDL before but jumped right into coding. I just picked random sources and learned bits by bits from them. I started coding this game around four months ago. I actually didn’t have this game idea at first. I started creating a pong game because I thought it would be much easier. However, looking at a time-frame of two semesters, I figured I should do something more complicated. That’s when I decided to create a 2D version of Rocket League, my favorite game that I played during my childhood. There is an official 2D version of Rocket League on Google Play or the App Store, called Rocket League Sideswipe. I thought it would be too hard to recreate in C++ with SDL libraries and thought of creating another game, but I stuck to it and made it all the way to the end. It was quite a journey. There were many days I stayed up until early morning, coding restlessly. Skill issues :)). I didn’t pay much attention to other subjects and devoted the majority of my time to this game development. And speaking of not learning all the basics of SDL in the first place, and not being aware of the importance of project management, I coded everything in one main.cpp file, that is the ‘main_ALL_IN_ONE.cpp’ (only at the early stage of the game). Looking at my friends who have multiple files in their projects, I decided I should do the same. It took me three days searching the internet for instructions and attempting to split into files. Only later did I know the advantages of splitting the original main.cpp file.
<br/>
<br/>
&emsp; The time of writing this note is 1:26 am, May 14th, 2023. My first experience of dealing with a ‘big project’. It was stressful at times when I had to debug and find out other ways to implement some functions. Looking back, I’m genuinely happy that I chose this game idea and was determined enough to finish this project. Being able to recreate my favorite game is such a proud achievement, to me at least.
<br/>
<br/>
&emsp; Lessons that I’ve learned: First, pick a trustworthy source and learn mostly from it. I really regret not learning from Lazy Foo and just searching across the internet for SDL’s functions that serve my purposes. Second, learn the general rules, such as splitting code into multiple files. This really helps with debugging, management, and future development. And third, be determined. I thought I couldn’t handle the game logic and finish this game in time. With my strong determination and devotion, I finally made it. It’s not perfect, I know, I already mentioned above, but I’m extremely proud of my work. I’m glad that I could finally distance myself from coding this game and focus on other priorities. It was an adventure, an unforgettable experience. 
<br/>
<br/>
&emsp; Some of my proposed features include showing a floating name above the player’s car, boost indicator with some refill mechanism, some other game effects,… I was focusing on the most interesting features and I missed out on those. It would be amazing if someone experienced could take this game’s concept and remake it, with much better graphics, optimization, and more features.
<br/>
<br/>
&emsp; If you’ve read this far, I really appreciate your time and patience. If you’re also creating a game as your college’s big project, it would be awesome to share your story as well! That’s all, see ya, random reader. I need some rest. Bye! 

</details>

