#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>

#ifdef _WIN32
#include <conio.h>
#include <windows.h>
#endif
#include "funcs.h"

#define TANK_WIDTH        40
#define TANK_HEIGHT       20
#define BULLETS_PER_TANK   3

typedef struct {
    int x;
    int y;
    int dir;    // 0 = up, 1 = right, 2 = down, 3 = left
    char symbol;
    int alive;
} Tank;

typedef struct {
    int x;
    int y;
    int dx;
    int dy;
    int active;
} Bullet;

// direction helper
static void dir_to_delta(int dir, int *dx, int *dy)
{
    switch (dir) {
        case 0: *dx = 0;  *dy = -1; break; // up
        case 1: *dx = 1;  *dy = 0;  break; // right
        case 2: *dx = 0;  *dy = 1;  break; // down
        case 3: *dx = -1; *dy = 0;  break; // left
        default: *dx = 0; *dy = 0;  break;
    }
}

static void rotate_left(Tank *t)
{
    t->dir = (t->dir + 3) % 4;
}

static void rotate_right(Tank *t)
{
    t->dir = (t->dir + 1) % 4;
}

static void move_tank(Tank *t, const Tank *other, int forward)
{
    int dx, dy;
    dir_to_delta(t->dir, &dx, &dy);
    if (!forward) {
        dx = -dx;
        dy = -dy;
    }

    int nx = t->x + dx;
    int ny = t->y + dy;

    // keep inside arena 
    if (nx <= 1 || nx >= TANK_WIDTH - 2 ||
        ny <= 1 || ny >= TANK_HEIGHT - 2) {
        return;
    }

    // don't move into other tank
    if (nx == other->x && ny == other->y) {
        return;
    }

    t->x = nx;
    t->y = ny;
}

// fire bullet: uses first free slot out of BULLETS_PER_TANK
static void fire_bullet(const Tank *t, Bullet bullets[BULLETS_PER_TANK])
{
    for (int i = 0; i < BULLETS_PER_TANK; i++) {
        if (!bullets[i].active) {

            int dx, dy;
            dir_to_delta(t->dir, &dx, &dy);

            int bx = t->x + dx;
            int by = t->y + dy;

            // starting in wall? then don't fire
            if (bx <= 0 || bx >= TANK_WIDTH - 1 ||
                by <= 0 || by >= TANK_HEIGHT - 1) {
                return;
            }

            bullets[i].x      = bx;
            bullets[i].y      = by;
            bullets[i].dx     = dx;
            bullets[i].dy     = dy;
            bullets[i].active = 1;
            return;
        }
    }
    // all bullets active= no shot
}

// move one bullet, return 1 if it hit target
static int update_bullet(Bullet *b, Tank *target)
{
    if (!b->active) return 0;

    int nx = b->x + b->dx;
    int ny = b->y + b->dy;

    // miss
    if (nx <= 0 || nx >= TANK_WIDTH - 1 ||
        ny <= 0 || ny >= TANK_HEIGHT - 1) {
        b->active = 0;
        return 0;
    }

    // hits tank
    if (nx == target->x && ny == target->y && target->alive) {
        b->active = 0;
        target->alive = 0;
        return 1;
    }

    b->x = nx;
    b->y = ny;
    return 0;
}

// draw arena, bullets, tanks + barrels
// different reperesentation of the tanks to avoid confusion
static void draw_tank_arena(const Tank *t1, const Tank *t2,
                            const Bullet b1[BULLETS_PER_TANK],
                            const Bullet b2[BULLETS_PER_TANK])
{
    char grid[TANK_HEIGHT][TANK_WIDTH + 1];

    // 1) border with +, -, |
    for (int y = 0; y < TANK_HEIGHT; ++y) {
        for (int x = 0; x < TANK_WIDTH; ++x) {
            if ((y == 0 || y == TANK_HEIGHT - 1) &&
                (x == 0 || x == TANK_WIDTH - 1)) {
                grid[y][x] = '+';          // corners
            } else if (y == 0 || y == TANK_HEIGHT - 1) {
                grid[y][x] = '-';          // top/bottom
            } else if (x == 0 || x == TANK_WIDTH - 1) {
                grid[y][x] = '|';          // left/right
            } else {
                grid[y][x] = ' ';
            }
        }
        grid[y][TANK_WIDTH] = '\0';
    }

    // 2) bullets
    for (int i = 0; i < BULLETS_PER_TANK; i++) {
        if (b1[i].active &&
            b1[i].x > 0 && b1[i].x < TANK_WIDTH &&
            b1[i].y > 0 && b1[i].y < TANK_HEIGHT) {
            grid[b1[i].y][b1[i].x] = '*';
        }
        if (b2[i].active &&
            b2[i].x > 0 && b2[i].x < TANK_WIDTH &&
            b2[i].y > 0 && b2[i].y < TANK_HEIGHT) {
            grid[b2[i].y][b2[i].x] = '*';
        }
    }

    // 3) tank 1 body + barrel
    if (t1->alive &&
        t1->x > 0 && t1->x < TANK_WIDTH &&
        t1->y > 0 && t1->y < TANK_HEIGHT) {

        grid[t1->y][t1->x] = t1->symbol;

        int dx1, dy1;
        dir_to_delta(t1->dir, &dx1, &dy1);
        int bx1 = t1->x + dx1;
        int by1 = t1->y + dy1;

        if (bx1 > 0 && bx1 < TANK_WIDTH &&
            by1 > 0 && by1 < TANK_HEIGHT) {
            char barrel1 = (dx1 != 0) ? '-' : '|';
            if (grid[by1][bx1] == ' ')
                grid[by1][bx1] = barrel1;
        }
    }

    // 4) tank 2 body + barrel
    if (t2->alive &&
        t2->x > 0 && t2->x < TANK_WIDTH &&
        t2->y > 0 && t2->y < TANK_HEIGHT) {

        grid[t2->y][t2->x] = t2->symbol;

        int dx2, dy2;
        dir_to_delta(t2->dir, &dx2, &dy2);
        int bx2 = t2->x + dx2;
        int by2 = t2->y + dy2;

        if (bx2 > 0 && bx2 < TANK_WIDTH &&
            by2 > 0 && by2 < TANK_HEIGHT) {
            char barrel2 = (dx2 != 0) ? '-' : '|';
            if (grid[by2][bx2] == ' ')
                grid[by2][bx2] = barrel2;
        }
    }

#ifdef _WIN32
    system("cls");
#else
    printf("\033[2J\033[H");
#endif

    for (int y = 0; y < TANK_HEIGHT; ++y) {
        printf("%s\n", grid[y]);
    }

    printf("\nControls:\n");
    printf("  Player 1: W/S = forward/back, A/D = turn, T = shoot\n");
    printf("  Player 2: arrows = move/turn, K = shoot\n");
    printf("  Press Q to quit.\n");
}

// menu 6

void menu_item_6(void)
{
#ifdef _WIN32
    Tank p1 = { 5, TANK_HEIGHT / 2, 1, 'A', 1 };                     // facing right
    Tank p2 = { TANK_WIDTH - 6, TANK_HEIGHT / 2, 3, 'B', 1 };        // facing left

    Bullet b1[BULLETS_PER_TANK];
    Bullet b2[BULLETS_PER_TANK];

    // init bullets
    for (int i = 0; i < BULLETS_PER_TANK; i++) {
        b1[i].active = 0;
        b2[i].active = 0;
    }

    int running = 1;
    int winner  = 0;

    while (running) {
        // inputs
        while (_kbhit()) {
            int ch = _getch();

            if (ch == 0 || ch == 224) {
                // arrow keys for player 2
                int ch2 = _getch();
                switch (ch2) {
                    case 72: // up
                        move_tank(&p2, &p1, 1);
                        break;
                    case 80: // down
                        move_tank(&p2, &p1, 0);
                        break;
                    case 75: // left
                        rotate_left(&p2);
                        break;
                    case 77: // right
                        rotate_right(&p2);
                        break;
                    default:
                        break;
                }
            } else {
                ch = tolower(ch);
                switch (ch) {
                    // player 1
                    case 'w':
                        move_tank(&p1, &p2, 1);
                        break;
                    case 's':
                        move_tank(&p1, &p2, 0);
                        break;
                    case 'a':
                        rotate_left(&p1);
                        break;
                    case 'd':
                        rotate_right(&p1);
                        break;
                    case 't':
                        fire_bullet(&p1, b1);
                        break;

                    // player 2 shoot
                    case 'k':
                        fire_bullet(&p2, b2);
                        break;

                    // quit
                    case 'q':
                        running = 0;
                        break;
                    default:
                        break;
                }
            }
        }

        // bullets
        for (int i = 0; i < BULLETS_PER_TANK && running; i++) {
            if (update_bullet(&b1[i], &p2)) {
                winner = 1;
                running = 0;
            }
        }
        for (int i = 0; i < BULLETS_PER_TANK && running; i++) {
            if (update_bullet(&b2[i], &p1)) {
                winner = 2;
                running = 0;
            }
        }

        if (!running) break;

        // fps
        draw_tank_arena(&p1, &p2, b1, b2);

        if (!p1.alive || !p2.alive) {
            running = 0;
        }

        Sleep(50); // slow it down a bit
    }

    // final frame
    draw_tank_arena(&p1, &p2, b1, b2);

    if (winner == 1) {
        printf("\nPlayer 1 wins!\n");
    } else if (winner == 2) {
        printf("\nPlayer 2 wins!\n");
    } else {
        printf("\nGame over.\n");
    }

    printf("Press any key to return to the main menu...");
    _getch();

#else
    printf("Tank game is only supported on Windows (needs <conio.h>).\n");
#endif
}
