#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
//#include <sys/time.h>
#include "p2.h"
#include <windows.h>

static unsigned m_w, m_z;
static unsigned my_rand()
{
    m_z = 36969 * (m_z & 65535) + (m_z >> 16);
    m_w = 18000 * (m_w & 65535) + (m_w >> 16);
    return (m_z << 16) + m_w;  /* 32-bit result */
}
static void my_srand(unsigned seed)
{
    m_z = seed & 65535;
    m_w = seed >> 16;
    int i;
    for (i=0; i<100; i++)
        my_rand();
}

/*
typedef struct set_node {
    int begin;
    int end;
    struct set_node* left;
    struct set_node* right;
    struct set_node* parent;
} set_node;
set_node* _set;

static void set_init(int num)
{
    _set = malloc(sizeof(set_node));
    _set->begin = 0;
    _set->end = num;
    _set->left = NULL;
    _set->right = NULL;
    _set->parent = NULL;
}

static int set_rand()
{
    set_node* cur = _set;
    while (cur->left != NULL) {
        int leftn = cur->left->end - cur->left->begin;
        int rightn = cur->right->end - cur->right->begin;
        if (my_rand() % (leftn + rightn) < leftn)
            cur = cur->left;
        else
            cur = cur->right;
    }
    return my_rand() % (cur->end - cur->begin) + cur->begin;
}

static void set_delete(int n)
{
    // find the node
    set_node* cur = _set;
    while (cur->left != NULL) {
        if (cur->left->end > n)
            cur = cur->left;
        else
            cur = cur->right;
    }
    // if only one, delete it and merge parent
    if (cur->end - cur->begin == 1) {
        set_node* p = cur->parent;
        if (p == NULL) { // the last node of the set is deleted
            free(cur);
            return;
        }
        set_node* nextp = (p->left == cur ? p->right : p->left);
        if (p->parent == NULL)
            _set = nextp;
        else if (p->parent->left == p)
            p->parent->left = nextp;
        else
            p->parent->right = nextp;
        nextp->parent = p->parent;
        free(p);
        free(cur);
    }
    else if (cur->begin == n) { // the smallest one
        ++cur->begin;
    }
    else if (cur->end == n+1) { // the biggest one
        --cur->end;
    }
    else { // split nodes
        cur->left = malloc(sizeof(set_node));
        cur->left->begin = cur->begin;
        cur->left->end = n;
        cur->left->left = cur->left->right = NULL;
        cur->left->parent = cur;
        
        cur->right = malloc(sizeof(set_node));
        cur->right->begin = n+1;
        cur->right->end = cur->end;
        cur->right->left = cur->right->right = NULL;
        cur->right->parent = cur;
    }
}
*/

int set_size;
int *_set;

static void set_init(int num)
{
    set_size = num;
    _set = malloc(num*sizeof(int));
    int i;
    for (i=0; i<num; i++)
        _set[i] = i;
    for (i=0; i<num; i++) {
        int rand = my_rand() % (num-i) + i;
        size_t t = _set[i];
        _set[i] = _set[rand];
        _set[rand] = t;
    }
}

#define set_rand() (my_rand() % set_size)
#define set_get(pos) (_set[pos])
#define set_delete(pos) (_set[pos] = _set[--set_size])

// return 1 if success, 0 otherwise
int solve_queen(int num, size_t* a)
{
    //struct timeval begin, end;
    //gettimeofday(&begin, NULL);

    int *l1 = malloc(num*sizeof(int)*2);
    int *l2 = malloc(num*sizeof(int)*2);
    int *col = malloc(num*sizeof(int));

    // generate initial placement
    int i,j,counter=0;
#define SIMPLE_MAXNUM 1000
    if (num < SIMPLE_MAXNUM) {
        for (i=0; i<num; i++)
            a[i] = i;
        for (i=0; i<num; i++) {
            int rand = my_rand() % (num-i) + i;
            size_t t = a[i];
            a[i] = a[rand];
            a[rand] = t;
        }
    } else {
        memset(l1,0,num*sizeof(int)*2);
        memset(l2,0,num*sizeof(int)*2);
#define MAGIC_CONSTANT 30
#define INIT_NUM (num<SIMPLE_MAXNUM ? 0 : num - MAGIC_CONSTANT)
        // first generate INIT_NUM non-conflict queens
        set_init(num);
        int pos, now;
        for (i=0; i<INIT_NUM; i++) {
            do {
                pos = set_rand();
                now = set_get(pos);
                if (counter++ > (num*3+(num>>1))) { // too many trials
                    //printf("random failed (%d), retry\n", counter);
                    return 0;
                }
            } while (l1[num+i-now] || l2[i+now]);
            set_delete(pos);
            a[i] = now;
            l1[num+i-now] = 1;
            l2[i+now] = 1;
        }
        for (i=INIT_NUM; i<num; i++) {
            pos = set_rand();
            a[i] = set_get(pos);
            set_delete(pos);
        }
    }
    //gettimeofday(&end, NULL);
    //printf("Random (%d) complete in %lf secs\n", counter, end.tv_sec - begin.tv_sec + (end.tv_usec - begin.tv_usec)/1.0E6);
    //begin = end;

    // build stats
    memset(l1,0,num*sizeof(int)*2);
    memset(l2,0,num*sizeof(int)*2);
    memset(col,0,num*sizeof(int));
    for (j=0; j<num; j++) {
        l1[num+a[j]-j]++;
        l2[j+a[j]]++;
        col[a[j]] = j;
    }

    // iterate and try
    int next_j = INIT_NUM;
#define MAX_RETRY (MAGIC_CONSTANT<<1)
    for (i=0; i<MAX_RETRY; i++) {
        for (j=next_j; j<num; j++)
            if (l1[num+a[j]-j] > 1 || l2[j+a[j]] > 1)
                break;
        if (j == num && next_j == INIT_NUM) { // no conflicts
            //gettimeofday(&end, NULL);
            //printf("%d iterations complete in %lf secs\n", i, end.tv_sec - begin.tv_sec + (end.tv_usec - begin.tv_usec)/1.0E6);
            return 1;
        }
        int swap = j;
#define conflicts_num(x) (l1[num+a[x]-x] + l2[a[x]+x])
        // int now_conflicts = conflicts_num(swap);
        int min_conflicts = 1<<30;
        int min_j = -1;
        for (j=0; j<num; j++) { // move swap to column j ?
            register int r = col[j]; // which row is the original
            register int a_r = a[r];
            register int a_swap = a[swap];
            // old: (swap, a[swap]), (r, a[r])
            // new: (swap, a[r]), (r, a[swap])
            // int old_conflicts = now_conflicts + conflicts_num(r);
            int new_conflicts = l1[num+a_r-swap] + l2[a_r+swap] + l1[num+a_swap-r] + l2[a_swap+r];
            if (new_conflicts < min_conflicts) {
                min_conflicts = new_conflicts;
                min_j = j;
            }
        }
        if (min_j == -1) { // no improvement
            next_j = min_j+1; // no improvement in [0,min_j]. Next time, search from here
            continue;
        }
        // (swap, a[swap]) <=> (col[min_j], a[col[min_j]])
        int r = col[min_j];
        --l1[num+a[swap]-swap];
        --l2[a[swap]+swap];
        --l1[num+a[r]-r];
        --l2[a[r]+r];
        ++l1[num+a[r]-swap];
        ++l2[a[r]+swap];
        ++l1[num+a[swap]-r];
        ++l2[a[swap]+r];
        col[a[swap]] = r;
        col[a[r]] = swap;
        a[r] = a[swap];
        a[swap] = min_j;
        next_j = INIT_NUM;
    }
    return 0;
}

size_t **NQueen(size_t queen_number)
{
    my_srand((unsigned)time(NULL));
    size_t **solution = malloc(3*sizeof(size_t));
    int i;
    for (i=0; i<3; i++) {
        solution[i] = malloc(queen_number * sizeof(size_t));
        while (!solve_queen(queen_number, solution[i]));
    }
    return solution;
}
