#include <avr/io.h>
#include <stdlib.h>
#include <avr/pgmspace.h>
#include <uzebox.h>
 
//#include "data/fonts.pic.inc"
 #include "data/tiles.inc"


enum {
    BLANK,
    WALL,
    PLAYER_BLUE_HEAD,
    PLAYER_RED_HEAD,
    BOLDER_TL,
    BOLDER_TR,
    SWITCH1_ON,
    SWITCH2_ON,
    FINISH_L,
    FINISH_R,
    PLAYER_BLUE_LEGS,
    PLAYER_RED_LEGS,
    BOLDER_BL,
    BOLDER_BR,
    SWITCH1_OFF,
    SWITCH2_OFF,
} tiles;

// okno ma 40x28

int main(){
    SetTileTable(quadTiles);

    srand(1);
    ClearVram();

start:

    for(int x = 0; x < 40; x++) {
        for(int y = 0; y < 28; y++) {
            if(x == 0 || y == 0 || x == 39 || y == 27)
               SetTile(x, y, WALL);
        }
    }
    
    SetTile(23,26, SWITCH1_ON);
    SetTile(27,26, SWITCH1_OFF);
    SetTile(15,25, SWITCH2_ON);
    SetTile(18,25, SWITCH2_OFF);

    DrawMap2(17, 25, map_playerBlue);
    DrawMap2(25, 25, map_playerRed);
    DrawMap2(30, 25, map_boulder);
    DrawMap2(5, 27, map_finish);


    int www = 5;

    for(int x = 5; x < 32; ++x)
        SetTile(x, 22, WALL);

    DrawMap2(5, 20, map_playerBlue);
    WaitVsync(www);
    for(int x = 5; x < 30; ++x) {
        DrawMap2(x, 20, map_playerBlue_m1);
        WaitVsync(www);
        x++;
        SetTile(x-1, 20, 0);
        SetTile(x-1, 21, 0);
        DrawMap2(x, 20, map_playerBlue_m2);
        WaitVsync(www);
        DrawMap2(x, 20, map_playerBlue_m3);
        WaitVsync(www);
        SetTile(x, 20, 0);
        SetTile(x, 21, 0);
        DrawMap2(x+1, 20, map_playerBlue_m2);
        WaitVsync(www);
    }
    DrawMap2(31, 20, map_playerBlue);

goto start;

    while(1);
}
