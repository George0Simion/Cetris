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
    switch (linesCleared) {
        case 1:
            return 100; // Single line
        case 2:
            return 300; // Double line
        case 3:
            return 500; // Triple line
        case 4:
            return 800; // Tetris
        default:
            return 0; // No lines cleared, or adjust as needed for your game's logic
    }
}

void drawBordersAndScore(WINDOW *win, int screen_width, int screen_height) {
    erase();

    // Top border and score
    char score_text[30];
    sprintf(score_text, "  SCORE: %d  ", score);
    int score_pos = (screen_width / 2) - (strlen(score_text) / 2);
    mvaddstr(0, score_pos, score_text);
    for (int x = 0; x < screen_width; x++) {
        if (x < score_pos || x >= score_pos + strlen(score_text)) {
            mvaddch(0, x, '#');
        }
    }
    // Side borders
    for (int y = 1; y < screen_height - 1; y++) {
        mvaddch(y, 0, '#');
        mvaddch(y, screen_width - 1, '#');
    }
    // Bottom border
    for (int x = 0; x < screen_width; x++) {
        mvaddch(screen_height - 1, x, '#');
    }
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

bool handleUserInput(int press, Tetromino *curent, int **board, int BOARD_HEIGHT, int BOARD_WIDTH) {
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
    }

    return true;
}

void drawGame(WINDOW *win, int **board, int BOARD_HEIGHT, int BOARD_WIDTH, int score, Tetromino curent) {
    // Clear the window for fresh drawing
    werase(win);
    drawBordersAndScore(win, BOARD_WIDTH + 2, BOARD_HEIGHT + 2);

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

    // Check for complete lines and update the board and score accordingly
    int linesCleared = checkCompleteLines(board, BOARD_HEIGHT, BOARD_WIDTH);
    *score += calculateScore(linesCleared); // Update score based on the lines cleared
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

    while (true) {
        int press = wgetch(win);

        // Handle user input for movement and rotation
        bool continueGame = handleUserInput(press, &curent, board, BOARD_HEIGHT, BOARD_WIDTH);
        if (!continueGame) {
            break;
        }

        // Automatic movement down based on timer
        clock_t currentTime = clock();
        double timeSinceLastUpdate = (double)(currentTime - lastUpdateTime) / CLOCKS_PER_SEC;
        if (timeSinceLastUpdate >= 1.0) {
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
        drawGame(win, board, BOARD_HEIGHT, BOARD_WIDTH, score, curent);
    }
    endwin();

    return 0;
}

/*

Maine:
    1. Rotirea formelor
    2. Debug mai serios sa vad ca merge peste tot 
    3. Culori
    4. Piesa Next
    5. Aspect mai frumos cu tot cu scor si piesa next 
        (BOARD_HEIGHT = screen_height \ 
         BOARD_WIDTH = variabil i guess)
    7. Cu timpul sa devina mai greu
   *8. space = teleport jos

*/
