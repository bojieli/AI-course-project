#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "p1.h"

typedef long __int64;

// state: 4bit for each grid, 1<<36 for the position of space
//long mask[9] = {1,1<<4,1<<8,1<<12,1<<16,1<<20,1<<24,1<<28,1<<32};
//long mask_space = 1<<36;
//long goal = 1 + (4<<4) + (7<<8) + (2<<12) + (5<<16) + (8<<20) + (3<<24) + (6<<28) + (0<<32) + (8<<36);
extern int goal[9];

static long tolong(int* s)
{
    int i;
    long space_pos = 0;
    long board = 0;
    for (i=8; i>=0; i--) {
        board <<= 4;
        board |= (long)s[i];
        if (s[i] == 0)
            space_pos = i;
    }
    board |= (space_pos<<36);
    return board;
}

static char next[4][9] = {
    {-1,0,1,-1,3,4,-1,6,7}, // up
    {1,2,-1,4,5,-1,7,8,-1}, // down
    {3,4,5,6,7,8,-1,-1,-1}, // right
    {-1,-1,-1,0,1,2,3,4,5}  // left
};
#define spacepos(board) (board>>36)
#define nextpos(board,action) next[action][spacepos(board)]
#define move(board,nextpos) ((board & ~((long)0xF<<(nextpos<<2)) & 0xFFFFFFFFF) | (((board>>(nextpos<<2))&0xF)<<(spacepos(board)<<2)) | (nextpos<<36))

static int* long2arr(long board)
{
    int *arr = malloc(9*sizeof(int));
    int i;
    for (i=0; i<9; i++) {
        arr[i] = board & 0xF;
        board >>= 4;
    }
    return arr;
}

static void print_board(long board)
{
    int *arr = long2arr(board);
    int err = 0;
    int i;
    for (i=0; i<9; i++) {
        printf("%d", arr[i]);
        if (arr[i] == 0 && board>>36 != i)
            err = 1;
    }
    if (err)
        printf(" (Error %ld)", board>>36);
    printf("\n");
    free(arr);
}

static char step2char[4] = {'u', 'd', 'r', 'l'};

int* randep(int d)
{
    srand((int)time(NULL));
    int steps = 0;
    long board = tolong(goal);
    while (steps < d) {
        int dir = rand() % 4;
        long nextpos = nextpos(board, dir);
        if (nextpos == -1)
            continue;
//      printf("%c %ld->%ld ", step2char[dir], spacepos(board), nextpos);
        board = move(board, nextpos);
        ++steps;
//      print_board(board);
    }
    return long2arr(board);
}

static char d[9][9] = {
    {0,1,2,1,2,3,2,3,4},
    {1,0,1,2,1,2,3,2,3},
    {2,1,0,3,2,1,4,3,2},
    {1,2,3,0,1,2,1,2,3},
    {2,1,2,1,0,1,2,1,2},
    {3,2,1,2,1,0,3,2,1},
    {2,3,4,1,2,3,0,1,2},
    {3,2,1,2,1,0,3,2,1},
    {4,3,2,3,2,1,2,1,0}
};

static inline int distance(long s, long t)
{
    int pos[9];
    int i=0;
    for (i=0; i<9; i++) {
        pos[s & 0xF] = i;
        s >>= 4;
    }
    int dist = 0;
    for (i=0; i<9; i++) {
        dist += d[i][pos[t & 0xF]];
        t >>= 4;
    }
    return dist;
}

typedef struct list {
    long board;
    int parent;
    int steps;
    char last_step;
    char visited;
} list;

static void reverse(char* s, int len)
{
     int i;
     for (i=0; i<(len>>1); i++) {
         char t;
         t = s[i];
         s[i] = s[len-i-1];
         s[len-i-1] = t;
     }
}

#define MAX_ELEMENTS 1000
list l[MAX_ELEMENTS];

char* idas(int* s, int* g)
{
    long src = tolong(s);
    long dst = tolong(g);
    int d_limit = distance(src, dst);
    while (1) {
        int next_d_limit = 1<<30;
        int top = 0;
        l[0].board = src;
        l[0].visited = 0;
        while (top >= 0) {
            list *s = &l[top];
            if (s->visited) {
                --top;
                continue;
            }
            s->visited = 1;
            int dist = distance(s->board, dst) + s->steps + 1;
//          printf("limit=%d top=%d dist=%d board=", d_limit, top, dist);
//          print_board(s->board);
            if (dist > d_limit) {
                if (dist < next_d_limit)
                    next_d_limit = dist;
            } else {
                if (s->board == dst) {
                    char *steps = malloc(MAX_ELEMENTS);
                    memset(steps,0,MAX_ELEMENTS);
                    int step_count = 0;
                    while (top != 0) {
                        steps[step_count++] = step2char[(int)(l[top].last_step)];
                        top = l[top].parent;
                    }
                    reverse(steps, step_count);
                    steps[step_count] = '\0';
                    return steps;
                }
                int orig_top = top;
                long t;
                int i;
                for (i=0; i<4; i++) {
                    t = nextpos(s->board, i);
                    if (t == -1)
                        continue;
                    ++top; // push stack
                    if (top == MAX_ELEMENTS) // stack full
                        return NULL;
                    l[top].board = move(s->board, t);
                    l[top].parent = orig_top;
                    l[top].steps = s->steps+1;
                    l[top].last_step = i;
                    l[top].visited = 0;
                }
            }
        }
        if (d_limit == next_d_limit) { // no solution
            printf("%d\n", top);
            return NULL;
        }
        d_limit = next_d_limit;
    }
}

long dst; // do not pass dst over recursion
char solution[MAX_ELEMENTS]; // record steps for solution

typedef struct {
    long board;
    int dist;
    char action;
} successor;

// return new limit if failure, -1 if success
int do_rbfs(long src, int src_dist, int step, int limit)
{
    if (src == dst) {
        solution[step] = -1; // indicate end
        return -1;
    }
    int i;
    successor succ[4];
    for (i=0; i<4; i++) {
        long t = nextpos(src, i);
        if (t == -1) {
            succ[i].dist = 1<<30;
            continue;
        }
        succ[i].board = move(src, t);
        succ[i].dist = distance(succ[i].board, dst) + step + 1;
        if (succ[i].dist < src_dist)
            succ[i].dist = src_dist;
        succ[i].action = i;
    }
    while (1) {
        successor *best_1 = (succ[0].dist < succ[1].dist ? &succ[0] : &succ[1]);
        successor *best_2 = (succ[2].dist < succ[3].dist ? &succ[2] : &succ[3]);
        successor *best = (best_1->dist < best_2->dist ? best_1 : best_2);
        if (best->dist > limit)
            return best->dist;
        int saved_best_dist = best->dist;
        best->dist = limit;
        int alter_1 = (succ[0].dist < succ[1].dist ? succ[0].dist : succ[1].dist);
        int alter_2 = (succ[2].dist < succ[3].dist ? succ[2].dist : succ[3].dist);
        int alter_dist = alter_1 < alter_2 ? alter_1 : alter_2;
        best->dist = do_rbfs(best->board, saved_best_dist, step+1, alter_dist);
        if (best->dist == -1) { // success
            solution[step] = best->action;
            return -1;
        }
    }
}

char* rbfs(int* s, int* g)
{
    long src = tolong(s);
    dst = tolong(g);

    int limit = do_rbfs(src, distance(src, dst), 0, 1<<30);
    if (limit != -1) // no solution
        return NULL;
    int i;
    for (i=0; solution[i] != -1; i++)
        solution[i] = step2char[(int)solution[i]];
    solution[i] = '\0';
    return solution;
}
