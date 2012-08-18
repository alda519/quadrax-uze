/*
 * Quadrax
 */

#ifndef GAME_H
#define GAME_H

#include "quadrax.h"

#define PLAYER_RED 0
#define PLAYER_BLUE 1

typedef struct {
    int x, y;
    int dead;
} TPlayer;

enum {
    MOVE_LEFT = 1,
    MOVE_RIGHT,
    MOVE_UP,
    MOVE_DOWN,
    MOVE_UPRIGHT,
    MOVE_UPLEFT,
    MOVE_DOWNRIGHT,
    MOVE_DOWNLEFT,
};

extern int scene[X_BLOCKS][Y_BLOCKS];
extern TPlayer players[2];
extern int current_player;

void scene_reset(void);

void scene_update(void);

int get_key(void);
void players_move(int);

int game_check_end(void);


void check_fall(int);

#endif
