# TANK TROUBLE
This is a simple game written in C++ using SDL2 development library. The game is a simple clone to an online game with the same name. Though with simpler graphics and mechanics. This is made for my Advanced Coding class.

## Description
As i said this game is based on an online game with the same name. You control your tanks around the maze in order to reach your opponent's tank and shoot'em. The goal of the game is to be the first one to shoot your opponent 5 times.
 
## Installation
For the sake of simplicity and for my sanity, you can download all the file you need in the latest [Releases](https://github.com/Vanhhoang2810/tank/releases) page.
- You may need the [Microsoft Visual C++ Redistributables](https://support.microsoft.com/en-us/topic/the-latest-supported-visual-c-downloads-2647da03-1eea-4433-9aff-95f26a218cc0) if you don't have them already installed.
- If you want to try the game, after download the files and extract it, go to tank/bin/debug, there should be Tank.exe with all of the necessary dll files.

## Control
|Control|Player 1 (green tank)|Player 2 (red tank)|
|:-----:|:-------------------:|:-------------------:|
|Forward|W|UP ARROW|
|Reverse|S|DOWN ARROW|
|Left|A|LEFT ARROW|
|Right|D|RIGHT ARROW|
|Shoot|SPACE|NUMPAD 1|

## List of features
- Create sprites for all objects
- Check and handle collision for tanks, bullets, walls and the boundaries of the screen
- Handle movement, tank rotations
- Scores tracking and display text
- Create win screen, where it will display who win
- Create a maze for the tank to traverse
- Create a shooting mechanic

## Features to add
- A replay mode
- Single player mode with a decent AI
- More maps, with the options to pick a map
- Power ups
- The ability to move and shoot diagonally
- A better gameState 

## Things that i wish i have time to fix
- The wall is a little thicker than my expectation
- The collision between the tanks are a little weird, since the red tank can push the green tank
- My idiotic friend
  
## Acknowledgements and Credits
[Lazy Foo's SDL Tutorials](https://lazyfoo.net/tutorials/SDL/index.php) have been very helpful and is one of the best, though a bit old, way to get in to SDL development in specific, and game development in general.

[pbsinclair42's version](https://github.com/pbsinclair42/TankTrouble) was my inspiration and source for my PNG sprites

[Peepo](https://www.dafont.com/peepo.font) is the font i choose to use, since it looked kinda silly

And of course [SDL 2.29.2](https://github.com/libsdl-org/SDL/releases/tag/prerelease-2.29.2), [SDL_image 2.8.2](https://github.com/libsdl-org/SDL_image/releases/tag/release-2.8.2),  [SDL_ttf 2.22.0](https://github.com/libsdl-org/SDL_ttf/releases/tag/release-2.22.0) are the core of the game.

