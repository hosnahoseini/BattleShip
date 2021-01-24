#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<math.h>

//structs

struct point{
    int x;
    int y;
};

struct ship{
    struct point start;
    struct point end;
    int len;
    int destroy;
    int num;
};

//global variable
struct node * ships = NULL;
int row,col;
int ship_map_1[row][col];
int ship_map_2[row][col];
int shot_map_1[row][col];
int shot_map_2[row][col];

//game functions
void help(){
    printf("Battleship (also Battleships or Sea Battle) is a strategy type guessing game for two players.");
    getchar();
    system("cls");
}

void menu (){
    printf("1)Play with a friend\n2)Play with a bot\n3)Load game\n4)Load last game\n5)Setting\n6)Score board\n7)Exit\n");
}

void show_map(char map[row][col]){
    printf("    ");
    for(char c = 'A'; c < ('A' + col); c++)
        printf("| %c ",c);
    printf("|\n");

    for(int i = 0; i < (4*(col+1))+1 ; i++)
        printf("-");
    printf("\n");

    for (int i = 1; i <= row; i++)
    {
        printf("| %d ", i);
        for (int j = 0; j < col; j++)
            printf("| %c ", map[i][j]);
        printf("|\n");

        for(int i = 0; i < (4*(col+1))+1 ; i++)
            printf("-");
        printf("\n");
    }
    