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
void score_board(){
    //sort_score();
    FILE * fp = fopen("score copy.bin","rb");
    char name [100];
    int score;
    printf("|          name           | score |\n");
    printf("-----------------------------------\n");
    while (1)
    {
       
        fread(name, sizeof(char), 100, fp);
        fread(&score, sizeof(int), 1, fp);
         if (feof(fp))
            break;
        printf("|          %-15s|   %4d|\n", name, score);
    }
    printf("press enter to go back to menu: ");
    fflush(stdin);
    getchar();
    system("cls");
    fclose(fp);
    
}

int search_in_players(char name[100]){
    FILE * fp = fopen("score copy.bin","r+b");
    if(fp == NULL)
        return -1;
    
    char file_name [100];
    while (1)
    {
        fread(file_name, sizeof(char), 100, fp);
        if(strcmp(name, file_name) == 0){
            return ftell(fp);
        }
        if(feof(fp))
            break;
        fseek(fp, sizeof(int), SEEK_CUR);
        
    }
    fclose(fp);
    return -1;
}
/*void save_score(char name[100], int score){
    
        FILE * fp = fopen("score.bin","ab");
        fwrite(name, sizeof(char), 100, fp);
        fwrite(&score, sizeof(int), 1, fp);
        fclose(fp);

}*/
void save_score(char name[100], int score){
    
    int k = search_in_players(name);

    if(k != -1){
        FILE *fp = fopen("score copy.bin","r+b");
        printf("%d",k);
        fseek(fp,k, SEEK_SET);
        //int file_score;
        
        fwrite(&score,sizeof(int), 1, fp);
        printf("%d", score);
        fclose(fp);
    }
    else
    {
        FILE * fp = fopen("score copy.bin","ab");
        fwrite(name, sizeof(char), 100, fp);
        fwrite(&score, sizeof(int), 1, fp);
        fclose(fp);
    }
}
int main(){
    save_score("1",100);
    score_board();
}