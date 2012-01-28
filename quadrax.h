/*
 * Quadrax
 */

#ifndef QUADRAX_H
#define QUADRAX_H

#define BLOCK_SIZE 20

#define X_BLOCKS 40
#define Y_BLOCKS 25

#define WIDTH (X_BLOCKS * BLOCK_SIZE)
#define HEIGHT (Y_BLOCKS * BLOCK_SIZE)

#define STATUSBAR_SIZE 0 //50

enum {
    BLANK = 0,
    WALL = 1,
    PLAYER1 = 2,
    PLAYER2 = 3,
    FINISH = 4,
} mapelements;

#endif
