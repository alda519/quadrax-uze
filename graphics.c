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
 * vykresli 1 policko zdi (16x16)
 */
void draw_wall(SDL_Surface *screen, int x, int y, int type)
{
    SDL_Rect from = {
        .x = type*BLOCK_SIZE,
        .y = 0,
        .w = BLOCK_SIZE,
        .h = BLOCK_SIZE
    };
    SDL_Rect to = {
        .x = x*BLOCK_SIZE,
        .y = y*BLOCK_SIZE,
        .w = BLOCK_SIZE,
        .h = BLOCK_SIZE
    };

    // prenos dat
    SDL_BlitSurface(blocks_images, &from, screen, &to);
    // update
    //SDL_UpdateRect(screen, x*BLOCK_SIZE, y*BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE);
}

/*
 * vykresli celou scenu
 */
int scene_draw(SDL_Surface *screen)
{
    // prekresleni sceny, mozna oddelit bokem
    for(int x = 0; x < X_BLOCKS; ++x)
        for(int y = 0; y < Y_BLOCKS; ++y)
            block_draw(screen, x, y, 0, scene[x][y]);

    // vykresleni aktualni polohy hracu
    block_draw(screen, players[0].x, players[0].y  , BLOCK_BOTTOM, PLAYER1);
    block_draw(screen, players[0].x, players[0].y-1, BLOCK_TOP, PLAYER1);
    block_draw(screen, players[1].x, players[1].y  , BLOCK_BOTTOM, PLAYER2);
    block_draw(screen, players[1].x, players[1].y-1, BLOCK_TOP, PLAYER2);

    // vykresleni
    SDL_UpdateRect(screen, 0, 0, 0, 0);

    return 0;
}
