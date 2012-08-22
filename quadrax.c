/*
 * Quadrax
 */

#include <stdio.h>
#include <stdlib.h>
#include <SDL/SDL.h>

#include "quadrax.h"
#include "game.h"
#include "graphics.h"


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


    int action;
    int level = 0;
    do {
        scene_reset(level);

        action = get_key();
        int gamestat;

        while((gamestat = game_check_end()) == GAME_CONTINUE && action >= 0) {
            action = get_key();

            check_fall(PLAYER_RED);
            check_fall(PLAYER_BLUE);
            players_move(action);
            boulders_fall(PLAYER_BLUE);
            scene_draw(screen);

            SDL_Delay(50);
        }
        if(gamestat == GAME_NEXTLEVEL)
            level += 1;
    } while(action != GAME_END && level < LEVELS);

    SDL_Quit();
    return 0;
}
