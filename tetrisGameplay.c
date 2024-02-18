#include "tetrisGameplay.h"

void rotateTetromino(int shape[4][4]) {
    int newTetromino[4][4];
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            newTetromino[3 - j][i] = shape[i][j];
        }
    }

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            shape[i][j] = newTetromino[i][j];
        }
    }
}

void lockTetrominoAndUpdateBoard(Tetromino *curent, int **board, int BOARD_HEIGHT, int BOARD_WIDTH, int *score, int **cellValue) {
    // Lock the current tetromino on the board
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (curent->shape[i][j] == 1) {
                int y = curent->y + i;
                int x = curent->x + j;
                if (y >= 0 && y < BOARD_HEIGHT && x >= 0 && x < BOARD_WIDTH) {
                    board[y][x] = 1; // Mark the cell as filled
                    cellValue[y][x] = curent->type;
                }
            }
        }
    }

    static int totalLinesCleared = 0;

    int linesCleared = checkCompleteLines(board, BOARD_HEIGHT, BOARD_WIDTH);
    *score += calculateScore(linesCleared); // Update score based on the lines cleared

    totalLinesCleared += linesCleared;

    if (totalLinesCleared / 10 > curentLevel - 1) {
        curentLevel = totalLinesCleared / 10 + 1;
    }

    dropInterval = 0.5 - (0.15 *(curentLevel - 1));
    if (dropInterval < 0.1) {
        dropInterval = 0.1;
    }
}

void hardDropTetromino(Tetromino *curent, int **board, int BOARD_HEIGHT, int BOARD_WIDTH) {
    int dropDistance = 0;
    while (isMoveValid(*curent, board, curent->x, curent->y + dropDistance + 1, BOARD_HEIGHT, BOARD_WIDTH)) {
        dropDistance++;
    }

    curent->y += dropDistance;
}

void calculateGhostPosition(Tetromino *current, int **board, int BOARD_HEIGHT, int BOARD_WIDTH, Tetromino *ghost) {
    *ghost = *current; // Copy the current tetromino to the ghost

    // Find the maximum distance the tetromino can move down without collision
    int dropDistance = 0;
    while (isMoveValid(*ghost, board, ghost->x, ghost->y + dropDistance + 1, BOARD_HEIGHT, BOARD_WIDTH)) {
        dropDistance++;
    }

    // Update the ghost's position
    ghost->y += dropDistance;
}

bool spawnNewTetromino(Tetromino *current, Tetromino tetrominoes[], int **board, int BOARD_HEIGHT, int BOARD_WIDTH, Tetromino *next) {
    *current = *next;
    // Set the starting position of the new tetromino
    current->x = BOARD_WIDTH / 2 - 2; // Center the tetromino horizontally
    current->y = 0; // Start at the top of the board
    
    // Generate a random index for the next tetromino
    int randomIndex = rand() % 7;
    *next = tetrominoes[randomIndex];
    next->x = (box_width - 4) / 2;
    next->y = (box_height - 2) / 2;

    next->x += nextStartX;
    next->y += nextStartY;

    // Check if the new tetromino can be placed without colliding
    return isMoveValid(*current, board, current->x, current->y, BOARD_HEIGHT, BOARD_WIDTH);
}
