#include "tetrisGameplay.h"

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

bool handleUserInput(int press, Tetromino *curent, int **board, int BOARD_HEIGHT, int BOARD_WIDTH, Tetromino *ghost, Tetromino *next, int **cellValue) {
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

            lockTetrominoAndUpdateBoard(curent, board, BOARD_HEIGHT, BOARD_WIDTH, &score, cellValue);

            Tetromino *tetrominoes = initializeTetrominoes();
            if (!spawnNewTetromino(curent, tetrominoes, board, BOARD_HEIGHT, BOARD_WIDTH, next)) {
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

int main() {
    // Initialize screen
	WINDOW *win = initscr();
	keypad(win, true);
	nodelay(win, true);
	curs_set(0);
    timeout(0);
    box(win, 0, 0);
    wrefresh(win);

    // start color functionality
	start_color();
	if (!has_colors()) {
		endwin();
		fprintf(stderr, "Error: colors");
		exit(1);
	}
    use_default_colors();
	init_pair(1, COLOR_CYAN, COLOR_BLACK); // I
    init_pair(2, COLOR_YELLOW, COLOR_BLACK); // O
    init_pair(3, COLOR_MAGENTA, COLOR_BLACK); // T
    init_pair(4, COLOR_GREEN, COLOR_BLACK); // S
    init_pair(5, COLOR_RED, COLOR_BLACK); // Z
	init_pair(6, COLOR_BLUE, COLOR_BLACK); // L
	init_pair(7, COLOR_WHITE, COLOR_BLACK); // L rev
    init_pair(8, COLOR_BLACK, COLOR_BLACK);
    // bkgd(COLOR_PAIR(8));

    // Get terminal dimensions
    int screen_height, screen_width;
    getmaxyx(win, screen_height, screen_width);
    
    int BOARD_HEIGHT = screen_height - 2;
    int BOARD_WIDTH = screen_width - 56;

    int** board = malloc(BOARD_HEIGHT * sizeof(int*));
    for (int i = 0; i < BOARD_HEIGHT; i++) {
        board[i] = malloc(BOARD_WIDTH * sizeof(int));
        for (int j = 0; j < BOARD_WIDTH; j++) {
            board[i][j] = 0; // Initialize the board to be empty
        }
    }

    int **cellValue = malloc(BOARD_HEIGHT * sizeof(int*));
    for (int i = 0; i < BOARD_HEIGHT; i++) {
        cellValue[i] = malloc(BOARD_WIDTH * sizeof(int));
        for (int j = 0; j < BOARD_WIDTH; j++) {
            cellValue[i][j] = 0; // Initialize the cellValues with 0
        }
    }

    srand(time(NULL));
    clock_t lastUpdateTime = clock();

    Tetromino *tetrominoes = initializeTetrominoes();
    int random = rand() % 7;
    Tetromino curent = tetrominoes[random];
    curent.x = BOARD_WIDTH / 2 - 2;
    curent.y = 0;
    Tetromino ghost;

    Tetromino next = tetrominoes[random];
    next.x = screen_width - box_width + 5;
    next.y = BOARD_HEIGHT / 2 - 1;
    bool isPaused = false;

    while (true) {
        int press = wgetch(win);

        if (press == 'p') {
            isPaused = !isPaused;
            if (isPaused) {
                // Show pause message only once when pausing
                clear();
                mvprintw(LINES / 2, (COLS - strlen("Game Paused. Press 'p' to resume...")) / 2, "Game Paused. Press 'p' to resume...");
                refresh();
            }
            continue; // Skip the rest of this iteration to avoid flickering
        }

        if (!isPaused) {
            // Handle user input for movement and rotation
            bool continueGame = handleUserInput(press, &curent, board, BOARD_HEIGHT, BOARD_WIDTH, &ghost, &next, cellValue);
            if (!continueGame) {
                break;
            }
        } 
        // Automatic movement down based on timer
        clock_t currentTime = clock();
        double timeSinceLastUpdate = (double)(currentTime - lastUpdateTime) / CLOCKS_PER_SEC;

        if (timeSinceLastUpdate >= dropInterval) {
            if (isMoveValid(curent, board, curent.x, curent.y + 1, BOARD_HEIGHT, BOARD_WIDTH)) {
                curent.y += 1; // Tetromino can move down, so move it

            } else {
                lockTetrominoAndUpdateBoard(&curent, board, BOARD_HEIGHT, BOARD_WIDTH, &score, cellValue);
                if (!spawnNewTetromino(&curent, tetrominoes, board, BOARD_HEIGHT, BOARD_WIDTH, &next)) {
                    printf("Game over! Your Score: %d\n", score);
                    break;
                }
            }
            lastUpdateTime = currentTime;
        }

        // Drawing logic
        if (!isPaused) {
            drawGame(win, board, BOARD_HEIGHT, BOARD_WIDTH, score, curent, &ghost, next, cellValue);
        }
    }
    endwin();

    return 0;
}
