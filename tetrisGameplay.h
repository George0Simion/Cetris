#include "initializeTetris.h"

void rotateTetromino(int shape[4][4]);
void lockTetrominoAndUpdateBoard(Tetromino *curent, int **board, int BOARD_HEIGHT, int BOARD_WIDTH, int *score);
void hardDropTetromino(Tetromino *curent, int **board, int BOARD_HEIGHT, int BOARD_WIDTH);
void calculateGhostPosition(Tetromino *current, int **board, int BOARD_HEIGHT, int BOARD_WIDTH, Tetromino *ghost);
bool spawnNewTetromino(Tetromino *current, Tetromino tetrominoes[], int **board, int BOARD_HEIGHT, int BOARD_WIDTH, Tetromino *next);