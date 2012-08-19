/*
 * Quadrax
 */

#include <SDL/SDL.h>

#include "game.h"
#include "quadrax.h"

int scene[X_BLOCKS][Y_BLOCKS];
// TODO: neexistuje current player, oni musi hrat soucasne!
int current_player = 0;
TPlayer players[2];


// makro rika, jestli je na pozici [X][Y] volne pole
#define FREE_BLOCK(X, Y) (scene[players[current_player]. X][players[current_player]. Y] == BLANK)

// pro kazdeho hrace je ulozen pocet poli, co spadl
int fall[2] = {0, 0};


/*
 * vykresli zakladni prvky do sceny
 * ozivi hrace, nastavi jim vychozi pozice
 */
void scene_reset(void)
{
    players[0].y = 23;
    players[0].x = 5;
    players[0].dead = 0;
    players[1].y = 23;
    players[1].x = 10;
    players[1].dead = 0;

    // TODO: struktura levelu by se mela cist z dataku nejakeho
    for(int x = 0; x < X_BLOCKS; ++x)
        for(int y = 0; y < Y_BLOCKS; ++y)
            if(x == 0 || x == X_BLOCKS - 1 || y == 0 || y == Y_BLOCKS - 1)
                scene[x][y] = WALL;
            else
                scene[x][y] = BLANK;

    // schody a plosinka
    for(int x = 20; x < 30; ++x) {
        scene[x][30+20-x] = WALL;
        scene[x-1][18] = WALL;
    }
    scene[30][19] = WALL;
    scene[29][17] = WALL;

    // prolizacka
    scene[2][27] = WALL;
    scene[3][25] = WALL;
    scene[2][23] = WALL;
    scene[3][21] = WALL;
    scene[2][19] = WALL;
    scene[3][17] = WALL;
    scene[2][15] = WALL;
    scene[3][13] = WALL;
    scene[2][11] = WALL;

    scene[20][17] = BOULDER;
    scene[20][16] = BOULDER;
    scene[21][17] = BOULDER;
    scene[21][16] = BOULDER;

    // TODO: stejne tak pozice cile
    scene[5][29] = scene[6][29] = FINISH;
}


/*
 * predzvejka udalosti z klavesnice pro posun hrace/konec hry
 */
int get_key(void)
{
    SDL_Event event;
    Uint8 *keys;

    int ev = SDL_PollEvent(&event);
//    if(!SDL_PollEvent(&event))
//        return 0;

    keys = SDL_GetKeyState(NULL);

    // NOTE: to ev je tam proto, aby to neprepinalo porad, ale jen pri udalosti
    if(ev && keys[SDLK_LALT])
        current_player = (current_player + 1) % 2;

    if(event.type == SDL_QUIT || (keys[SDLK_F4] && keys[SDLK_LALT]))
        return -1;
    if(keys[SDLK_ESCAPE])
        return -1;
    if(keys[SDLK_RIGHT] && keys[SDLK_UP])
        return MOVE_UPRIGHT;
    if(keys[SDLK_LEFT] && keys[SDLK_UP])
        return MOVE_UPLEFT;
    if(keys[SDLK_RIGHT] && keys[SDLK_DOWN])
        return MOVE_DOWNRIGHT;
    if(keys[SDLK_LEFT] && keys[SDLK_DOWN])
        return MOVE_DOWNLEFT;
    if(keys[SDLK_RIGHT])
        return MOVE_RIGHT;
    if(keys[SDLK_LEFT])
        return MOVE_LEFT;
    if(keys[SDLK_UP])
        return MOVE_UP; 
    if(keys[SDLK_DOWN])
        return MOVE_DOWN;
    // TODO: promyslet poradi v jakem se to vyhodnocuje!


    return 0;
}


/*
 * posune aktualnim hracem, je-li to mozne
 */
void players_move(int action)
{
    // TODO:
    // chuze ze schodu je pomala?

    // pokud hrac pada, tak se nemuze hybat
    if(fall[current_player])
        return;

    switch(action) {
    case MOVE_RIGHT:
        // krok doprava s ochranou padu z vyzky
        if(FREE_BLOCK(x+1, y) && FREE_BLOCK(x+1, y-1) && !(FREE_BLOCK(x+1,y+1) && FREE_BLOCK(x+1,y+2) && FREE_BLOCK(x+1, y+3)))
            players[current_player].x += 1;
        // krok do schodu s kontrolou existence schodu
        else if(FREE_BLOCK(x+1, y-1) && FREE_BLOCK(x+1, y-2) && FREE_BLOCK(x, y-2) && !FREE_BLOCK(x+1,y)) {
            players[current_player].x += 1;
            players[current_player].y -= 1;
        }
        break;
    case MOVE_LEFT:
        // krok doleva s ochranou proti padu z vysky
        if(FREE_BLOCK(x-1,y) && FREE_BLOCK(x-1,y-1) && !(FREE_BLOCK(x-1,y+1) && FREE_BLOCK(x-1,y+2) && FREE_BLOCK(x-1, y+3)))
            players[current_player].x -= 1;
        // krok do schodu doleva s kontrolou existence schodu
        else if(FREE_BLOCK(x-1, y-1) && FREE_BLOCK(x-1, y-2) && FREE_BLOCK(x, y-2) && !FREE_BLOCK(x-1,y)) {
            players[current_player].x -= 1;
            players[current_player].y -= 1;
        }
        break;

    case MOVE_UPLEFT:
        // vyleze na kosku vlevo
        if(FREE_BLOCK(x-1,y-2) && FREE_BLOCK(x-1, y-3) && FREE_BLOCK(x, y-2) && !FREE_BLOCK(x-1,y-1)) {
            players[current_player].x -= 1;
            players[current_player].y -= 2;
        } else { // pokud to nejde nahoru, tak zkusi jit aspon jen vlevo
            players_move(MOVE_LEFT);
        }
        break;
    case MOVE_UPRIGHT:
        // vyleze na kostku vpravo
        if(FREE_BLOCK(x+1,y-2) && FREE_BLOCK(x+1, y-3) && FREE_BLOCK(x, y-2) && !FREE_BLOCK(x+1,y-1)) {
            players[current_player].x += 1;
            players[current_player].y -= 2;
        } else { // pokud to nejde nahoru, tak zkusi jit aspon jen vpravo
            players_move(MOVE_RIGHT);
        }
        break;
    case MOVE_DOWNLEFT:
        // podleze vlevo dolu, dava pozor, aby nespadl
        if(FREE_BLOCK(x-1, y+1) && FREE_BLOCK(x-1, y+2) && FREE_BLOCK(x-1, y) && ! FREE_BLOCK(x-1, y+3)) {
            players[current_player].x -= 1;
            players[current_player].y += 2;
        // pripadne jde jen vlevo, kdyz to nejde dolu
        } else {
            players_move(MOVE_LEFT);
        }
        break;
    case MOVE_DOWNRIGHT:
        // podleze vpravo dolu, dava pozor, aby nespadl
        if(FREE_BLOCK(x+1, y+1) && FREE_BLOCK(x+1, y+2) && FREE_BLOCK(x+1, y) && ! FREE_BLOCK(x+1, y+3)) {
            players[current_player].x += 1;
            players[current_player].y += 2;
        // pripadne jde jen vpravo, kdyz to nejde dolu
        } else {
            players_move(MOVE_RIGHT);
        }
// TODO: chuze nahoru a dolu jen po zebriku nebo necem podobnem
//    case MOVE_UP:
//    case MOVE_DOWN:
    default:
        break;
    }
}


/*
 * kontroluje, zda ma hra pokracovat
 * mrtva postava nebo dosazeni cile znaci konec hry
 */
int game_check_end(void)
{
    // TODO: tohle prepsat, vypada to hnusne!
    return !(
        (scene[players[0].x][players[0].y+1] == FINISH
                            &&
         scene[players[1].x][players[1].y+1] == FINISH)
                            ||
        players[0].dead || players[1].dead);
}



/*
 * pokud pod hracem neni zem, tak spadne
 * pokud spadne moc, umre
 */
void check_fall(int player)
{
    // pokud pod hracem neni podlaha
    if(scene[players[player].x][players[player].y+1] == BLANK) {
        fall[player] += 1;
        players[player].y += 1;
    } else {
        // pokud spadl moc, tak umrel
        if(fall[player] > 3) {
            printf("UMREL JSI !!!  [%d]\n", fall[player]);
        }
        // na pevne zemi se resetuje pocitadlo padu
        fall[player] = 0;
    }
}
