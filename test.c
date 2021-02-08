#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#define COL 10
#define ROW 10
typedef struct{
    int x;
    int y;
}point;

typedef struct{
    point start;
    point end;
    int len;
    int destroy;
    int num;
}ship;

struct node{
    ship info;
    struct node *next;
};

typedef struct{
    int len;
    int num;
    int score;
}ship_info;
struct node * create_node(ship ship_info){
    struct node * new = (struct node *)malloc(sizeof(struct node));
    if(new == NULL)
        return NULL;
    new->info = ship_info;
    new->next = NULL;
    return new;
}

void add_end(struct node ** list, struct node * new){
    if(*list == NULL)
        *list = new;
    else{
        struct node * curr;
        for(curr = *list; curr->next != NULL; curr = curr->next);
        curr->next = new;
    }
}

int cnt = 0;
int row = ROW,col = COL;
char ship_map_1[ROW][COL];                                                    //it was declared here just because of init, in the main prog it shoul be in main;
char shot_map_1[ROW][COL];
char ship_map_2[ROW][COL];                                                    //it was declared here just because of init, in the main prog it shoul be in main;
char shot_map_2[ROW][COL];
struct node * ships_list_1 = NULL,* ships_list_2 = NULL;
char name[2][100];
int score[2];                                                               //scores of two players save here --> score[0] = player one scores & score[1] = player 2 scores
ship_info *ShipTypeInfo;                                                        //len, number, scores of each ship type
int number = 4;  
int players;
int turn;   

typedef struct{
    char name [100];
    int score;
}score_board_info;
void f(){
    printf("bye!");
}
int main(){
    atexit(f);
    getchar();
    printf("hi\n");
    
    exit(0);
}