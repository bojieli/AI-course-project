#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/time.h>
#include "p2.h"

static int check_solution(int num, size_t* solution)
{
    char *used = malloc(num);
    char *l1 = malloc(num*2);
    char *l2 = malloc(num*2);
    memset(used,0,num);
    memset(l1,0,num*2);
    memset(l2,0,num*2);
    int i;
    for (i=0; i<num; i++) {
        if (l1[num+solution[i]-i])
            goto error;
        l1[num+solution[i]-i] = 1;
        if (l2[solution[i]+i])
            goto error;
        l2[solution[i]+i] = 1;
        if (used[solution[i]])
            goto error;
        used[solution[i]] = 1;
    }
    int flag;
    flag = 1;
    goto free;
error:
    flag = 0;
free:
    free(used);
    free(l1);
    free(l2);
    return flag;
}

int main(int argc, char** argv)
{
    if (argc < 2)
        return 1;
    int num = atoi(argv[1]);
    printf("searching %d queens\n", num);
    struct timeval begin, end;
    gettimeofday(&begin, NULL);
    size_t **solution = NQueen(num);
    gettimeofday(&end, NULL);
    printf("3 solutions found in %lf secs\n", end.tv_sec - begin.tv_sec + (end.tv_usec - begin.tv_usec)/1.0E6);
    int i;
    for (i=0; i<3; i++)
        if (check_solution(num, solution[i]))
            printf("solution %d: OK\n", i);
        else
            printf("solution %d: ERROR\n", i);
    return 0;
}
