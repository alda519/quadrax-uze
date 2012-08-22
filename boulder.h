/*
 * Quadrax
 */

#ifndef BOULDER_H
#define BOULDER_H

typedef struct boulder_t {
    int x;
    int y;
    struct boulder_t * next;
} boulder_t;

#define MAX_BOULDERS 30

extern int boulders_cnt;
extern boulder_t boulders[MAX_BOULDERS];


int init_boulders(void);
int new_boulder(int, int);
int del_boulder(int);
int find_boulder(int, int);
void boulders_fall(void);

#endif
