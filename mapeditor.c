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

#if BLOCK_X * SCALE_X == BLOCK_Y * SCALE_Y
    #define BLOCK (BLOCK_X * SCALE_X)
#else
    #warning Block size has to be square
#endif

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
    BLANK = '.',
    WALL = '#',
    BLUE_PLAYER = '1',
    RED_PLAYER = '2',
    BOULDER = 'B',
    FINISH = 'F',
};

int objects[] = { BLANK, WALL, FINISH, BOULDER, BLUE_PLAYER, RED_PLAYER };

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

// 
int colors[] = {
    0xff, // BLANK
    0x999999ff, // WALL
    0xffff00ff, // FINISH
    0xffffff, // BOULDER
    0xffff, // BLUE PLAYER
    0xff0000ff, // RED PLAYER
};

// array to store map
unsigned char map[WIDTH][HEIGHT];


int finish_x = -1, finish_y = -1, player1_x = -1, player1_y = -1, player2_x = -1, player2_y = -1;


// boulders staff
struct boulder_t{ int x, y; };
#define MAX_BOULDERS 50
int boulders_cnt = 0;
struct boulder_t boulders[MAX_BOULDERS];

/**
 *
 */
int new_boulder(int x, int y)
{
    if(boulders_cnt < MAX_BOULDERS) {
        boulders[boulders_cnt].x = x;
        boulders[boulders_cnt].y = y;
        boulders_cnt++;
    } else
        return 0;
}

/**
 *
 */
int find_boulder(int x, int y)
{
    return 1;
}

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
}


/**
 *
 */
void drawSquare(SDL_Surface *scr, int x, int y, int color)
{
    boxColor(scr, 11+x*BLOCK, 11+y*BLOCK, 9+(x+1)*BLOCK, 9+(y+1)*BLOCK, color);
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
            if(map[x][y] == WALL) {
                drawSquare(screen, x, y, colors[P_WALL]);
            }
        }
    }

    // draw boulders
    for(int b = 0; b < boulders_cnt; b++) {
        drawSquare(screen, boulders[b].x, boulders[b].y, colors[P_BOULDER]);
        drawSquare(screen, boulders[b].x, boulders[b].y+1, colors[P_BOULDER]);
        drawSquare(screen, boulders[b].x+1, boulders[b].y, colors[P_BOULDER]);
        drawSquare(screen, boulders[b].x+1, boulders[b].y+1, colors[P_BOULDER]);
    }
    // draw players and finish
    drawSquare(screen, player1_x, player1_y, colors[P_BLUE_PLAYER]);
    drawSquare(screen, player1_x, player1_y+1, colors[P_BLUE_PLAYER]);
    drawSquare(screen, player2_x, player2_y, colors[P_RED_PLAYER]);
    drawSquare(screen, player2_x, player2_y+1, colors[P_RED_PLAYER]);
    drawSquare(screen, finish_x, finish_y, colors[P_FINISH]);
    drawSquare(screen, finish_x+1, finish_y, colors[P_FINISH]);

    redraw_grid(screen);
    
    // update
    SDL_UpdateRect(screen, 0, 0, 0, 0);
}


/**
 *
 */
void delete_map(SDL_Surface *screen, int x, int y, int w, int h)
{
    printf("delete_map %d %d\n", x, y);
    for(int i = x; i < x + w; ++i)
        for(int j = y; j < y + h; ++j) {
            printf(" -- %d %d\n", i, j);
            map[i][j] = objects[P_BLANK];
            drawSquare(screen, i, j, colors[P_BLANK]);
        }
    SDL_UpdateRect(screen, 0, 0, 0, 0);
}

/**
 *
 */
void delete(SDL_Surface *screen, int x, int y, int tool)
{
    // smazat hrace, kameny, cil
    switch(map[x][y]) {
        //case P_BLANK:
        //case P_WALL:
        case 1: break;
/*
        case objects[P_FINISH]:
            delete_map(screen, finish_x, finish_y, 2, 1);
            break;
        case objects[P_BOULDER]:
            // najit vsechny kameny
            // delete_map(screen, x, y, 2, 2);
            break;
        case objects[P_BLUE_PLAYER]:
            delete_map(screen, player1_x, player1_y, 1, 2);
            break;
        case objects[P_RED_PLAYER]:
            delete_map(screen, player2_x, player2_y, 1, 2);
            break;
*/
    }
}


/**
 *
 */
void fill_area_map(SDL_Surface *screen, int x1, int y1, int x2, int y2, int tool)
{
    for(int i = x1; i <= x2; ++i) {
        for(int j = y1; j <= y2; ++j) {
            // remove special objects
            delete(screen, i, j, tool);
            // fill with new object
            drawSquare(screen, i, j, colors[tool]);
            map[i][j]  = objects[tool];
        }
    }
}

/**
 *
 */
void fill_area(SDL_Surface *screen, int x1, int y1, int x2, int y2, int tool)
{
    if(tool != P_BLANK && tool != P_WALL)
        return; // do not fill area with anything else
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
    fill_area_map(screen, (x1-10)/BLOCK, (y1-10)/BLOCK, (x2-10)/BLOCK, (y2-10)/BLOCK, tool); 
    SDL_UpdateRect(screen, 0, 0, 0, 0);
}

/**
 *
 */
void fill_point(SDL_Surface *screen, int x, int y, int tool)
{
    int i = (x-10)/(BLOCK_X * SCALE_X);
    int j = (y-10)/(BLOCK_Y*SCALE_Y);

    // TODO: nakreslit to tam

    switch(tool) {
        case P_BLANK:
        case P_WALL:
            if(map[i][j] == objects[tool])
                return;
            delete(screen, i, j, tool);
            drawSquare(screen, i, j, colors[tool]);
            break;
        case P_FINISH:
            delete_map(screen, finish_x, finish_y, 2, 1);
            drawSquare(screen, i, j, colors[tool]);
            drawSquare(screen, i+1, j, colors[tool]);
            finish_x = i;
            finish_y = j;
            break;
        case P_BLUE_PLAYER:
            delete_map(screen, player1_x, player1_y, 1, 2);
            drawSquare(screen, i, j, colors[tool]);
            drawSquare(screen, i, j+1, colors[tool]);
            player1_x = i;
            player1_y = j;
            break;
        case P_RED_PLAYER:
            delete_map(screen, player2_x, player2_y, 1, 2);
            drawSquare(screen, i, j, colors[tool]);
            drawSquare(screen, i, j+1, colors[tool]);
            player2_x = i;
            player2_y = j;
            break;
        case P_BOULDER:
            new_boulder(i, j);
            drawSquare(screen, i, j, colors[tool]);
            drawSquare(screen, i, j+1, colors[tool]);
            drawSquare(screen, i+1, j, colors[tool]);
            drawSquare(screen, i+1, j+1, colors[tool]);
            break;
    }
    map[i][j] = objects[tool];
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
            switch(c) {
                case BOULDER:
                    new_boulder(x, y);
                    break;
                case WALL:
                case BLANK:
                    break;
                case FINISH:
                    finish_x = x;
                    finish_y = y;
                    break;
                case BLUE_PLAYER:
                    player1_x = x;
                    player1_y = y;
                    break;
                case RED_PLAYER:
                    player2_x = x;
                    player2_y = y;
                    break;
                case EOF:
                    fprintf(stderr, "not valid input file\n");
                    return;
                default:
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
        // wait for event
        if(!SDL_PollEvent(&event)) {
            SDL_Delay(1);
            continue;
        }

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
                    if(tools[0] == P_BLANK || tools[0] == P_WALL)
                        fill_point(screen, event.button.x, event.button.y, tools[0]);
                } else if(event.motion.state & SDL_BUTTON(3)) {
                    if(tools[1] == P_BLANK || tools[1] == P_WALL)
                        fill_point(screen, event.button.x, event.button.y, tools[1]);
                }
                break;

            case SDL_MOUSEBUTTONDOWN: // first corner of area to fill
                click = event.button;
                if(in_pallete_area(event.button.x, event.button.y))
                    change_tool(event.button);
                if(mode == POINT && in_map_area(click.x, click.y)) {
                    if(click.button == SDL_BUTTON_LEFT)
                        fill_point(screen, click.x, click.y, tools[0]);
                    else if(click.button == SDL_BUTTON_RIGHT)
                        fill_point(screen, click.x, click.y, tools[1]);
                }
                    
                break;

            case SDL_MOUSEBUTTONUP: // the other corner of area to fill
                if(mode == POINT)
                    break;
                if(in_map_area(event.button.x, event.button.y) && in_map_area(click.x, click.y)) {
                    int tool;
                    if(event.button.button == SDL_BUTTON_LEFT)
                        tool = tools[0];
                    else if(event.button.button == SDL_BUTTON_RIGHT)
                        tool = tools[1];
                    else
                        break;
                    if(tool == P_WALL || tool == P_BLANK)
                        fill_area(screen, click.x, click.y, event.button.x, event.button.y, tool);
                }
                break;
        }
        SDL_Delay(1);
    } while(event.type != SDL_QUIT);

    save("map_pokus.txt");

    SDL_Quit();
    return 0;
}

// coding of objects:
// 6(x) + 5(y) + 5(extra)
//   extra: type of boulder, player, 

// elevator: start + max + min (x + y y y) coords, which lever 
// boulder: type + (X+y)

// magnet and special boulder?
// disappearing stone?
// flame?
