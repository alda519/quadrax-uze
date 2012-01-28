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
    while(game_check_end() && action != -1) {
        action = get_key();
        players_move(action);
        scene_draw(screen);
        SDL_Delay(10);
    }

    
    SDL_Delay(500);

    SDL_Quit();
    return 0;
}
