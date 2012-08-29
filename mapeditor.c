/*
 * Quadrax for uzebox map editor
 */

#include <stdio.h>
#include <stdlib.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

#include <SDL/SDL_gfxPrimitives.h>

#define WIDTH 40
#define HEIGHT 28

#define BLOCK_X 6
#define BLOCK_Y 8

#define SCALE_X 4
#define SCALE_Y 3

#define PAL_WIDTH 10 * 6 * SCALE_X
#define PAL_HEIGHT 6 * 8 * SCALE_Y * 2

#define MAP_WIDTH 40 * 6 * SCALE_X
#define MAP_HEIGHT 28 * 8 * SCALE_Y

#define SCREEN_WIDTH (MAP_WIDTH + PAL_WIDTH + 30)
#define SCREEN_HEIGHT (MAP_HEIGHT + 20)

#define STATUSBAR_SIZE 0

#define PADDING 10

const char datafile[] = "data/tiles_big.png";

// text representation of objects
enum {
    BLANK = ' ',
    WALL = '#',
    BLUE_PLAYER = '1',
    RED_PLAYER = '2',
    BOULDER = 'B',
    FINISH = 'F',
};

// drawing modes
enum {
    POINT,
    RECTANGLE,
};

// tools pallete
enum {
    P_BLANK,
    P_WALL,
    P_FINISH,
    P_BOULDER,
    P_BLUE_PLAYER,
    P_RED_PLAYER,
};

// implicit drawing tools
int tools[2] = {P_WALL, P_BLANK};


// array to store map
unsigned char map[WIDTH][HEIGHT];

/**
 *
 */
int in_map_area(int x, int y)
{
    return x > 10 && x < 10 + MAP_WIDTH && y > 10 && y < 10 + MAP_HEIGHT;
}

/**
 *
 */
int in_pallete_area(int x, int y)
{
    int neco = 100;
    return x > 10 + MAP_WIDTH + 10 && x < 10 + MAP_WIDTH + 10 + PAL_WIDTH && y > neco && y < neco + PAL_HEIGHT;
}


/**
 *
 */
void change_tool(SDL_MouseButtonEvent button)
{
    switch(button.button) {
        case SDL_BUTTON_LEFT:
            tools[0] = (button.y - 110) / 48;
            break;
        case SDL_BUTTON_RIGHT:
            tools[1] = (button.y - 110) / 48;
            break;
    }
    printf("tools : %d , %d \n", tools[0], tools[1]);
}


/**
 * 
 */
void redraw_grid(SDL_Surface *screen)
{
    for(int i = 0; i < WIDTH; ++i) {
       vlineColor(screen, 10+BLOCK_X*i*SCALE_X, 10, 10+MAP_HEIGHT-1, 0x663333ff);
    }
    for(int i = 0; i < HEIGHT; ++i) {
       hlineColor(screen, 10, 10+MAP_WIDTH-1, 10+BLOCK_Y*SCALE_Y*i, 0x663333ff);
    }
}

/**
 *
 */
void init_screen(SDL_Surface *screen)
{
    // background
    SDL_FillRect(screen, NULL, 0x00407f);

    // editor area
    SDL_Rect map_area = {.x = 10, .y = 10, .w = MAP_WIDTH, .h = MAP_HEIGHT };
    SDL_FillRect(screen, &map_area, 0x000000);
    
    // preview of selected piece
    SDL_Rect preview_area = {.x = MAP_WIDTH + 20, .y = 10, .w = SCALE_X * 6, .h = SCALE_Y * 8 };
    SDL_FillRect(screen, &preview_area, 0x00ff00);

    // draw loaded map
    for(int x = 0; x < WIDTH; ++x) {
        for(int y = 0; y < HEIGHT; ++y) {
            if(map[x][y] == WALL)
                boxColor(screen, 10+x*BLOCK_X*SCALE_X, 10+y*BLOCK_Y*SCALE_Y,10+(x+1)*BLOCK_X*SCALE_X, 10+(y+1)*BLOCK_Y*SCALE_Y, 0x999999ff);
        }
    }

    redraw_grid(screen);
    
    // update
    SDL_UpdateRect(screen, 0, 0, 0, 0);
}

/**
 *
 */
void fill_area(SDL_Surface *screen, int x1, int y1, int x2, int y2, int which)
{
    int tmp;
    if(x2 < x1) {
        tmp = x1;
        x1 = x2;
        x2 = tmp;
    }
    if(y2 < y1) {
        tmp = y1;
        y1 = y2;
        y2 = tmp;
    }
    for(int i = (x1-10)/(BLOCK_X * SCALE_X); i <= (x2-10)/(BLOCK_X*SCALE_X); ++i) {
        for(int j = (y1-10)/(BLOCK_Y*SCALE_Y); j <= (y2-10)/(BLOCK_Y*SCALE_Y); ++j) {
            if(which) {
                boxColor(screen, 10+i*BLOCK_X * SCALE_X+1, 10+j*(BLOCK_Y*SCALE_Y)+1, 10+(i+1)*BLOCK_X * SCALE_X-1, 10+(j+1)*(BLOCK_Y*SCALE_Y)-1 , 0x000000ff);
                map[i][j]  = '.';
            } else {
                boxColor(screen, 10+i*BLOCK_X * SCALE_X+1, 10+j*(BLOCK_Y*SCALE_Y)+1, 10+(i+1)*BLOCK_X * SCALE_X-1, 10+(j+1)*(BLOCK_Y*SCALE_Y)-1, 0x999999ff);
                map[i][j]  = '#';
            }
        }
    }
    SDL_UpdateRect(screen, 0, 0, 0, 0);
}

/**
 *
 */
void fill_point(SDL_Surface *screen, int x, int y, int which)
{
    int i = (x-10)/(BLOCK_X * SCALE_X);
    int j = (y-10)/(BLOCK_Y*SCALE_Y);
    if(which) {
        if(map[i][j] == '.')
            return;
        boxColor(screen, 10+i*BLOCK_X * SCALE_X+1, 10+j*(BLOCK_Y*SCALE_Y)+1, 10+(i+1)*BLOCK_X * SCALE_X-1, 10+(j+1)*(BLOCK_Y*SCALE_Y)-1 , 0x000000ff);
        map[i][j]  = '.';
    } else {
        if(map[i][j] == '#')
            return;
        boxColor(screen, 10+i*BLOCK_X * SCALE_X+1, 10+j*(BLOCK_Y*SCALE_Y)+1, 10+(i+1)*BLOCK_X * SCALE_X-1, 10+(j+1)*(BLOCK_Y*SCALE_Y)-1 , 0x999999ff);
        map[i][j]  = '#';
    }
    SDL_UpdateRect(screen, 0, 0, 0, 0);
}

/**
 * Loads level
 */
void load(const char * filename)
{
    FILE *f = fopen(filename, "r");
    if(f == NULL) {
        perror("fopen");
        return;
    }

    int c;

    for(int y = 0; y < HEIGHT; ++y)
        for(int x = 0; x < WIDTH; ++x) {
            c = fgetc(f);
            if(c == '\n') {
                x--;
                continue;
            }
            map[x][y] = c;
        }
}

/**
 * Save level
 */
void save(const char * filename)
{
    FILE *f = fopen(filename, "w");
    if(f == NULL) {
        perror("fopen");
        return;
    }
    for(int y = 0; y < HEIGHT; ++y) {
        for(int x = 0; x < WIDTH; ++x) {
            fputc(map[x][y], f);
        }
        fputc('\n', f);
    }
}


/**
 *
 */
int main(int argc, char *argv[])
{
    if(SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "SDL: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Surface * pallete = IMG_Load("data/pallete.png");
    if(pallete == NULL) {
        fprintf(stderr, "Error loading data file %s\n", datafile);
        return 1;
    }

    SDL_Surface *screen;
    SDL_putenv("SDL_VIDEO_CENTERED=1");
    screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, 32, SDL_HWSURFACE);
    if(screen == NULL) {
        fprintf(stderr, "SDL: %s\n", SDL_GetError());
        return 1;
    }
    SDL_WM_SetCaption("Quadrax for uzebox mapeditor", NULL);


    if(argc > 1)
        load(argv[1]);
    else
        load("level00.map");


    // basic graphics
    init_screen(screen);
    // pallete of objects
    SDL_Rect pal_area = {.x = MAP_WIDTH + 20, .y = 10+100, .w = PAL_WIDTH, .h = PAL_HEIGHT };
    // show pallete
    SDL_Rect from = { .x = 0, .y = 0, .w = PAL_WIDTH, .h = PAL_HEIGHT};
    SDL_BlitSurface(pallete, &from, screen, &pal_area);
    SDL_UpdateRect(screen, 0, 0, 0, 0);
    

    SDL_MouseButtonEvent click; // first click
    SDL_Event event;
    Uint8 *keys;

    int mode = POINT;

    do {
        SDL_PollEvent(&event);

        switch(event.type) {
            case SDL_KEYDOWN: // left constroll switches mode
            case SDL_KEYUP:
                if(event.key.keysym.sym == SDLK_LCTRL)
                    mode = event.type == SDL_KEYDOWN; // POINT/RECTANGLE
                break;

            case SDL_MOUSEMOTION: // fill particular place
                if(mode == RECTANGLE)
                    break;
                if(!in_map_area(event.button.x, event.button.y))
                    break;
                if(event.motion.state & SDL_BUTTON(1)) {
                    fill_point(screen, event.button.x, event.button.y, 0);
                } else if(event.motion.state & SDL_BUTTON(3)) {
                    fill_point(screen, event.button.x, event.button.y, 1);
                }
                break;

            case SDL_MOUSEBUTTONDOWN: // first corner of area to fill
                click = event.button;
                if(in_pallete_area(event.button.x, event.button.y))
                    change_tool(event.button);
                if(mode == POINT && in_map_area(click.x, click.y)) {
                    fill_area(screen, click.x, click.y, event.button.x, event.button.y, event.button.button == SDL_BUTTON_RIGHT);
                }
                    
                break;

            case SDL_MOUSEBUTTONUP: // the other corner of area to fill
                if(mode == POINT)
                    break;
                if(in_map_area(event.button.x, event.button.y) && in_map_area(click.x, click.y)) {
                    if(event.button.button == SDL_BUTTON_LEFT || event.button.button == SDL_BUTTON_RIGHT)
                        fill_area(screen, click.x, click.y, event.button.x, event.button.y, event.button.button == SDL_BUTTON_RIGHT);
                }
                break;
        }
        SDL_Delay(1);
    } while(event.type != SDL_QUIT);

    save("map_pokus.txt");

    SDL_Quit();
    return 0;
}


// left and right click different action?
// select sth from pallete
// set player start location
// set finish location
// place boulders
// place other things

// coding of objects:
// 6(x) + 5(y) + 5(extra)
//   extra: type of boulder, player, 

// elevator: start + max + min (x + y y y) coords, which lever 
// boulder: type + (X+y)

// magnet and special boulder?
// disappearing stone?
// flame?
