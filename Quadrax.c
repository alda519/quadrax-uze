#include <avr/io.h>
#include <stdlib.h>
#include <avr/pgmspace.h>
#include <uzebox.h>
 
#include "data/fonts.pic.inc"
#include "data/tiles.inc"


#define WIDTH 40
#define HEIGHT 28
#include "data/levels.h"

typedef union {
    unsigned char block;
    struct {
        unsigned char extra : 7;
        unsigned char type : 1; // NOTE that this bit is MSB
    } adv;
} map_t;

// current level scene
map_t scene[WIDTH][HEIGHT];

// symbolic names for tiles from tiletable
enum {
    BLANK, WALL, PLAYER_BLUE_HEAD, PLAYER_RED_HEAD, BOULDER_TL, BOULDER_TR, SWITCH1_ON, SWITCH2_ON, XXX1, XXX2,
    FINISH_L, FINISH_R, PLAYER_BLUE_LEGS, PLAYER_RED_LEGS, BOULDER_BL, BOULDER_BR, SWITCH1_OFF, SWITCH2_OFF,
} tiles;

// symbolic names for block throuhgput
enum { FREE, SOLID };

// game return status codes
enum { GAME_NEXTLEVEL, GAME_RESET, GAME_END, };

// true if given block is not solid
#define FREE_BLOCK(x, y) (scene[x][y].adv.type == FREE)

// struct describing player
typedef struct {
    unsigned char x, y;
    char walk, fall;
} player_t;

// array contains both players parameters
player_t players[2];

// is anyone dead?
unsigned char dead = 0;

// finish coordinates
unsigned char finish_x, finish_y;

#define MAX_BOULDERS 20

// boulder description
typedef struct boulder_t {
    unsigned char x, y;
    unsigned char fall;
} boulder_t;

// array of boulders
boulder_t boulders[MAX_BOULDERS];
unsigned char boulders_cnt = 0;

// data stored in eeprom support
struct EepromBlockStruct eeprom_data;
enum { LAST_LEVEL, MAX_LEVEL };
#define EEPROM_ID 30

/**************** LEVELS **************/

int new_boulder(int, int);

void level1(void)
{
    for(int x = 0; x < WIDTH; x++) {
        for(int y = 0; y < HEIGHT; y++) {
            if(x == 0 || y == 0 || x == WIDTH-1|| y == HEIGHT-1)
                scene[x][y].block = WALL | 0x80;
            else
                scene[x][y].block = BLANK;
        }
    }
    for(int i = 0; i < 5; ++i) {
        scene[5-i][26-i].block = WALL | 0x80;
        scene[35+i][26-i].block = WALL | 0x80;


        scene[24-i][15].block = WALL | 0x80;
    }

    new_boulder(25, 25);
    new_boulder(15, 25);
    new_boulder(21, 13);
    
    scene[24][23].block = WALL | 0x80;
    scene[23][24].block = WALL | 0x80;
    scene[25][21].block = WALL | 0x80;
    scene[24][19].block = WALL | 0x80;
    scene[25][17].block = WALL | 0x80;
    
    players[0].x = 23;
    players[0].y = 13;
    players[1].x = 10;
    players[1].y = 25;
    finish_x = 16;
    finish_y = 21;
    scene[finish_x][finish_y].block = WALL | 0x80;
    scene[finish_x+1][finish_y].block = WALL | 0x80;
}

void level2(void)
{
    for(int x = 0; x < WIDTH; x++) {
        for(int y = 0; y < HEIGHT; y++) {
            if(x == 0 || y == 0 || x == WIDTH-1|| y == HEIGHT-1)
                scene[x][y].block = WALL | 0x80;
            else
                scene[x][y].block = BLANK;
        }
    }
    players[0].x = 7;
    players[0].y = 25;
    players[1].x = 10;
    players[1].y = 25;
    finish_x = 16;
    finish_y = 26;
    scene[finish_x][finish_y].block = WALL | 0x80;
    scene[finish_x+1][finish_y].block = WALL | 0x80;
}
void level3(void)
{
    for(int x = 0; x < WIDTH; x++) {
        for(int y = 0; y < HEIGHT; y++) {
            if(x == 0 || y == 0 || x == WIDTH-1|| y == HEIGHT-1)
                scene[x][y].block = WALL | 0x80;
            else
                scene[x][y].block = BLANK;
        }
    }
    players[0].x = 7;
    players[0].y = 25;
    players[1].x = 10;
    players[1].y = 25;
    finish_x = 16;
    finish_y = 25;
    scene[finish_x][finish_y].block = WALL | 0x80;
    scene[finish_x+1][finish_y].block = WALL | 0x80;
}

void (*levels[])() = {level1, level2, level3};

/************** END LEVELS ************/


/**
 * Inits boulders
 */
void init_boulders(void) {
    boulders_cnt = 0;
}

/**
 * Adds new boulder
 */
int new_boulder(int x, int y) {
    if(boulders_cnt < MAX_BOULDERS) {
        boulders[boulders_cnt].x = x;
        boulders[boulders_cnt].y = y;
        boulders[boulders_cnt].fall = 0;
        boulders_cnt += 1;
        scene[x][y].adv.type = SOLID;
        scene[x][y+1].adv.type = SOLID;
        scene[x+1][y].adv.type = SOLID;
        scene[x+1][y+1].adv.type = SOLID;
        return 1;
    } else {
        return 0;
    }
}


/**
 * Returns true if boulder is found on given coordinates
 */
int find_boulder(int x, int y) {
    for(int b = 0; b < boulders_cnt; ++b)
        if(boulders[b].x == x && boulders[b].y == y)
            return b+1;
    return 0;
}

/**
 * Boulders also fall if they do not stand on ground
 */
void boulders_fall(void) {
    for(int b = 0; b < boulders_cnt; b++) {
        if(FREE_BLOCK(boulders[b].x, boulders[b].y+2) && FREE_BLOCK(boulders[b].x+1, boulders[b].y+2)) {
            SetTile(boulders[b].x, boulders[b].y, BLANK);
            SetTile(boulders[b].x+1, boulders[b].y, BLANK);
            scene[boulders[b].x][boulders[b].y].adv.type = FREE;
            scene[boulders[b].x+1][boulders[b].y].adv.type = FREE;
            scene[boulders[b].x][boulders[b].y+2].adv.type = SOLID;
            scene[boulders[b].x+1][boulders[b].y+2].adv.type = SOLID;
            boulders[b].y += 1;
            boulders[b].fall = 1;
        } else {
            boulders[b].fall = 0;
        }
        // what if boulder falls on someones head
        for(int p = 0; p < 2; ++p)
            if((players[p].x == boulders[b].x || players[p].x == boulders[b].x+1) && players[p].y == boulders[b].y + 1)
                dead = 1;
    }
}

/**
 * Tells whether there is any falling boulder on given coordinates
 */
int falling_boulder(int x, int y)
{
    int b = find_boulder(x, y);
    return b && boulders[b-1].fall;
}

/**
 * This function asks user to select level
 * Returns number of selected level
 */
int get_start_level()
{
    SetTileTable(fonts);
    ClearVram();
    Print(4, 2, PSTR("SELECT LEVEL"));

    // load highest level from eeprom, player is not able to skip levels
    int max_level = eeprom_data.data[MAX_LEVEL];
    if(max_level == 0)
        max_level = 1;

    if(max_level > LEVELS)
        max_level = LEVELS;

    // print level list
    int x = 4, y = 4;
    for(int i = 1; i <= max_level; ++i) {
        PrintByte(x, y, i, 0);
        if(i % 9 == 0) {
            x = 4;
            y += 3;
        } else {
            x += 4;
        }
    }

    // try to load last played level from eeprom
    int level = eeprom_data.data[LAST_LEVEL];
    if(level < 1)
        level = 1;
    if(level > max_level)
        level = max_level;

    // compute starting position
    x = 5 + ((level-1) % 9)*4;
    y = 4 + ((level-1) / 9)*3;

    int button;
    do {
        button = ReadJoypad(0);
        PrintChar(x, y, ' ');
        PrintChar(x-3, y, ' ');
        if(button & BTN_LEFT) {
            if(level > 1)
                level--;
        } else if(button & BTN_RIGHT) {
            if(level < max_level)
                level++;
        } else if(button & BTN_UP) {
            if(level > 9)
                level -= 9;
        } else if(button & BTN_DOWN) {
            if(level < max_level - 8)
                level += 9;
        }
        x = 5 + ((level-1) % 9)*4;
        y = 4 + ((level-1) / 9)*3;
        PrintChar(x, y, '<');
        PrintChar(x-3, y, '>');
        WaitVsync(4);
    } while(! (button & BTN_START));

    return level;
}


/**
 * Loads given level from flash to RAM
 */
void load_level(int level)
{
    // pgm_read_byte
    levels[level-1]();

    players[0].walk = 0;
    players[0].fall = 0;
    players[1].walk = 0;
    players[1].fall = 0;
    dead = 0;
}


/**
 * Moves given player according to pressed buttons and terrain
 */
void move_player(int player, int buttons)
{
    // players cannot move while falling
    if(players[player].fall)
        return;

    int x = players[player].x;
    int y = players[player].y;
    if((buttons & (BTN_LEFT | BTN_UP)) == (BTN_LEFT | BTN_UP)) {
        // climbs up left
        if(FREE_BLOCK(x-1,y-1) && FREE_BLOCK(x-1, y-2) && FREE_BLOCK(x, y-1) && !FREE_BLOCK(x-1,y)) {
            players[player].x -= 1;
            players[player].y -= 2;
        } else {
            // if it is not possible to climb, then try move left
            move_player(player, BTN_LEFT);
        }

    } else if((buttons & (BTN_LEFT | BTN_DOWN)) == (BTN_LEFT | BTN_DOWN)) {
        // climbs down left
        if(FREE_BLOCK(x-1, y+1) && FREE_BLOCK(x-1, y+2) && FREE_BLOCK(x-1, y+3) && ! FREE_BLOCK(x-1, y+4)) {
            players[player].x -= 1;
            players[player].y += 2;
        } else {
            // if he cannost climb down, then try it left
            move_player(player, BTN_LEFT);
        }

    } else if((buttons & (BTN_RIGHT | BTN_UP)) == (BTN_RIGHT | BTN_UP)) {
        // climbs up right
        if(FREE_BLOCK(x+1,y-1) && FREE_BLOCK(x+1, y-2) && FREE_BLOCK(x, y-1) && !FREE_BLOCK(x+1,y)) {
            players[player].x += 1;
            players[player].y -= 2;
        } else { // pokud to nejde nahoru, tak zkusi jit aspon jen vpravo
            move_player(player, BTN_RIGHT);
        }

    } else if((buttons & (BTN_RIGHT | BTN_DOWN)) == (BTN_RIGHT | BTN_DOWN)) {
        // climbs down right
        if(FREE_BLOCK(x+1, y+1) && FREE_BLOCK(x+1, y+2) && FREE_BLOCK(x+1, y+3) && ! FREE_BLOCK(x+1, y+4)) {
            players[player].x += 1;
            players[player].y += 2;
        } else {
            // if he cannost climb down, then try it right
            move_player(player, BTN_RIGHT);
        }

    } else if(buttons & BTN_LEFT) {
        if(FREE_BLOCK(x-1,y) && FREE_BLOCK(x-1,y+1) && FREE_BLOCK(x-1,y+2) && !FREE_BLOCK(x-1,y+3) && !falling_boulder(x-2,y+3)) {
            players[player].x -= 1;
            players[player].y += 1;
        } else if(FREE_BLOCK(x-1,y) && FREE_BLOCK(x-1,y+1) && !(FREE_BLOCK(x-1,y+2) && FREE_BLOCK(x-1,y+3) && FREE_BLOCK(x-1, y+4) && FREE_BLOCK(x-1, y+5)) && !falling_boulder(x-2, y+2) && !falling_boulder(x-2, y+3) && !falling_boulder(x-2, y+4) && !falling_boulder(x-2, y+5))
            players[player].x -= 1;
        else if(FREE_BLOCK(x-1, y) && FREE_BLOCK(x-1, y-1) && FREE_BLOCK(x, y-1) && !FREE_BLOCK(x-1,y+1) && !falling_boulder(x-2, y+1)) {
            players[player].x -= 1;
            players[player].y -= 1;
        } else if(FREE_BLOCK(x-3, y) && FREE_BLOCK(x-3, y+1)) {
            // the other player might block pushing boulder
            if(players[(player+1)%2].x == x-3 && (players[(player+1)%2].y == y || players[(player+1)%2].y == y+1))
                ;
            else {
            int b = find_boulder(x-2, y);
            // check boulder put on top of boulder
            if(b && !find_boulder(x-3, y-2) && !find_boulder(x-2, y-2) && !find_boulder(x-1, y-2)) {
                boulders[b-1].x -= 1;
                scene[x-1][y].adv.type = FREE;
                scene[x-1][y+1].adv.type = FREE;
                scene[x-3][y].adv.type = SOLID;
                scene[x-3][y+1].adv.type = SOLID;
                players[player].x -= 1;
            }
            }
        }

    } else if(buttons & BTN_RIGHT) {
        if(FREE_BLOCK(x+1,y) && FREE_BLOCK(x+1,y+1) && FREE_BLOCK(x+1,y+2) && !FREE_BLOCK(x+1,y+3) && !falling_boulder(x+1,y+3)) {
            players[player].x += 1;
            players[player].y += 1;
        } else if(FREE_BLOCK(x+1, y) && FREE_BLOCK(x+1, y+1) && !(FREE_BLOCK(x+1,y+2) && FREE_BLOCK(x+1,y+3) && FREE_BLOCK(x+1, y+4) && FREE_BLOCK(x+1, y+5)) && !falling_boulder(x+1, y+2) && !falling_boulder(x+1, y+3) && !falling_boulder(x+1, y+4) && !falling_boulder(x+1, y+5)) {
            players[player].x += 1;
            players[player].walk += 1;
        } else if(FREE_BLOCK(x+1, y) && FREE_BLOCK(x+1, y-1) && FREE_BLOCK(x, y-1) && !FREE_BLOCK(x+1,y+1) && !falling_boulder(x+1, y+1)) {
            players[player].x += 1;
            players[player].y -= 1;
        } else if(FREE_BLOCK(x+3, y) && FREE_BLOCK(x+3, y+1)) {
            // the other player might block pushing boulder
            if(players[(player+1)%2].x == x+3 && (players[(player+1)%2].y == y || players[(player+1)%2].y == y+1))
                ;
            else {
            int b = find_boulder(x+1, y);
            // check boulder put on top of boulder
            if(b && !find_boulder(x, y-2) && !find_boulder(x+1, y-2) && !find_boulder(x+2, y-2)) {
                boulders[b-1].x += 1;
                scene[x+1][y].adv.type = FREE;
                scene[x+1][y+1].adv.type = FREE;
                scene[x+3][y].adv.type = SOLID;
                scene[x+3][y+1].adv.type = SOLID;
                players[player].x += 1;
            }
            }
        }

    } /*else if(buttons & BTN_UP) {
        players[player].y -= 1;

    } else if(buttons & BTN_DOWN) {
        players[player].y += 1;
    }*/
    else {
        players[player].walk = 0;
        return; // hackity hack, no moving = no erasing
    }
    // erase players previous position
    SetTile(x, y, BLANK);
    SetTile(x, y+1, BLANK);
}

/**
 * If player does not stand on ground, he falls
 */
void fall_player(int player)
{
    if(scene[players[player].x][players[player].y+2].adv.type == 0) {
        SetTile(players[player].x, players[player].y, BLANK);
        players[player].y += 1;
        players[player].fall += 1;
    } else {
        if(players[player].fall > 3)
            dead = 1;
        // reset counter on ground
        players[player].fall = 0;
    }
}


/**
 * Draw only wall on screen
 */
void draw_walls(void)
{
    for(int x = 0; x < 40; x++)
        for(int y = 0; y < 28; y++)
               SetTile(x, y, scene[x][y].adv.extra);
}

/**
 * Draw entire screen again
 */
void redraw(void)
{
    // boulders
    for(int b = 0; b < boulders_cnt; ++b)
        DrawMap2(boulders[b].x, boulders[b].y, map_boulder);

    // levers ...
    /*
    SetTile(23,26, SWITCH1_ON);
    SetTile(27,26, SWITCH1_OFF);
    SetTile(15,25, SWITCH2_ON);
    SetTile(18,25, SWITCH2_OFF);
    */
    // players
    if(players[0].walk > 4)
        players[0].walk = 1;

    switch(players[0].walk) {
        case 0:
            DrawMap2(players[0].x, players[0].y, map_playerBlue);
            break;

        case 1: // walk right
            DrawMap2(players[0].x, players[0].y, map_playerBlue_m1r);
            break;
        case 2: // walk right
            DrawMap2(players[0].x, players[0].y, map_playerBlue_m2r);
            break;
        case 3: // walk right
            DrawMap2(players[0].x, players[0].y, map_playerBlue_m3r);
            break;
        case 4: // walk right
            DrawMap2(players[0].x, players[0].y, map_playerBlue_m2r);
            break;
    }
    if(players[0].fall) {
        DrawMap2(players[0].x, players[0].y, map_playerBlue_f1);
        DrawMap2(players[0].x, players[0].y, map_playerBlue_f1);
    }

    DrawMap2(players[1].x, players[1].y, map_playerRed);
    // finish
    DrawMap2(finish_x, finish_y, map_finish);
    // lifts ..
}


/**
 * Runs selected level
 */
int play_level(int level)
{
    init_boulders();
    load_level(level);
    draw_walls();
    int buttons = 0;
    char swap = 0;
    int prevbtns = 0;
    do {
        redraw();
        WaitVsync(3);
        // for each player
        for(int player = 1; player >= 0; --player) {
            buttons = ReadJoypad(player);
            fall_player(player^swap);
            move_player(player^swap, buttons);
        }
        // boulders are falling
        boulders_fall();
        // player 0 can reset game
        if(buttons & BTN_START)
            return GAME_RESET;
        // player can swap controllers by A button
        if((prevbtns ^ buttons) & buttons & BTN_A)
            swap = (swap + 1) & 1;
        // check finish
        if((players[0].x == finish_x || players[0].x == finish_x+1) && players[0].y == finish_y-2 && (players[1].x == finish_x || players[1].x == finish_x+1) && players[1].y == finish_y-2) 
            return GAME_NEXTLEVEL;
        // restart game if someone is dead
        if(dead)
            return GAME_RESET;
        // back to menu
        if(buttons & BTN_SELECT)
            return GAME_END;
        prevbtns = buttons;
    } while(1);
}


/**
 * Main!
 */
int main()
{
    srand(1);

    // read eeprom data
    EepromReadBlock(EEPROM_ID, &eeprom_data);
    eeprom_data.id = EEPROM_ID;

    unsigned char level = 0;
    unsigned char game_status;
    // infinite loop of selecting levels and starting them
    do {
        level = get_start_level();
        SetTileTable(quadTiles);
        ClearVram();

        // restarting level loop
        do {
            // store to EEPROM last played level
            eeprom_data.data[LAST_LEVEL] = level;
            EepromWriteBlock(&eeprom_data);
            // play level
            game_status = play_level(level);

            if(game_status == GAME_NEXTLEVEL) {
                level += 1;
                // store to EEPROM highest played level
                if(level > eeprom_data.data[MAX_LEVEL]) {
                    eeprom_data.data[MAX_LEVEL] = level;
                    EepromWriteBlock(&eeprom_data);
                }
            } else if(game_status == GAME_END)
                break;
        } while(level < LEVELS + 1);

        if(level > LEVELS) {
            // TODO: congratz, you win all levels
        }
    } while(1);
}
