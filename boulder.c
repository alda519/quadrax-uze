/*
 * Quadrax
 */

#include "boulder.h"
#include "game.h"

boulder_t boulders[MAX_BOULDERS];

int boulders_cnt;

int init_boulders(void) {
    boulders_cnt = 0;
    return 0;
}


/**
 * Pridani moveho balvanu
 */
int new_boulder(int x, int y) {
    if(boulders_cnt < MAX_BOULDERS) {
        boulders[boulders_cnt].x = x;
        boulders[boulders_cnt].y = y;
        boulders_cnt += 1;

        scene[x][y].type = SOLID;
        scene[x][y+1].type = SOLID;
        scene[x+1][y].type = SOLID;
        scene[x+1][y+1].type = SOLID;

        return 1;
    } else {
        return 0;
    }
}

/**
 * Smaze balvan
 */
int del_boulder(int i) {
    if(boulders_cnt) {
        boulders[i] = boulders[boulders_cnt];
        boulders_cnt -= 1;
    }
    return 0;
}


/**
 * vrati true, pokud na dane souradnici je balvan
 */
int find_boulder(int x, int y) {
    for(int b = 0; b < boulders_cnt; ++b)
        if(boulders[b].x == x && boulders[b].y == y)
            return b+1;
    return 0;
}


void boulders_fall(void) {
    for(int b = 0; b < boulders_cnt; b++) {
        if(FREE_BLOCK(boulders[b].x, boulders[b].y+2) && FREE_BLOCK(boulders[b].x+1, boulders[b].y+2)) {
            scene[boulders[b].x][boulders[b].y].type = FREE;
            scene[boulders[b].x+1][boulders[b].y].type = FREE;
            scene[boulders[b].x][boulders[b].y+2].type = SOLID;
            scene[boulders[b].x+1][boulders[b].y+2].type = SOLID;
            boulders[b].y += 1;
        }
        for(int p = 0; p < 2; ++p)
            if((players[p].x == boulders[b].x || players[p].x == boulders[b].x+1) && players[p].y == boulders[b].y + 2)
                players[p].dead = 1;
    }
}
