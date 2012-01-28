/*
 * Quadrax
 */

#include <SDL/SDL.h>

#include "quadrax.h"
#include "game.h"

static SDL_Surface *blocks_images = NULL;

int graphics_init(void)
{
    blocks_images = SDL_LoadBMP("data.bmp");
    return blocks_images == NULL;
}

int block_draw(SDL_Surface *screen, int x, int y, int type)
{
    SDL_Rect from = {.x = type*BLOCK_SIZE, .y = 0, .w = BLOCK_SIZE, .h = BLOCK_SIZE};
    SDL_Rect to   = {.x = x*BLOCK_SIZE, .y = y*BLOCK_SIZE, .w = BLOCK_SIZE, .h = BLOCK_SIZE};
    SDL_BlitSurface(blocks_images, &from, screen, &to);
//    SDL_UpdateRect(screen, snake.x*TILE,snake.y*TILE,TILE,TILE);
    return 0;
}

int scene_draw(SDL_Surface *screen)
{
    // prekresleni sceny, mozna oddelit bokem
    for(int i = 0; i < Y_BLOCKS; ++i)
        for(int j = 0; j < X_BLOCKS; ++j)
            block_draw(screen, j, i, scene[i][j]);

    // vykresleni aktualni polohy hracu
    block_draw(screen, players[0].x, players[0].y, PLAYER1);
    block_draw(screen, players[1].x, players[1].y, PLAYER2);

    // vykresleni
    SDL_UpdateRect(screen, 0, 0, 0, 0);
}
