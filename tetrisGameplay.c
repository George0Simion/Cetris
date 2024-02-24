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
    /* Simple rotation */
}

void lockTetrominoAndUpdateBoard(WINDOW *win ,Tetromino *curent, int **board, int BOARD_HEIGHT, int BOARD_WIDTH, int *score, int **cellValue, bool *canHold) {
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

    int linesCleared = checkCompleteLines(board, BOARD_HEIGHT, BOARD_WIDTH);
    *score += calculateScore(linesCleared); // Update score based on the lines cleared

    totalLinesCleared += linesCleared;

    // Message on the screen if lines cleared
    if (linesCleared != 0) {
        char message[8];
        int message_score = calculateScore(linesCleared);
        switch (linesCleared) {
            case 1:
                strcpy(message, "Single");
                break;
            case 2:
                strcpy(message, "Double");
                break;
            case 3:
                strcpy(message, "Triple");
                break;
            case 4:
                strcpy(message, "Tetris");
                break;
            default:
                break;    
        }

        int msj_len = strlen(message);
        int score_len = snprintf(NULL, 0, "+%d", message_score);
        int leftOffset = (getmaxx(win) - BOARD_WIDTH) / 2;
        int x = BOARD_WIDTH / 2 - msj_len / 2 + leftOffset;
        int y = curent->y;

        attron(COLOR_PAIR(9));
        mvwprintw(win, y, x, "%s", message);
        mvwprintw(win, y+1, x, "+%d", message_score);
        attroff(COLOR_PAIR(9));
        wrefresh(win);
        usleep(200000);
    }
    
    // Level logic
    if (totalLinesCleared / 10 > curentLevel - 1) {
        curentLevel = totalLinesCleared / 10 + 1;
    }

    dropInterval = 0.5 - (0.15 *(curentLevel - 1));
    if (dropInterval < 0.1) {
        dropInterval = 0.1;
    }

    *canHold = true;
}

void hardDropTetromino(Tetromino *curent, int **board, int BOARD_HEIGHT, int BOARD_WIDTH) {
    int dropDistance = 0;
    while (isMoveValid(*curent, board, curent->x, curent->y + dropDistance + 1, BOARD_HEIGHT, BOARD_WIDTH)) {
        // Calculating drop distance
        dropDistance++;
    }

    // Score and y updates
    score += 2 * dropDistance;
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
    next->y = (box_height - 2) / 2 - 1;

    next->x += nextStartX;
    next->y += nextStartY;

    // Check if the new tetromino can be placed without colliding
    return isMoveValid(*current, board, current->x, current->y, BOARD_HEIGHT, BOARD_WIDTH);
}
