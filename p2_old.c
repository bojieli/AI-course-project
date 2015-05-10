#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "p2.h"

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

// return 1 if success, 0 otherwise
int solve_queen(int num, size_t* a)
{
    my_srand((unsigned)time(NULL));

    // generate initial placement
    int i;
    for (i=0; i<num; i++)
        a[i] = i;
    for (i=0; i<num; i++) {
        int rand = my_rand() % (num-i) + i;
        size_t t = a[i];
        a[i] = a[rand];
        a[rand] = t;
    }
#define MAX_CONFLICT_NUM 1000
    int l1[MAX_CONFLICT_NUM];
    int l2[MAX_CONFLICT_NUM];
    int head1 = 0, head2 = 0, tail1 = 0, tail2 = 0;
    for (i=0; i<num; i++) {
        if (a[i] == i) {
            if (tail1 == MAX_CONFLICT_NUM)
                return 0;
            l1[tail1++] = i;
        }
        if (a[i] == num-i) {
            if (tail2 == MAX_CONFLICT_NUM)
                return 0;
            l2[tail2++] = i;
        }
    }
    while (tail1-head1 > 1 || tail2-head2 > 1) {
        int try1, try2;
        if (tail1-head1 > 0)
            try1 = l1[head1++];
        else
            try1 = l2[head2++];
        if (tail1-head1 > 0)
            try2 = l1[head1++];
        else
            try2 = l2[head2++];
        // old: (try1, a[try1]), (try2, a[try2])
        // new: (try1, a[try2]), (try2, a[try1])
        int conflicts = (try1 == a[try2]) + (num-try1 == a[try2]) + (try2 == a[try1]) + (num-try2 == a[try1]);
        if (conflicts == 0) { // conflicts are eliminated
            size_t tmp = a[try1];
            a[try1] = a[try2];
            a[try2] = tmp;
        }
        else // conflicts cannot be eliminated
            return 0;
    }
    return 1;
}

size_t **NQueen(size_t queen_number)
{
    int i;
    size_t **solution = malloc(3*sizeof(size_t));
    for (i=0; i<3; i++) {
        solution[i] = malloc(queen_number * sizeof(size_t));
        while (!solve_queen(queen_number, solution[i]));
    }
    return solution;
}
