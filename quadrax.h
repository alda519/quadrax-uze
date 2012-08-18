/*
 * Quadrax
 */

#ifndef QUADRAX_H
#define QUADRAX_H


#define BLOCK_SIZE 16

#define X_BLOCKS 45
#define Y_BLOCKS 30

#define WIDTH (X_BLOCKS * BLOCK_SIZE)
#define HEIGHT (Y_BLOCKS * BLOCK_SIZE + STATUSBAR_SIZE)

#define STATUSBAR_SIZE 0 //50

#define BLOCK_TOP 0
#define BLOCK_BOTTOM 1

enum {
    BLANK = 0,
    WALL = 1,
    PLAYER1 = 2,
    PLAYER2 = 3,
    FINISH = 4,
} mapelements;

#endif
