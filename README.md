# Cetris - A Classic Tetris Experience

  Cetris is a modern take on the classic Tetris game, implemented in C with ncurses for terminal-based UI. It combines the beloved traditional gameplay with added mouse control for a refreshing experience. Dive into the world of falling tetrominos, clear lines, and challenge yourself with increasing levels of difficulty!

<p float="left">
  <img src="https://github.com/George0Simion/Cetris/assets/149818276/1bb765a0-8752-409d-9065-cea7ce229c72" width="400" />&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
  <img src="https://github.com/George0Simion/Cetris---A-Tetris-Clone/assets/149818276/dc5c744c-4503-4cdc-9f2b-2fe1ab06f174" width="400" /> 
</p>


# Features

 - Intuitive Controls: Use W, A, S, D or arrow keys for moving tetrominos left, right, and down, and for rotating them.
 - Rotation and Hard Drop: Rotate your tetrominos with a tap and hard drop them with space for a faster pace.
 - Hold Feature: Save a tetromino for later use, giving you the strategic edge.
 - Scoring System: Score points just like in the original Tetris game. The more lines you clear, the higher your score!
 - Pause, Restart, and Resume: Take a break when needed, start fresh, or pick up right where you left off with ease.
 - Increasing Difficulty: The game becomes faster as you clear more lines, testing your reflexes and strategy.
 - Mouse Logic: Right-click and drag to position, release to hard drop, and left-click to rotate for precision control.
 - Line Clear Display: Watch the screen flash as you clear lines, adding to the satisfaction of your success.

# How to Play
   ### Installation

First, make sure you have ncurses installed on your system. On most Linux distributions, you can install ncurses using the package manager. For example, on Ubuntu:
 - `sudo apt-get install libncurses5-dev libncursesw5-dev`

Next, clone the repository to your local machine:
 - `git clone https://github.com/George0Simion/Cetris`
 - `cd Cetris`

To compile and run the game, use the provided Makefile:
 - `make run-tetris`

  ### Gameplay

- Movement: Navigate the falling tetrominos using W, A, S, D or the arrow keys.
- Rotate: Press W or the up arrow to rotate the tetromino clockwise.
- Hard Drop: Press the spacebar to instantly drop the tetromino to the bottom of the play area.
- Hold: Press C to hold a tetromino for later use.
- Pause/Resume: Press P to pause the game. Press again to resume.
- Restart: Press P to pause, then R to restart the game.
- Quit: Press Esc to quit the game at any time.

### Mouse Control

 - Positioning: Right-click and drag the tetromino to move it to the desired column.
 - Hard Drop: Release the right-click to hard drop the tetromino in place.
 - Rotate: Left-click to rotate the tetromino.
