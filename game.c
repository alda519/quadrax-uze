/*
 * Quadrax
 */

#include <SDL/SDL.h>

#include "game.h"
#include "quadrax.h"
#include "boulder.h"


t_block scene[X_BLOCKS][Y_BLOCKS];

// TODO: neexistuje current player, oni musi hrat soucasne!
int current_player = 0;
TPlayer players[2];


// pro kazdeho hrace je ulozen pocet poli, co spadl
int fall[2] = {0, 0};

/*
 * vykresli zakladni prvky do sceny
 * ozivi hrace, nastavi jim vychozi pozice
 */
void scene_reset(int level)
{
    char levelname[] = "level00.map";
    levelname[6] += level;

    FILE *map = fopen(levelname, "r");
    if(map == NULL) {
        perror("fopen");
        exit(1);
    }
    init_boulders();

    // nacteni mapy ze souboru, pozdeji na nic, protoze to bude hardcoded
    for(int y = 0; y < Y_BLOCKS; ++y) {
        for(int x = 0; x < X_BLOCKS; ++x) {
            int c = fgetc(map);
            switch(c) {
                case '1':
                case '2':
                    players[c-'1'].x = x;
                    players[c-'1'].y = y;
                    scene[x][y].type = FREE;
                    scene[x][y+1].type = FREE;
                    scene[x][y].extra = BLANK;
                    scene[x][y+1].extra = BLANK;
                    break;
                case '.':
                    scene[x][y].type = FREE;
                    scene[x][y].extra = BLANK;
                    break;
                case '#':
                    scene[x][y].type = SOLID;
                    scene[x][y].extra = WALL;
                    break;
                case 'B':
                    new_boulder(x-1, y-1);
                    break;
                case 'F':
                    scene[x][y].type = SOLID;
                    scene[x][y].extra = FINISH;
                    break;
                case EOF:
                    fprintf(stderr, "Rozbity soubor s mapou!\n");
                    exit(1);
                default:
                    x -= 1;
                    break;
            }
        }
    }
    players[0].dead = 0;
    players[1].dead = 0;
    fclose(map);
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
        return GAME_END;
    if(keys[SDLK_ESCAPE])
        return GAME_RESET;
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

    int *px = &(players[current_player].x);
    int *py = &(players[current_player].y);
    int x = *px;
    int y = *py;

    switch(action) {
    case MOVE_RIGHT:
        // krok doprava s ochranou padu z vyzky
        if(FREE_BLOCK(x+1, y) && FREE_BLOCK(x+1, y+1) && !(FREE_BLOCK(x+1,y+2) && FREE_BLOCK(x+1,y+3) && FREE_BLOCK(x+1, y+4) && FREE_BLOCK(x+1, y+5)))
            players[current_player].x += 1;
        // krok do schodu s kontrolou existence schodu
        else if(FREE_BLOCK(x+1, y) && FREE_BLOCK(x+1, y-1) && FREE_BLOCK(x, y-1) && !FREE_BLOCK(x+1,y+1)) {
            players[current_player].x += 1;
            players[current_player].y -= 1;
        // popotlaceni balvanu
        } else if(FREE_BLOCK(x+3, y) && FREE_BLOCK(x+3, y+1)) {
            // nejde prejet druheho
            if(players[(current_player+1)%2].x == x+3 && (players[(current_player+1)%2].y == y || players[(current_player+1)%2].y == y+1))
                break;
            int b = find_boulder(players[current_player].x+1, players[current_player].y);
            if(b) {
                boulders[b-1].x += 1;
                scene[x+1][y].type = FREE;
                scene[x+1][y+1].type = FREE;
                scene[x+3][y].type = SOLID;
                scene[x+3][y+1].type = SOLID;
                players[current_player].x += 1;
            }
        }
        break;
    case MOVE_LEFT:
        // krok doleva s ochranou proti padu z vysky
        if(FREE_BLOCK(x-1,y) && FREE_BLOCK(x-1,y+1) && !(FREE_BLOCK(x-1,y+2) && FREE_BLOCK(x-1,y+3) && FREE_BLOCK(x-1, y+4) && FREE_BLOCK(x-1, y+5)))
            players[current_player].x -= 1;
        // krok do schodu doleva s kontrolou existence schodu
        else if(FREE_BLOCK(x-1, y) && FREE_BLOCK(x-1, y-1) && FREE_BLOCK(x, y-1) && !FREE_BLOCK(x-1,y+1)) {
            players[current_player].x -= 1;
            players[current_player].y -= 1;
        // posun balvanu
        } else if(FREE_BLOCK(x-3, y) && FREE_BLOCK(x-3, y+1)) {
            // nejde prejet druheho
            if(players[(current_player+1)%2].x == x-3 && (players[(current_player+1)%2].y == y || players[(current_player+1)%2].y == y+1))
                break;
            int b = find_boulder(players[current_player].x-2, players[current_player].y);
            if(b) {
                boulders[b-1].x -= 1;
                scene[x-1][y].type = FREE;
                scene[x-1][y+1].type = FREE;
                scene[x-3][y].type = SOLID;
                scene[x-3][y+1].type = SOLID;
                players[current_player].x -= 1;
            }
        }
        break;

    case MOVE_UPLEFT:
        // vyleze na kosku vlevo
        if(FREE_BLOCK(x-1,y-1) && FREE_BLOCK(x-1, y-2) && FREE_BLOCK(x, y-1) && !FREE_BLOCK(x-1,y)) {
            players[current_player].x -= 1;
            players[current_player].y -= 2;
        } else { // pokud to nejde nahoru, tak zkusi jit aspon jen vlevo
            players_move(MOVE_LEFT);
        }
        break;
    case MOVE_UPRIGHT:
        // vyleze na kostku vpravo
        if(FREE_BLOCK(x+1,y-1) && FREE_BLOCK(x+1, y-2) && FREE_BLOCK(x, y-1) && !FREE_BLOCK(x+1,y)) {
            players[current_player].x += 1;
            players[current_player].y -= 2;
        } else { // pokud to nejde nahoru, tak zkusi jit aspon jen vpravo
            players_move(MOVE_RIGHT);
        }
        break;

    case MOVE_DOWNLEFT:
        // podleze vlevo dolu, dava pozor, aby nespadl
        if(FREE_BLOCK(x-1, y+1) && FREE_BLOCK(x-1, y+2) && FREE_BLOCK(x-1, y+1) && ! FREE_BLOCK(x-1, y+4)) {
            players[current_player].x -= 1;
            players[current_player].y += 2;
        // pripadne jde jen vlevo, kdyz to nejde dolu
        } else {
            players_move(MOVE_LEFT);
        }
        break;
    case MOVE_DOWNRIGHT:
        // podleze vpravo dolu, dava pozor, aby nespadl
        if(FREE_BLOCK(x+1, y+1) && FREE_BLOCK(x+1, y+2) && FREE_BLOCK(x+1, y+3) && ! FREE_BLOCK(x+1, y+4)) {
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
    if(scene[players[0].x][players[0].y+2].extra == FINISH && scene[players[1].x][players[1].y+2].extra == FINISH)
        return GAME_NEXTLEVEL;
    if(players[0].dead || players[1].dead)
        return GAME_RESET;
    return GAME_CONTINUE;
}



/*
 * pokud pod hracem neni zem, tak spadne
 * pokud spadne moc, umre
 */
void check_fall(int player)
{
    // pokud pod hracem neni podlaha
    if(scene[players[player].x][players[player].y+2].type == FREE) {
        fall[player] += 1;
        players[player].y += 1;
    } else {
        // pokud spadl moc, tak umrel
        if(fall[player] > 3) {
            printf("UMREL JSI !!!  [%d]\n", fall[player]);
            players[player].dead = 1;
        }
        // na pevne zemi se resetuje pocitadlo padu
        fall[player] = 0;
    }
}
