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
int cmp(const void *i1, const void *i2){
	int a =  ((score_board_info *)i1) ->score;
	int b =  ((score_board_info *)i2) ->score;

	return (a < b) ? 1 : (a == b) ? 0 : -1;
}

void sort_score(){

    FILE * fp = fopen("score copy.bin", "rb");
    score_board_info * score_info;
    int n = 1;
    score_info = (score_board_info *)malloc(sizeof(score_board_info));
    fread(&(score_info[0].name), sizeof(char), 100, fp);
    fread(&(score_info[0].score), sizeof(int), 1, fp);
    
    while (1)
    {
        n ++;
        char name [100];
        int score;
        score_info = (score_board_info *)realloc(score_info, n * sizeof(score_board_info));
        fread(name, sizeof(char), 100, fp);
        fread(&(score), sizeof(int), 1, fp);
        
        if(feof(fp)){
            n --;
            break;
        }
        strcpy(score_info[n - 1].name ,name);
        score_info[n - 1].score = score; 
    }
    fclose(fp);
    
    qsort(score_info, n, sizeof(score_board_info), cmp);
    
    FILE * fw = fopen("score copy.bin", "wb");
    for (int i = 0; i < n; i++)
    {
        fwrite(&(score_info[i].name), sizeof(char), 100, fw);
        fwrite(&(score_info[i].score), sizeof(int), 1, fw);
    }
    fclose(fw);
}                                                        
void score_board(){
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
int main(){
    sort_score();
    score_board();
    
}