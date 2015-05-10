#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "p1.h"

int goal[9] = {1,4,7,2,5,8,3,6,0};

static void print_route(char* route)
{
    if (route == NULL)
        printf("No route found.\n");
    else
        printf("Found route [length %d]: %s\n", (int)strlen(route), route);
}

int main(int argc, char** argv)
{
    if (argc < 2)
        return 1;
    int steps = atoi(argv[1]);
    int i;
    printf("Goal: ");
    for (i=0;i<9;i++)
        printf("%d",goal[i]);
    printf("\n");
    int *board = randep(steps);
    printf("Initial Board: ");
    for (i=0;i<9;i++)
        printf("%d",board[i]);
    printf("\n");
    printf("IDA* search...\n");
    char* route = idas(board, goal);
    print_route(route);
    printf("RBFS search...\n");
    route = rbfs(board, goal);
    print_route(route);
    return 0;
}
