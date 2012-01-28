/*
 * Quadrax
 */

#include <SDL/SDL.h>

#include "game.h"

int scene[Y_BLOCKS][X_BLOCKS];
int current_player = 0;
TPlayer players[2];

/*
 * vykresli zakladni prvky do sceny
 */
void scene_reset(void)
{
    players[0].y = 23;
    players[0].x = 5;
    players[1].y = 23;
    players[1].x = 10;

    for(int i = 0; i < Y_BLOCKS; ++i)
        for(int j = 0; j < X_BLOCKS; ++j)
            if(i == 0 || i == Y_BLOCKS - 1 || j == 0 || j == X_BLOCKS - 1)
                scene[i][j] = WALL;
            else
                scene[i][j] = BLANK;

    // pozice hracu by mela byt nactena ze souboru levelu
    scene[players[0].y][players[0].x] = PLAYER1;
    scene[players[1].y][players[1].x] = PLAYER2;

    // stejne tak pozice cile
    scene[24][5] = scene[24][6] = FINISH;
}


int get_key(void)
{
    SDL_Event event;
    Uint8 *keys;

    int ev = SDL_PollEvent(&event);
//    if(!SDL_PollEvent(&event))
//        return 0;

    keys = SDL_GetKeyState(NULL);
    if(event.type == SDL_QUIT || (keys[SDLK_F4] && keys[SDLK_LALT]))
        return -1;
    if(keys[SDLK_ESCAPE])
        return -1;
    if(keys[SDLK_RIGHT])
        return 1;
    if(keys[SDLK_LEFT])
        return 3;
    if(keys[SDLK_UP])
        return 4; 
    if(keys[SDLK_DOWN])
        return 2;

    if(ev && keys[SDLK_LALT])
        current_player = (current_player + 1) % 2;

    return 0;
}

void players_move(int action)
{
    if(action)
        printf("moving %d\n", action);
    switch(action) {
        case 1:
            if(scene[players[current_player].y][players[current_player].x + 1] == BLANK)
                players[current_player].x += 1;
            break;
        case 2:
            if(scene[players[current_player].y + 1][players[current_player].x] == BLANK)
                players[current_player].y += 1;
            break;
        case 3:
            if(scene[players[current_player].y][players[current_player].x - 1] == BLANK)
                players[current_player].x -= 1;
            break;
        case 4:
            if(scene[players[current_player].y - 1][players[current_player].x] == BLANK)
                players[current_player].y -= 1;
            break;
        default:
            break;
    }
}
