/*
 * Quadrax
 */

#ifndef GAME_H
#define GAME_H

#include "quadrax.h"

typedef struct {
    int x, y;
} TPlayer;

extern int scene[Y_BLOCKS][X_BLOCKS];
extern TPlayer players[2];
extern int current_player;

void scene_reset(void);

void scene_update(void);

int get_key(void);
void players_move(int);

#endif
