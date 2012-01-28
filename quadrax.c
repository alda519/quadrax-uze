/*
 * Quadrax
 */

#include <stdio.h>
#include <stdlib.h>
#include <SDL/SDL.h>

#include "quadrax.h"
#include "game.h"


int main(int argc, char *argv[])
{
    if(SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "SDL: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Surface *screen;
    SDL_putenv("SDL_VIDEO_CENTERED=1"); //FIXME: okno doprostred, ale vyvola warning pri kompilaci
    screen = SDL_SetVideoMode(WIDTH, HEIGHT + STATUSBAR_SIZE, 32, SDL_HWSURFACE);
    if(screen == NULL) {
        fprintf(stderr, "SDL: %s\n", SDL_GetError());
        return 1;
    }
    SDL_WM_SetCaption("Quadrax BETA", NULL);

    graphics_init();
    scene_reset();

    int action = 0;
    do {
        action = get_key();
        SDL_Delay(10);
        scene_draw(screen);

        players_move(action);

    } while(action >= 0);

    SDL_Quit();
    return 0;
}
