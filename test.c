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
int turn;                                                           //number of ships type
int getLinkedListSize(struct node * list){
    int cnt = 0;
    struct node * curr = list;
    while(curr != NULL){
        curr = curr -> next;
        cnt ++;
    }
    return cnt;
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

void write_info_in_file(FILE * fp){
    fwrite(name,sizeof(char), 100 * 2, fp);

        fwrite(&row, sizeof(int), 1, fp);
        fwrite(&col, sizeof(int), 1, fp);

        fwrite(&number, sizeof(int), 1, fp);

        fwrite(&score, sizeof(int), 2 * 1, fp);
        
        int ListSize1 = getLinkedListSize(ships_list_1);
        int ListSize2 = getLinkedListSize(ships_list_2);
        fwrite(&ListSize1, sizeof(int), 1, fp);
        fwrite(&ListSize2, sizeof(int), 1, fp);

        fwrite(ShipTypeInfo, sizeof(ship_info), number, fp);

        fwrite(shot_map_1, sizeof(char), row * col, fp);
        fwrite(shot_map_2, sizeof(char), row * col, fp);

        struct node * curr = ships_list_1;
        while (curr != NULL) {
            fwrite(&(curr->info), 1, sizeof(curr->info), fp);
            curr = curr->next;
            }

        curr = ships_list_2;
        while (curr != NULL) {
            fwrite(&(curr->info), 1, sizeof(curr->info), fp);
            curr = curr->next;
            }
        fwrite(&players, sizeof(int), 1, fp);
        fwrite(&turn, sizeof(int), 1, fp);
}
void save(){
    char save;
    char filename[100];
    printf("Do you want to save the game? ");
    scanf(" %c", &save);
    if (save == 'y'){
        printf("Enter game name: ");
        scanf(" %s", filename);

        FILE * files = fopen("filesname.bin","ab");
        if(files == NULL)
            files = fopen("filesname.bin","wb");

        fwrite(filename, sizeof(char), 100, files);

        fclose(files);
        strcat(filename,".bin");
        
        FILE * last = fopen("last.bin","wb");
        FILE * new = fopen(filename,"wb");
        write_info_in_file(last);
        write_info_in_file(new);
        fclose(last);
        fclose(new);
    }
}
char * print_game(){
    printf("trying to print game\n");
    FILE * fp = fopen("filesname.bin", "rb");
    char game_name[100];
    char *name_p = malloc(100 * sizeof(char));
    static int cnt = 0;
    while (1)
    {   
        
        fread(game_name, sizeof(char), 100, fp);
        printf("here\n")  ;
        cnt ++;
        if(feof(fp))
            break;   
        printf("|%2d|%25s|\n", cnt, game_name);
        
        
    }
    printf("Enter name of the game which you want to play: ");
    scanf(" %s", name_p);
    
    return name_p;
}
void show_map(char map[row][col]){
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

void load(/*char * game_name*/){
        //strcat(game_name,".bin");
        //puts(game_name);
        FILE * fp = fopen("mb.bin","rb");
        printf("1\n");
        fread(name,sizeof(char), 100 * 2, fp);
        printf("%s %s\n", name[0],name[1]);
        fread(&row, sizeof(int), 1, fp);
        fread(&col, sizeof(int), 1, fp);
        printf("%d %d\n", row,col);
        fread(&number, sizeof(int), 1, fp);
        printf("%d\n", number);
        int ListSize1 ;
        int ListSize2 ;
        fread(&ListSize1, sizeof(int), 1, fp);
        fread(&ListSize2, sizeof(int), 1, fp);
        printf("%d %d\n",ListSize1, ListSize2);
        fread(score, sizeof(int), 2 * 1, fp);
        printf("%d %d\n", score[0], score[1]);
        ShipTypeInfo = malloc(sizeof(ship_info) * number);
        fread(ShipTypeInfo, sizeof(ship_info), number, fp);
        printf("7\n");
        fread(shot_map_1, sizeof(char), row * col, fp);
        fread(shot_map_2, sizeof(char), row * col, fp);
        show_map(shot_map_1);
        show_map(shot_map_2);
        //ships_list_1 = ships_list_2 = NULL;
        ship new_info;
        for(int i =0 ;i< ListSize1; i++){
            fread(&new_info, sizeof(ship), 1, fp);
            add_end(&ships_list_1,create_node(new_info));
        }
        print_list(ships_list_1);
        for(int i =0 ;i< ListSize2; i++){
            fread(&new_info, sizeof(ship), 1, fp);
            add_end(&ships_list_2,create_node(new_info));
        }
        print_list(ships_list_2);
        fread(&players, sizeof(int), 1, fp);
        printf("10\n");
        fread(&turn, sizeof(int), 1, fp);
        fclose(fp);
    
}
int main(){
    //score_board();
    
    load();
    print_list(ships_list_2);

}