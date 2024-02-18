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

typedef struct {
    int shape[4][4];
    int x, y;
    int type;
}Tetromino;

Tetromino* initializeTetrominoes();
void drawBordersAndScore(WINDOW *win, int screen_width, int screen_height, int leftOffset);
void drawGhostPiece(WINDOW *win, Tetromino ghost, int leftOffset);
void drawGame(WINDOW *win, int **board, int BOARD_HEIGHT, int BOARD_WIDTH, int score, Tetromino curent, Tetromino *ghost, Tetromino next);
int checkCompleteLines(int **board, int BOARD_HEIGHT, int BOARD_WIDTH);
int calculateScore(int linesCleared);
bool isMoveValid(Tetromino tetromino, int** board, int newX, int newY, int BOARD_HEIGHT, int BOARD_WIDTH);
