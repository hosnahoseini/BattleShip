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
    FILE * fp = fopen("score.bin","r+b");
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
void save_score(char name[100], int score){
    
    int k = search_in_players(name);

    if(k != -1){
        FILE *fp = fopen("score.bin","r+b");
        printf("%d",k);
        fseek(fp,k, SEEK_SET);
        //int file_score;
        
        fwrite(&score,sizeof(int), 1, fp);
        printf("%d", score);
        fclose(fp);
    }
    else
    {
        FILE * fp = fopen("score.bin","ab");
        fwrite(name, sizeof(char), 100, fp);
        fwrite(&score, sizeof(int), 1, fp);
        fclose(fp);
    }
}
void print_list(struct node * list){
    struct node * curr = list;
    while (curr != NULL)
    {
        printf("%d ", curr->info.len);
        curr = curr->next;
    }
    printf("\n");
}
void load(){
        
        //puts(game_name);
        printf("here");
        FILE * fp = fopen("ma.bin","rb");
        //printf("1\n");
        fread(name,sizeof(char), 100 * 2, fp);
        //printf("2\n");
        fread(&row, sizeof(int), 1, fp);
        fread(&col, sizeof(int), 1, fp);
        //printf("3\n");
        fread(&number, sizeof(int), 1, fp);
        //printf("4\n");
        fread(&score, sizeof(int), 2 * 1, fp);
        //printf("6\n");
        int ListSize1 ;
        int ListSize2 ;
        fread(&ListSize1, sizeof(int), 1, fp);
        fread(&ListSize2, sizeof(int), 1, fp);
        //printf("5\n");
        
        ShipTypeInfo = malloc(sizeof(ship_info) * number);
        fread(ShipTypeInfo, sizeof(ship_info), number, fp);
        //printf("7\n");
        fread(shot_map_1, sizeof(char), row * col, fp);
        fread(shot_map_2, sizeof(char), row * col, fp);
        //printf("8\n");
        //ships_list_1 = ships_list_2 = NULL;
        ship new_info;
        for(int i =0 ;i< ListSize1; i++){
            fread(&new_info, sizeof(ship), 1, fp);
            add_end(&ships_list_1,create_node(new_info));
        }
        //printf("8\n");
        for(int i =0 ;i< ListSize2; i++){
            fread(&new_info, sizeof(ship), 1, fp);
            add_end(&ships_list_2,create_node(new_info));
        }
        //printf("9\n");
        fread(&players, sizeof(int), 1, fp);
        //printf("10\n");
        fread(&turn, sizeof(int), 1, fp);
        fclose(fp);
    
}
void show_map(char map[row][col],char name[100]){
    printf("%s \'s map: \n", name);
    printf("    ");
    for(char c = 'A'; c < ('A' + col); c++)
        printf("| %c ",c);
    printf("|\n");

    for(int i = 0; i < (4*(col+1))+1 ; i++)
        printf("-");
    printf("\n");

    for (int i = 0; i < row; i++)
    {
        printf("|%2d ", i + 1);
        for (int j = 0; j < col; j++)
            printf("| %c ", map[i][j]);
        printf("|\n");

        for(int i = 0; i < (4*(col+1))+1 ; i++)
            printf("-");
        printf("\n");
    }
}
void play_back(int turn, char * name){
    turn ++;
    char map[row][col];
    char add[1];
    sprintf(add, "%d", turn);
    char filename[100] = "playback";
    strcat(filename,add);
    puts(filename);
    FILE * fp = fopen(filename, "rb");
    fseek(fp,0,SEEK_END);
    while (1)
    {
        fseek(fp, -1 * sizeof(char) * row * col , SEEK_CUR);
        fread(map, sizeof(char), row * col, fp);
        show_map(map, name);
        fseek(fp, -1 * sizeof(char) * row * col , SEEK_CUR);
        
        if(ftell(fp) == 0)
            break;
    }
    fclose(fp);
}
int main(){
    //play_back(0,"you");
    printf("%d",remove("playback2.bin"));
    printf("%d",remove("third.bin"));
}