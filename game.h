/*
 * Quadrax
 */

#ifndef GAME_H
#define GAME_H

#include "quadrax.h"

#define PLAYER_RED 0
#define PLAYER_BLUE 1

// makro rika, jestli je na pozici [X][Y] volne pole
#define FREE_BLOCK(X, Y) (scene[X][Y].type)

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

/**
 * structura popisujici jedno policko herni mapy
 * type 0 = volno, 1 = zed/balvan/cokoliv_cim_nejde_chodit
 * extra = presne urceni typu elementu
 */
typedef struct {
    unsigned char type : 1;
    unsigned char extra : 7;
} t_block;

extern t_block scene[X_BLOCKS][Y_BLOCKS];
extern TPlayer players[2];
extern int current_player;

void scene_reset(void);

void scene_update(void);

int get_key(void);
void players_move(int);

int game_check_end(void);


void check_fall(int);

#endif
