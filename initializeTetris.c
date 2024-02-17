#include "initializeTetris.h"

int score  = 0;
int curentLevel = 1;
double dropInterval = 0.5;
int nextStartX;
int nextStartY;

Tetromino* initializeTetrominoes() {
    Tetromino *tetrominoes = malloc(7 * sizeof(Tetromino));
    if (tetrominoes == NULL) {
        return NULL;
    }

    int shapes[7][4][4] = {
        { // I shape
            {0, 1, 0, 0},
            {0, 1, 0, 0},
            {0, 1, 0, 0},
            {0, 1, 0, 0}
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

void drawBordersAndScore(WINDOW *win, int screen_width, int screen_height, int leftOffset) {
    werase(win);

    int statsWidth = box_width;
    int statsHeight = box_height - 2;
    int statsStartX = leftOffset - statsWidth - 3;
    int statsStartY = (screen_height - statsHeight) / 2;

    // Draw the stats border
    char stats_text[] = "Stats";
    int stats_pos = (statsWidth / 2) - (strlen(stats_text) / 2);

    mvwaddch(win, statsStartY, statsStartX, ACS_ULCORNER);
    for (int x = statsStartX + 1; x < statsStartX + stats_pos; x++) {
        mvwaddch(win, statsStartY, x, ACS_HLINE);
    }
    mvwaddstr(win, statsStartY, statsStartX + stats_pos, stats_text);
    for (int x = statsStartX + stats_pos + strlen(stats_text); x < statsStartX + statsWidth - 1; x++) {
        mvwaddch(win, statsStartY, x, ACS_HLINE);
    }
    mvwaddch(win, statsStartY, statsStartX + statsWidth - 1, ACS_URCORNER);

    // Draw sides of the stats box
    for (int y = statsStartY + 1; y < statsStartY + statsHeight - 1; y++) {
        mvwaddch(win, y, statsStartX, ACS_VLINE);
        mvwaddch(win, y, statsStartX + statsWidth - 1, ACS_VLINE);
    }

    // Draw bottom of the stats box
    mvwaddch(win, statsStartY + statsHeight - 1, statsStartX, ACS_LLCORNER);
    for (int x = statsStartX + 1; x < statsStartX + statsWidth - 1; x++) {
        mvwaddch(win, statsStartY + statsHeight - 1, x, ACS_HLINE);
    }
    mvwaddch(win, statsStartY + statsHeight - 1, statsStartX + statsWidth - 1, ACS_LRCORNER);

    // Print the score and level inside the stats border
    mvwprintw(win, statsStartY + 1, statsStartX + 2, "Score:        %d", score);
    mvwprintw(win, statsStartY + 3, statsStartX + 2, "Level:        %d", curentLevel);

    int nextWidth = box_width;
    int nextHeight = box_height;
    nextStartX = leftOffset + screen_width + 3;
    nextStartY = (screen_height - nextHeight) / 2;

    char next_text[] = "Next";
    int next_pos = (nextWidth / 2) - (strlen(next_text) / 2);

    mvwaddch(win, nextStartY, nextStartX, ACS_ULCORNER);
    for (int x = nextStartX + 1; x < nextStartX + next_pos; x++) {
        mvwaddch(win, nextStartY, x, ACS_HLINE);
    }
    mvwaddstr(win, nextStartY, nextStartX + next_pos, next_text);
    for (int x = nextStartX + next_pos + strlen(next_text); x < nextStartX + nextWidth - 1; x++) {
        mvwaddch(win, nextStartY, x, ACS_HLINE);
    }
    mvwaddch(win, nextStartY, nextStartX + nextWidth - 1, ACS_URCORNER);

    // Draw sides of the stats box
    for (int y = nextStartY + 1; y < nextStartY + nextHeight - 1; y++) {
        mvwaddch(win, y, nextStartX, ACS_VLINE);
        mvwaddch(win, y, nextStartX + nextWidth - 1, ACS_VLINE);
    }

    // Draw bottom of the stats box
    mvwaddch(win, nextStartY + nextHeight - 1, nextStartX, ACS_LLCORNER);
    for (int x = nextStartX + 1; x < nextStartX + nextWidth - 1; x++) {
        mvwaddch(win, nextStartY + nextHeight - 1, x, ACS_HLINE);
    }
    mvwaddch(win, nextStartY + nextHeight - 1, nextStartX + nextWidth - 1, ACS_LRCORNER);

    // Top border
    char score_text[7] = "Tetris";
    int score_pos = (screen_width / 2) - (strlen(score_text) / 2);
    mvaddstr(0, leftOffset + score_pos, score_text);
    mvaddch(0, leftOffset, ACS_ULCORNER);
    for (int x = 1; x < screen_width - 1; x++) {
        if (x < score_pos || x >= score_pos + strlen(score_text)) {
            mvaddch(0, leftOffset + x, ACS_HLINE);
        }
    }
    mvaddch(0, leftOffset + screen_width - 1, ACS_URCORNER);

    // Side borders
    for (int y = 1; y < screen_height - 1; y++) {
        mvaddch(y, leftOffset, ACS_VLINE);
        mvaddch(y, leftOffset + screen_width - 1, ACS_VLINE);
    }

    // Bottom border
    mvaddch(screen_height - 1, leftOffset, ACS_LLCORNER);
    for (int x = 1; x < screen_width - 1; x++) {
        mvaddch(screen_height - 1, x + leftOffset, ACS_HLINE);
    }
    mvaddch(screen_height - 1, leftOffset + screen_width - 1, ACS_LRCORNER);
}

void drawGhostPiece(WINDOW *win, Tetromino ghost, int leftOffset) {
    wattron(win, A_DIM); // Make the ghost less visible

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (ghost.shape[i][j] == 1) {
                int y = ghost.y + i + 1;
                int x = ghost.x + j + 1 + leftOffset;

                mvwaddch(win, y, x, ACS_CKBOARD);
            }
        }
    }
    wattroff(win, A_DIM); // Turn off the dim attribute
}

void drawGame(WINDOW *win, int **board, int BOARD_HEIGHT, int BOARD_WIDTH, int score, Tetromino curent, Tetromino *ghost, Tetromino next) {
    // Clear the window for fresh drawing
    werase(win);
    int leftOffset = (getmaxx(win) - BOARD_WIDTH) / 2;

    drawBordersAndScore(win, BOARD_WIDTH + 2, BOARD_HEIGHT + 2, leftOffset);
    drawGhostPiece(win, *ghost, leftOffset);

    // Draw the board with locked tetrominoes
    for (int y = 0; y < BOARD_HEIGHT; y++) {
        for (int x = 0; x < BOARD_WIDTH; x++) {
            if (board[y][x] == 1) {
                mvwaddch(win, y+1, x+1+leftOffset, ACS_CKBOARD);
            }
        }
    }

    // Draw the current tetromino
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (curent.shape[i][j] == 1) {
                int posY = curent.y + i + 1;
                int posX = curent.x + j + 1 + leftOffset;
                mvwaddch(win, posY, posX, ACS_CKBOARD);
            }
        }
    }

    // Draw the next tetromino
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (next.shape[i][j] == 1) {
                int posY = next.y + i;
                int posX = next.x + j;
                mvwaddch(win, posY, posX, ACS_CKBOARD);
            }
        }
    }

    // Refresh the window to update the screen
    wrefresh(win);
}