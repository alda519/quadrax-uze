/*
 * Quadrax
 */

#include <SDL/SDL.h>

#include "quadrax.h"
#include "game.h"
#include "boulder.h"

static SDL_Surface *blocks_images = NULL;

/*
 * nacteni textur
 */
int graphics_init(void)
{
    blocks_images = SDL_LoadBMP("data.bmp");
    return blocks_images == NULL;
}

/*
 * vykresleni jednoho/dvou/ctyr policek
 */
int block_draw(SDL_Surface *screen, int x, int y, int l, int type)
{
    SDL_Rect from = {
        .x = type*BLOCK_SIZE,
        .y = (l == 1) * BLOCK_SIZE,
        .w = BLOCK_SIZE,
        .h = BLOCK_SIZE};
    SDL_Rect to   = {
        .x = x*BLOCK_SIZE,
        .y = y*BLOCK_SIZE,
        .w = BLOCK_SIZE*2,
        .h = BLOCK_SIZE*2};
    SDL_BlitSurface(blocks_images, &from, screen, &to);
//    SDL_UpdateRect(screen, snake.x*TILE,snake.y*TILE,TILE,TILE);
    return 0;
}


/*
 * vykresli celou scenu
 */
int scene_draw(SDL_Surface *screen)
{
    // prekresleni sceny, mozna oddelit bokem
    for(int x = 0; x < X_BLOCKS; ++x)
        for(int y = 0; y < Y_BLOCKS; ++y)
            block_draw(screen, x, y, 0, scene[x][y].extra);

    // vykresleni balvanu
    for(int b = 0; b < boulders_cnt; b++) {
        block_draw(screen, boulders[b].x, boulders[b].y, 0, BOULDER);
        block_draw(screen, boulders[b].x+1, boulders[b].y, 0, BOULDER);
        block_draw(screen, boulders[b].x, boulders[b].y+1, 0, BOULDER);
        block_draw(screen, boulders[b].x+1, boulders[b].y+1, 0, BOULDER);
    }

    // vykresleni aktualni polohy hracu
    block_draw(screen, players[0].x, players[0].y, BLOCK_TOP, PLAYER1);
    block_draw(screen, players[0].x, players[0].y+1, BLOCK_BOTTOM, PLAYER1);
    block_draw(screen, players[1].x, players[1].y, BLOCK_TOP, PLAYER2);
    block_draw(screen, players[1].x, players[1].y+1, BLOCK_BOTTOM, PLAYER2);

    // vykresleni
    SDL_UpdateRect(screen, 0, 0, 0, 0);

    return 0;
}
