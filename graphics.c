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
    SDL_Rect from = {.x = type*20, .y = 0, .w = 20, .h = 20};
    SDL_Rect to   = {.x = x*20, .y = y*20, .w = 20, .h = 20};
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
