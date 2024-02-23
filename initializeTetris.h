#include <stdlib.h>
#include <curses.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <ncurses.h>
#include <time.h>
#define box_width 20
#define box_height 7

extern int score;
extern int curentLevel;
extern double dropInterval;
extern int nextStartX;
extern int nextStartY;
extern int shapes[7][4][4];
extern int totalLinesCleared;

typedef struct {
    int shape[4][4];
    int x, y;
    int type;
    bool isActive;
}Tetromino;

Tetromino* initializeTetrominoes();
void printWraped(int startY, int screen_width, const char* text);
void drawBordersAndScore(WINDOW *win, int screen_width, int screen_height, int leftOffset);
void drawGhostPiece(WINDOW *win, Tetromino ghost, int leftOffset);
void drawHoldTetromino(WINDOW *win, Tetromino hold);
void drawGame(WINDOW *win, int **board, int BOARD_HEIGHT, int BOARD_WIDTH, int score, Tetromino curent, Tetromino *ghost, Tetromino next, int **cellValue, Tetromino hold);
void drawNextTetromino(WINDOW *win, Tetromino next);
int checkCompleteLines(int **board, int BOARD_HEIGHT, int BOARD_WIDTH);
int calculateScore(int linesCleared);
bool isMoveValid(Tetromino tetromino, int** board, int newX, int newY, int BOARD_HEIGHT, int BOARD_WIDTH);
bool handleUserInput(WINDOW *win, long long press, Tetromino *curent, int **board, int BOARD_HEIGHT, int BOARD_WIDTH, Tetromino *ghost, Tetromino *next, int **cellValue, Tetromino *hold, bool *canHold, Tetromino *tetrominoes);
int calculateBoardPosition(int mouseX, int screen_width, int BOARD_WIDTH);
void countdownStart(WINDOW *win, int screen_height, int screen_width);
void countdown(WINDOW *win, int screen_height, int screen_width);
void freeTetromino(int **board, int **cellValue, int BOARD_HEIGHT, Tetromino *tetrominoes);
