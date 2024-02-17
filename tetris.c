#include <stdlib.h>
#include <curses.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <ncurses.h>
#include <time.h>

typedef struct {
    int shape[4][4];
    int x, y;
}Tetromino;

Tetromino* initializeTetrominoes() {
    Tetromino *tetrominoes = malloc(7 * sizeof(Tetromino));
    if (tetrominoes == NULL) {
        return NULL;
    }

    int shapes[7][4][4] = {
        { // I shape
            {0, 0, 0, 0},
            {1, 1, 1, 1},
            {0, 0, 0, 0},
            {0, 0, 0, 0}
        },
        { // O shape
            {1, 1, 0, 0},
            {1, 1, 0, 0},
            {0, 0, 0, 0},
            {0, 0, 0, 0}
        },
        { // T shape
            {0, 1, 0, 0},
            {1, 1, 1, 0},
            {0, 0, 0, 0},
            {0, 0, 0, 0}
        },
        { // S shape
            {0, 1, 1, 0},
            {1, 1, 0, 0},
            {0, 0, 0, 0},
            {0, 0, 0, 0}
        },
        { // Z shape
            {1, 1, 0, 0},
            {0, 1, 1, 0},
            {0, 0, 0, 0},
            {0, 0, 0, 0}
        },
        { // L shape
            {0, 1, 0, 0},
            {0, 1, 0, 0},
            {0, 1, 0, 0},
            {0, 1, 1, 0}
        },
        { // L reversed shape
            {0, 1, 0, 0},
            {0, 1, 0, 0},
            {0, 1, 0, 0},
            {1, 1, 0, 0}
        }
    };

    for (int i = 0; i < 7; i++) {
        memcpy(tetrominoes[i].shape, shapes[i], sizeof(shapes[i]));
        tetrominoes[i].x = 0; // Set initial x position
        tetrominoes[i].y = 0; // Set initial y position
    }

    return tetrominoes;
}

int score  = 0;
int curentLevel = 1;
double dropInterval = 0.5;

int checkCompleteLines(int **board, int BOARD_HEIGHT, int BOARD_WIDTH) {
    int completeLines = 0;
    for (int i = 0; i < BOARD_HEIGHT; i++) {
        int rowFilled = 1; // Assume the row is filled initially
        for (int j = 0; j < BOARD_WIDTH; j++) {
            if (board[i][j] == 0) {
                rowFilled = 0; // Found an empty cell, row is not filled
                break;
            }
        }
        if (rowFilled) {
            completeLines++;
            for (int k = 0; k < BOARD_WIDTH; k++) {
                board[i][k] = 0; // Clear the row by setting all cells to 0
            }
            // Move down all rows above the cleared row
            for (int m = i; m > 0; m--) {
                for (int n = 0; n < BOARD_WIDTH; n++) {
                    board[m][n] = board[m-1][n];
                }
            }
            // Make sure the top row is set to empty after moving everything down
            for (int n = 0; n < BOARD_WIDTH; n++) {
                board[0][n] = 0;
            }
            i--; // Check the same row index again as it now contains the previous row
        }
    }
    return completeLines;
}

int calculateScore(int linesCleared) {
    int baseScore;
    switch (linesCleared) {
        case 1:
            baseScore = 40; // Single line
            break;
        case 2:
            baseScore = 100; // Double line
            break;
        case 3:
            baseScore = 300; // Triple line
            break;
        case 4:
            baseScore = 1200; // Tetris
            break;
        default:
            return 0; // No lines cleared, or adjust as needed for your game's logic
    }

    return baseScore * curentLevel;
}

void drawBordersAndScore(WINDOW *win, int screen_width, int screen_height) {
    erase();

    // Top border and score
    char score_text[60];
    sprintf(score_text, "  SCORE: %d  LEVEL: %d  ", score, curentLevel);
    int score_pos = (screen_width / 2) - (strlen(score_text) / 2);
    mvaddstr(0, score_pos, score_text);
    mvaddch(0, 0, ACS_ULCORNER);
    for (int x = 1; x < screen_width - 1; x++) {
        if (x < score_pos || x >= score_pos + strlen(score_text)) {
            mvaddch(0, x, ACS_HLINE);
        }
    }
    mvaddch(0, screen_width - 1, ACS_URCORNER);

    // Side borders
    for (int y = 1; y < screen_height - 1; y++) {
        mvaddch(y, 0, ACS_VLINE);
        mvaddch(y, screen_width - 1, ACS_VLINE);
    }

    // Bottom border
    mvaddch(screen_height - 1, 0, ACS_LLCORNER);
    for (int x = 1; x < screen_width - 1; x++) {
        mvaddch(screen_height - 1, x, ACS_HLINE);
    }
    mvaddch(screen_height - 1, screen_width - 1, ACS_LRCORNER);
}

bool isMoveValid(Tetromino tetromino, int** board, int newX, int newY, int BOARD_HEIGHT, int BOARD_WIDTH) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (tetromino.shape[i][j] != 0) {
                int proposedX = newX + j;
                int proposedY = newY + i;
                // Check boundaries and collision with existing blocks
                if (proposedX < 0 || proposedX >= BOARD_WIDTH || proposedY >= BOARD_HEIGHT || (proposedY >= 0 && board[proposedY][proposedX] != 0)) {
                    return false;
                }
            }
        }
    }
    return true;
}

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

void lockTetrominoAndUpdateBoard(Tetromino *curent, int **board, int BOARD_HEIGHT, int BOARD_WIDTH, int *score) {
    // Lock the current tetromino on the board
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (curent->shape[i][j] == 1) {
                int y = curent->y + i;
                int x = curent->x + j;
                if (y >= 0 && y < BOARD_HEIGHT && x >= 0 && x < BOARD_WIDTH) {
                    board[y][x] = 1; // Mark the cell as filled
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

void drawGhostPiece(WINDOW *win, Tetromino ghost) {
    wattron(win, A_DIM); // Make the ghost less visible

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (ghost.shape[i][j] == 1) {
                int y = ghost.y + i + 1;
                int x = ghost.x + j + 1;

                mvwaddch(win, y, x, ACS_CKBOARD);
            }
        }
    }
    wattroff(win, A_DIM); // Turn off the dim attribute
}

bool spawnNewTetromino(Tetromino *current, Tetromino tetrominoes[], int **board, int BOARD_HEIGHT, int BOARD_WIDTH) {
    // Generate a random index for the next tetromino
    int randomIndex = rand() % 7;
    *current = tetrominoes[randomIndex];

    // Set the starting position of the new tetromino
    current->x = BOARD_WIDTH / 2 - 2; // Center the tetromino horizontally
    current->y = 0; // Start at the top of the board

    // Check if the new tetromino can be placed without colliding
    return isMoveValid(*current, board, current->x, current->y, BOARD_HEIGHT, BOARD_WIDTH);
}

bool handleUserInput(int press, Tetromino *curent, int **board, int BOARD_HEIGHT, int BOARD_WIDTH, Tetromino *ghost) {
    int dirX = 0, dirY = 0;

    switch (press) {
        case KEY_LEFT:
            dirX = -1;
            break;
        case KEY_RIGHT:
            dirX = 1;
            break;
        case KEY_DOWN:
            dirY = 1;
            break;
        case KEY_UP:
            rotateTetromino(curent->shape); // Rotate
            calculateGhostPosition(curent, board, BOARD_HEIGHT, BOARD_WIDTH, ghost);


            if (!isMoveValid(*curent, board, curent->x, curent->y, BOARD_HEIGHT, BOARD_WIDTH)) {
                // If rotation is not valid, rotate back to original position
                rotateTetromino(curent->shape); // Rotate 3 more times to revert
                rotateTetromino(curent->shape);
                rotateTetromino(curent->shape);
                calculateGhostPosition(curent, board, BOARD_HEIGHT, BOARD_WIDTH, ghost);

            }
            break;
        case ' ':
            hardDropTetromino(curent, board, BOARD_HEIGHT, BOARD_WIDTH);

            lockTetrominoAndUpdateBoard(curent, board, BOARD_HEIGHT, BOARD_WIDTH, &score);

            Tetromino *tetrominoes = initializeTetrominoes();
            if (!spawnNewTetromino(curent, tetrominoes, board, BOARD_HEIGHT, BOARD_WIDTH)) {
                // Game Over condition
                    break;
            }            
            break;
        default:
            break;
    }
    if (press == '\e') {
        return false;
    }

    // Check if the movement is valid before applying it
    if (isMoveValid(*curent, board, curent->x + dirX, curent->y + dirY, BOARD_HEIGHT, BOARD_WIDTH)) {
        curent->x += dirX;
        curent->y += dirY;

        calculateGhostPosition(curent, board, BOARD_HEIGHT, BOARD_WIDTH, ghost);
    }

    return true;
}

void drawGame(WINDOW *win, int **board, int BOARD_HEIGHT, int BOARD_WIDTH, int score, Tetromino curent, Tetromino *ghost) {
    // Clear the window for fresh drawing
    werase(win);
    drawBordersAndScore(win, BOARD_WIDTH + 2, BOARD_HEIGHT + 2);
    drawGhostPiece(win, *ghost);

    // Draw the board with locked tetrominoes
    for (int y = 0; y < BOARD_HEIGHT; y++) {
        for (int x = 0; x < BOARD_WIDTH; x++) {
            if (board[y][x] == 1) {
                mvwaddch(win, y+1, x+1, ACS_CKBOARD);
            }
        }
    }

    // Draw the current tetromino
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (curent.shape[i][j] == 1) {
                int posY = curent.y + i + 1;
                int posX = curent.x + j + 1;
                mvwaddch(win, posY, posX, ACS_CKBOARD);
            }
        }
    }

    // Refresh the window to update the screen
    wrefresh(win);
}


int main() {
    // Initialize screen
	WINDOW *win = initscr();
	keypad(win, true);
	nodelay(win, true);
	curs_set(0);
    timeout(0);
    box(win, 0, 0);
    wrefresh(win);

    // Get terminal dimensions
    int screen_height, screen_width;
    getmaxyx(win, screen_height, screen_width);
    
    int BOARD_HEIGHT = screen_height - 2;
    int BOARD_WIDTH = screen_width - 2;

    int** board = malloc(BOARD_HEIGHT * sizeof(int*));
    for (int i = 0; i < BOARD_HEIGHT; i++) {
        board[i] = malloc(BOARD_WIDTH * sizeof(int));
        for (int j = 0; j < BOARD_WIDTH; j++) {
            board[i][j] = 0; // Initialize the board to be empty
        }
    }

    srand(time(NULL));
    clock_t lastUpdateTime = clock();

    Tetromino *tetrominoes = initializeTetrominoes();
    int random = rand() % 7;
    Tetromino curent = tetrominoes[random];
    Tetromino ghost;

    while (true) {
        int press = wgetch(win);

        // Handle user input for movement and rotation
        bool continueGame = handleUserInput(press, &curent, board, BOARD_HEIGHT, BOARD_WIDTH, &ghost);
        if (!continueGame) {
            break;
        }

        // Automatic movement down based on timer
        clock_t currentTime = clock();
        double timeSinceLastUpdate = (double)(currentTime - lastUpdateTime) / CLOCKS_PER_SEC;

        if (timeSinceLastUpdate >= dropInterval) {
            if (isMoveValid(curent, board, curent.x, curent.y + 1, BOARD_HEIGHT, BOARD_WIDTH)) {
                curent.y += 1; // Tetromino can move down, so move it

            } else {
                lockTetrominoAndUpdateBoard(&curent, board, BOARD_HEIGHT, BOARD_WIDTH, &score);
                if (!spawnNewTetromino(&curent, tetrominoes, board, BOARD_HEIGHT, BOARD_WIDTH)) {
                    // Game Over condition

                    break;
                }
            }
            lastUpdateTime = currentTime;
        }

        // Drawing logic
        drawGame(win, board, BOARD_HEIGHT, BOARD_WIDTH, score, curent, &ghost);
    }
    endwin();

    return 0;
}

/*

    3. Culori
    4. Piesa Next
    5. Aspect mai frumos cu tot cu scor si piesa next 
        (BOARD_HEIGHT = screen_height \ 
         BOARD_WIDTH = variabil i guess)

*/
