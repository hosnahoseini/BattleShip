#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<math.h>
#include<time.h>
#include<windows.h>
#include<string.h>

#define COL 10
#define ROW 10

//structs

typedef struct{
    int x;
    int y;
}point;

typedef struct{
    point start;                
    point end;
    int len;
    int destroy;                //number of destroyed cells
    int num;
}ship;                          //info of each ship

struct node{
    ship info;
    struct node *next;
};

typedef struct{
    int len;
    int num;
    int score;
}ship_info;                    //info of each ship "type"

typedef struct{
    char name [100];
    int score;
}score_board_info;

//global variable

int row = ROW,col = COL;
char ship_map_1[ROW][COL];                                                  //player1 ship places                                                    
char shot_map_1[ROW][COL];                                                  //player1 shot places
char ship_map_2[ROW][COL];                                                  //player2 ship places    
char shot_map_2[ROW][COL];                                                  //player2 shot places
struct node * ships_list_1 = NULL,* ships_list_2 = NULL;
char name[2][100];
int score[2];                                                               //scores of two players save here --> score[0] = player one scores & score[1] = player 2 scores
ship_info *ShipTypeInfo;                                                    //len, number, scores of each ship type
int number = 4;                                                             //number of ships type
int players;                                                                //show number of real plyers: if players == 1 --> play with bot //if players == 2 --> play with friends
int turn;   
void shot_loop_players();
void shot_loop_playerbot();

//linked list functions

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

void delete_node(struct node ** list, struct node * del){
    if(del == *list){
        *list = (*list)->next;
        free(del);
        return;
    }
    struct node * curr = (*list) ->next;
    struct node * prev = (*list);
    while (curr != NULL)
    {
        if(curr == del){
            prev->next = curr->next;
            free(curr);
            return;
        }
        curr = curr->next;
        prev = prev->next;
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

//game functions

void help(){
    printf("Battleship (also Battleships or Sea Battle) is a strategy type guessing game for two players.\npress any key to continue...");
    getchar();
    system("cls");
}

void menu (){                                                                   //show menu;
    printf("1)Play with a friend\n2)Play with a bot\n3)Load game\n4)Load last game\n5)Setting\n6)Score board\n7)Exit\n");
}

void init_ship_info (ship_info *ShipTypeInfo){                                 //initialize ship types info(default value);

    ShipTypeInfo[0].len = 1;
    ShipTypeInfo[0].num = 4;
    ShipTypeInfo[0].score = 25;
    ShipTypeInfo[1].len = 2;
    ShipTypeInfo[1].num = 3;
    ShipTypeInfo[1].score = 12;
    ShipTypeInfo[2].len = 3;
    ShipTypeInfo[2].num = 2;
    ShipTypeInfo[2].score = 8;
    ShipTypeInfo[3].len = 5;
    ShipTypeInfo[3].num = 1;
    ShipTypeInfo[3].score = 5;

}

void show_map(char map[row][col],char name[100]){
    printf("%s \'s map: \n", name);

    printf("    ");                                                             //first line of table
    for(char c = 'A'; c < ('A' + col); c++)
        printf("| %c ",c);
    printf("|\n");

    for(int i = 0; i < (4*(col+1))+1 ; i++)                                     //-------------------
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

void empty_map(char map[row][col]){
    for(int i = 0; i < row ; i ++)
        for(int j = 0; j < col; j++)
            map[i][j] = '\0';
}

// save
int search_in_players(char name[100]){                                           //search a name in score.bin(<-format: name1 score1 name2 score2 ...)
    FILE * fp = fopen("score.bin","r+b");
    
    if(fp == NULL)                                                               //no saved players
        return -1;
    
    char file_name [100];
    while (1)
    {
        fread(file_name, sizeof(char), 100, fp);
        if(strcmp(name, file_name) == 0){
            return ftell(fp);                                                   //return place of that name in score.bin
        }
        
        if(feof(fp))
            break;
        
        fseek(fp, sizeof(int), SEEK_CUR);
        
    }
    fclose(fp);
    return -1;                                                                 //not found
}

void save_score(char name[100], int score){
    int k = search_in_players(name);

    if(k != -1){                                                              //reapeated name --> replace new score with old one
        FILE *fp = fopen("score.bin","r+b");
        fseek(fp,k, SEEK_SET);
        
        fwrite(&score,sizeof(int), 1, fp);
        fclose(fp);
    }
    else                                                                      //new score --> add to end of score.bin
    {
        FILE * fp = fopen("score.bin","ab");
        fwrite(name, sizeof(char), 100, fp);
        fwrite(&score, sizeof(int), 1, fp);
        fclose(fp);
    }
}

int cmp(const void *i1, const void *i2){
	int a =  ((score_board_info *)i1) ->score;
	int b =  ((score_board_info *)i2) ->score;

	return (a < b) ? 1 : (a == b) ? 0 : -1;                                 //compare score for decending sort
}

void sort_score(){

    FILE * fp = fopen("score.bin", "rb");
    score_board_info * score_info;
    int n = 1;
    score_info = (score_board_info *)malloc(sizeof(score_board_info));    //read first score + name and save it in score.bin
    fread(&(score_info[0].name), sizeof(char), 100, fp);                   
    fread(&(score_info[0].score), sizeof(int), 1, fp);
    
    while (1)                                                            //read all the info in score.bin and save them in array of struct
    {
        n ++;
        char name [100];
        int score;
        score_info = (score_board_info *)realloc(score_info, n * sizeof(score_board_info));
        fread(name, sizeof(char), 100, fp);                             //read
        fread(&(score), sizeof(int), 1, fp);
        
        if(feof(fp)){
            n --;
            break;
        }
        strcpy(score_info[n - 1].name ,name);                           //save
        score_info[n - 1].score = score; 
    }
    fclose(fp);
    
    qsort(score_info, n, sizeof(score_board_info), cmp);               //sort array
    
    FILE * fw = fopen("score.bin", "wb");
    for (int i = 0; i < n; i++)                                       //save sorted array in score.bin
    {
        fwrite(&(score_info[i].name), sizeof(char), 100, fw);
        fwrite(&(score_info[i].score), sizeof(int), 1, fw);
    }
    fclose(fw);
}  

void score_board(){                                                 //show score board
    FILE * fp = fopen("score.bin","rb");
    
    if(fp == NULL){                                                 //empty
        printf("empty!\n");
        Sleep(500);
        system("cls");
        return;
    }
    else
        rewind(fp);
    
    sort_score();
    
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
        printf("|          %-15s|   %-4d|\n", name, score);
    }
    printf("press enter to go back to menu: ");
    fflush(stdin);
    getchar();
    system("cls");
    fclose(fp);
    
}

void save_map(char map[row][col],int turn){                         //for saving shot map in playback files (turn = 0/1)
    FILE * fp;
    
    char add[1];                                                    //make name of the file
    sprintf(add, "%d", turn + 1);
    char filename[100] = "playback";
    strcat(filename,add);
    strcat(filename,".bin");

    fp = fopen(filename, "ab");;
    fwrite(map, sizeof(char), row * col, fp);                       //save

    fclose(fp);
}

void play_back(int turn_to_add, char * name){
    char map[row][col];
    char add[1];

    sprintf(add, "%d", turn_to_add + 1);                            //make name of the file
    char filename[100] = "playback";
    strcat(filename,add);
    strcat(filename,".bin");

    FILE * fp = fopen(filename, "rb");

    if(fp == NULL){                                                //the player don't have any move
        empty_map(shot_map_1);
        show_map(shot_map_1, name);
    }
    
    else{
        fseek(fp,0,SEEK_END);                                     //read from end of the file 
        while (1)
        {
            fseek(fp, -1 * sizeof(char) * row * col , SEEK_CUR);
            fread(map, sizeof(char), row * col, fp);
            show_map(map, name);
            Sleep(1000);
            system("cls");
            fseek(fp, -1 * sizeof(char) * row * col , SEEK_CUR);
            
            if(ftell(fp) == 0)
                break;
        }
        fclose(fp);

        empty_map(shot_map_1);
        show_map(shot_map_1, name);                             //show empty map
    }
}

bool choose_from_user(char player_name[], int * player_score){
    FILE * fp = fopen("score.bin","rb");

    if(fp == NULL){
        printf("empty!\n");
        Sleep(500);
        system("cls");
        return false;
    }

    else
        rewind(fp);

    char name [100];
    int score;
    int cnt = 0;

    printf("|##|          name           |\n");
    printf("-----------------------------\n");
    while (1)
    {
       
        fread(name, sizeof(char), 100, fp);
        cnt ++;
        fread(&score, sizeof(int), 1, fp);

         if (feof(fp))
            break;

        printf("|%2d|          %-15s|\n", cnt, name);
    }
    printf("Enter # of user: ");
    int n;
    scanf("%d", &n);

    fseek(fp, (n - 1) * (sizeof(int) + 100 * sizeof(char)),SEEK_SET);
    fread(name, sizeof(char), 100, fp);
    fread(&score, sizeof(int), 1, fp);

    strcpy(player_name, name);
    (*player_score) = score;

    system("cls");
    fclose(fp);
    return true;
    
}

int getLinkedListSize(struct node * list){
    int cnt = 0;
    struct node * curr = list;
    while(curr != NULL){
        curr = curr -> next;
        cnt ++;
    }
    return cnt;
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
            fwrite(&(curr->info), sizeof(curr->info), 1, fp);
            curr = curr->next;
            }

        curr = ships_list_2;
        while (curr != NULL) {
            fwrite(&(curr->info), sizeof(curr->info), 1, fp);
            curr = curr->next;
            }

        fwrite(&players, sizeof(int), 1, fp);
        fwrite(&turn, sizeof(int), 1, fp);
}

void save(){

        char filename[100];
        printf("Enter game name: ");
        scanf(" %s", filename);

        FILE * files = fopen("filesname.bin","ab");                 //write name of the file in filesname.bin to use for loading them
        fwrite(filename, sizeof(char), 100, files);
        fclose(files);

        strcat(filename,".bin");
        FILE * last = fopen("last.bin","wb");                       //save last game
        FILE * new = fopen(filename,"wb");                          //add new game

        write_info_in_file(last);
        write_info_in_file(new);

        fclose(last);
        fclose(new);
}

char * print_game(){                                                //print saved games
    
    FILE * fp = fopen("filesname.bin", "rb");
    
    if(fp == NULL){
        printf("empty!\n");
        Sleep(500);
        system("cls");
        return "\0";
    }
    else
        rewind(fp);

    char game_name[100];
    char *name_p = malloc(100 * sizeof(char));
    int cnt = 0;
    while (1)
    {   
        
        fread(game_name, sizeof(char), 100, fp);
        cnt ++;
        if(feof(fp))
            break;   
        printf("|%2d|         %12s|\n", cnt, game_name);
        
        
    }
    printf("Enter name of the game which you want to play: ");
    scanf(" %s", name_p);
    
    return name_p;                                                 //return name of the game we want to play
}

void load(char * game_name){
    
        strcat(game_name,".bin");
        
        FILE * fp = fopen(game_name,"rb");
        
        fread(name,sizeof(char), 100 * 2, fp);
        
        fread(&row, sizeof(int), 1, fp);
        fread(&col, sizeof(int), 1, fp);
        
        fread(&number, sizeof(int), 1, fp);
        
        fread(&score, sizeof(int), 2 * 1, fp);
        
        int ListSize1 ;
        int ListSize2 ;
        fread(&ListSize1, sizeof(int), 1, fp);
        fread(&ListSize2, sizeof(int), 1, fp);
        
        
        ShipTypeInfo = malloc(sizeof(ship_info) * number);
        fread(ShipTypeInfo, sizeof(ship_info), number, fp);
        
        fread(shot_map_1, sizeof(char), row * col, fp);
        fread(shot_map_2, sizeof(char), row * col, fp);
        
        ship new_info;
        for(int i =0 ;i< ListSize1; i++){
            fread(&new_info, sizeof(ship), 1, fp);
            add_end(&ships_list_1,create_node(new_info));
        }
        
        for(int i =0 ;i< ListSize2; i++){
            fread(&new_info, sizeof(ship), 1, fp);
            add_end(&ships_list_2,create_node(new_info));
        }
        
        fread(&players, sizeof(int), 1, fp);
        
        fread(&turn, sizeof(int), 1, fp);

        fclose(fp);
    
}
void in_game_menu(){
    char choice;
    char filename[100];
    printf("press \'s\' for save, press \'e\' for exit, press \'l\' for load other game, press any other key(except white space) to shot: ");
    fflush(stdin);
    choice = getchar();

    char* load_game;

    switch (choice)
    {
    case 's':
        save();
        break;
    case 'e':
        exit(0);
    case 'l':
        load_game = print_game();
        if(strcmp(load_game,"\0") == 0)
            break;
        else{
        load(load_game);
            if(players == 2)
                    shot_loop_players();
                else
                    shot_loop_playerbot();
            break;
        }
    default:
        break;
    }
}

void swap_point(point *p1, point *p2){
    point tmp = *p1;
    *p1 = *p2;
    *p2 = *p1;
}

bool isvalid_ship_point(point start, point end, int len, char ship_map[row][col]){
    if(end.x > col || start.x > col || end.y > row || start.y > row             //out of table
     ||end.x < 0 || start.x < 0 || end.y < 0 || start.y < 0)    
        return false;

    if(end.x - start.x + 1 != len && end.y - start.y + 1 != len)                //check len
        return false;
    
    for (int i = start.x; i <= end.x ; i++){
        for(int j = start.y; j <= end.y ; j++){                                 //check 9 cell for each point around it
            if(ship_map[i][j] != '\0')
                return false;
            if(i + 1 < row && ship_map[i + 1][j] != '\0')
                return false;
            if(j + 1 < col && ship_map[i][j + 1] != '\0')
                return false;
            if(i - 1 >= 0 && ship_map[i - 1][j] != '\0')
                return false;
            if(j - 1 >= 0 && ship_map[i][j - 1] != '\0')
                return false;
            if(j - 1 >= 0 && i - 1 >= 0 && ship_map[i - 1][j - 1] != '\0')
                return false;
            if(j - 1 >= 0 && i + 1 < row && ship_map[i + 1][j - 1] != '\0')
                return false;
            if(j + 1 < col && i - 1 >= 0 && ship_map[i - 1][j + 1] != '\0')
                return false;
            if(j + 1 < col && i + 1 < row && ship_map[i + 1][j + 1] != '\0')
                return false;
        }
    }
    return true;

}

void get_ships(struct node **ships_list, char ship_map[row][col], char name[100]){                    
    int ship_index = 1;
    printf("Enter start and end point of your ships:\n");
    
    for (int i = 0; i < number; i++)                                               //loop for differrnt len
    {
        printf("Info of ships with len = %d:\n",ShipTypeInfo[i].len);

        for (int j = 0; j < ShipTypeInfo[i].num; j++)                             //loop for ships with same len
        {

            int IsValid = 0;                                                 //for cheking validation of points
            while(IsValid == 0){                                             //getting start point ship[ship_index]
                point start, end;
                char start_y, end_y;
                printf("Start point of ship #%d with len %d (x, y): ", j + 1,ShipTypeInfo[i].len);
                scanf("%d %c",&(start.x), &(start_y));
                printf("End point of ship #%d with len %d (x, y): ", j + 1,ShipTypeInfo[i].len);
                scanf("%d %c",&(end.x), &(end_y));
                system("cls");

                start.y = start_y - 'A';                                    //change input to -> 0 < number < row 
                end.y = end_y - 'A';
                start.x --;                                                 //change input to -> 0 < number < col
                end.x --;

                if(start.x > end.x || start.y > end.y)                      //put the larger point for end(to avoid abs())
                    swap_point(&start, &end);

                if(isvalid_ship_point(start, end, ShipTypeInfo[i].len, ship_map)){
                    ship new_ship;                                                           //create new node
                    new_ship.len = ShipTypeInfo[i].len;
                    new_ship.destroy = 0;
                    new_ship.start = start;
                    new_ship.end = end;
                    new_ship.num = ship_index;
                    add_end(ships_list,create_node(new_ship));                             //add to list

                    for (int k = start.x; k <= end.x ; k++)                                //mark ships place in matrix ship_map 
                        for(int m = start.y; m <= end.y ; m++)
                            ship_map[k][m] = 'X';

                    IsValid = 1;
                    ship_index ++;
                }
                else
                    printf("Invalid point! try again\n");
            }
        
        }
    
    }
    show_map(ship_map, name);
    Sleep(500);
    system("cls");
}

void get_ships_auto(struct node **ships_list, char ship_map[row][col], char name[100]){
    empty_map(ship_map);
    (*ships_list) = NULL;                                                      //delete previous one!
                    
    int ship_index = 1;
    time_t t=time(NULL);
    srand(t);

    for (int i = 0; i < number; i++)                                           //loop for differrnt len
    {
        //Info of ships with len ShipTypeInfo[i].len
        for (int j = 0; j < ShipTypeInfo[i].num; j++)                         //loop for ships with same len
        {

            int IsValid = 0;                                                 //for cheking validation of points
            while(IsValid == 0){                                             //getting start point ship[ship_index]
                point start, end;
                int horizontal_or_vetical = rand() % 2;                      //horizontal_or_vetical = 0 --> horizontal
                                                                             //horizontal_or_vetical = 1 -->vertical

                if(horizontal_or_vetical){
                    start.x = end.x = rand() % row;
                    start.y = rand() % (col - ShipTypeInfo[i].len + 1);
                    end.y = start.y + ShipTypeInfo[i].len - 1;
                }
                else
                {
                    start.y = end.y = rand() % col;
                    start.x = rand() % (row - ShipTypeInfo[i].len + 1);
                    end.x = start.x + ShipTypeInfo[i].len - 1;
                }

                if(isvalid_ship_point(start, end, ShipTypeInfo[i].len, ship_map)){
                    ship new_ship;                                         //create new node
                    new_ship.len = ShipTypeInfo[i].len;
                    new_ship.destroy = 0;
                    new_ship.start = start;
                    new_ship.end = end;
                    new_ship.num = ship_index;
                    add_end(ships_list,create_node(new_ship));                             //add to list

                    for (int k = start.x; k <= end.x ; k++)                                //mark ships place in matrix ship_map 
                        for(int m = start.y; m <= end.y ; m++)
                            ship_map[k][m] = 'X';

                    IsValid = 1;
                    ship_index ++;
                }
            }
        
        }
    
    }
    show_map(ship_map, name);
}

bool shot(char shot_map[row][col],struct node ** ships_list, int turn, point p){           //apply changes of shot
    
    if (shot_map[p.x][p.y] == '\0' && p.x < col && p.x >= 0 && p.y < row && p.y >= 0){     //correct point
        
        struct node *curr = *ships_list;
        while (curr != NULL)                                                               //check for each ship in list
        {
            if((p.x <= curr->info.end.x && p.x >= curr->info.start.x && p.y == curr->info.start.y)
                 || (p.y <= curr->info.end.y && p.y >= curr->info.start.y && p.x == curr->info.start.x)){
                //^ check if the point is on a ship -> E / C
                (curr->info.destroy) ++;
                
                if(curr->info.destroy == curr->info.len){                               //p is the last remaining cell of a ship
                    
                    for (int i = curr->info.start.x; i <= curr->info.end.x ; i++)       //all the cells os the ship ->C
                        for(int j = curr->info.start.y; j <= curr->info.end.y ; j++)
                            shot_map[i][j] = 'C';

                    for (int i = curr->info.start.x - 1; i <= curr->info.end.x + 1 ; i++)
                        for(int j = curr->info.start.y - 1; j <= curr->info.end.y + 1; j++){
                            if(i >= 0 && j >= 0 && i < col && j < row && shot_map[i][j] != 'C')
                                shot_map[i][j] = 'W';                                 //cells around the ship -> W
                        }

                    delete_node(ships_list, curr);

                    int addscore;
                    for(int i = 0; i < number; i++)
                        if(ShipTypeInfo[i].len == curr->info.len)
                            score[turn % 2] += (1 + ShipTypeInfo[i].score);         //score for complete destruction of shops

                    return true;
                    
                }
                shot_map[p.x][p.y] = 'E';
                score[turn % 2] ++;                                           //one point for each successful shot
                
                return true;
            }
            curr = curr->next;
        }

        shot_map[p.x][p.y] = 'W';
        return true;
    }
    else{
        printf("wrong point !\n");                                                  //was chosen=) already
        return false;
    }
}
void end_of_game(){
    char PlayBack;

    printf("%s wins!!!\n",name[(turn-1) % 2] );                                    //announce winner
    score[turn % 2] /= 2;                                                           //looser score
    Sleep(1000);
    system("cls");
    
    printf("%s score = %d\n%s score = %d\n", name[0], score[0], name[1], score[1]);
    fflush(stdin);

    printf("press enter to continue ...");
    getchar();
    system("cls");

    printf("Do you want to see the playback?(y/n): ");
    scanf(" %s", &PlayBack);
    system("cls");

    if(PlayBack == 'y'){
        printf("%s play back", name[0]);
        play_back(0, name[0]);
        printf("press enter to continue ...");
        fflush(stdin);
        getchar();
        system("cls");
        play_back(1, name[1]);
        printf("press enter to continue ...");
        fflush(stdin);
        getchar();
        system("cls");
    }
}

void shot_loop_players(){
    
    point p;
    char p_y;
    int wrong;
    while(ships_list_1 != NULL && ships_list_2 != NULL){

        if(turn % 2 == 0){                                                          //player1 
            do
            {
                wrong = 0;
                show_map(shot_map_1, name[turn % 2]);

                printf("%s is your turn:\n", name[turn % 2]);
                in_game_menu();
                printf("Enter your shot: ");
                scanf("%d %c", &p.x,&p_y);
                system("cls");
                p.y = p_y - 'A';
                (p.x)--;

                if(shot(shot_map_1, &ships_list_2, turn, p) == false){              //invalid shot
                    wrong = 1;
                    continue;
                }

                show_map(shot_map_1, name[turn % 2]);
                save_map(shot_map_1, turn % 2);                                     //for play back

                printf("%s score = %d\n", name[turn % 2], score[turn % 2]);
                Sleep(1000);
                system("cls");
            } while ((shot_map_1[p.x][p.y] != 'W' || wrong) && ships_list_1 != NULL && ships_list_2 != NULL);
            //          ^bouns                      ^invalid
            turn ++;
        }

        else                                                                        //player2
        {
             do
            {
                wrong = 0;
                show_map(shot_map_2, name[turn % 2]);
                printf("%s is your turn:\n", name[turn % 2]);
                in_game_menu();
                printf("Enter your shot: ");
                scanf("%d %c", &p.x,&p_y);
                system("cls");
                p.y = p_y - 'A';
                (p.x)--;

                if(shot(shot_map_2, &ships_list_1, turn, p) == false){
                    wrong = 1;
                    continue;
                }

                show_map(shot_map_2, name[turn % 2]);
                save_map(shot_map_2, turn % 2);

                printf("%s score = %d\n", name[turn % 2], score[turn % 2]);
                Sleep(1000);
                system("cls");
            } while ((shot_map_2[p.x][p.y] != 'W' || wrong) && ships_list_1 != NULL && ships_list_2 != NULL);
            
            turn ++;
            
        }
    }
    end_of_game();
    
}

void shot_loop_playerbot(){
    
    point p;
    char p_y;
    int wrong;
    time_t t=time(NULL);
    srand(t);

    while(ships_list_1 != NULL && ships_list_2 != NULL){
        if(turn % 2 == 0){                                                              //player turn
            do
            {
                wrong = 0;
                show_map(shot_map_1, name[turn % 2]);
                printf("%s is your turn:\n", name[turn % 2]);
                in_game_menu();
                printf("Enter your shot: ");
                scanf("%d %c", &p.x,&p_y);
                system("cls");
                p.y = p_y - 'A';
                (p.x)--;

                if(shot(shot_map_1, &ships_list_2, turn, p) == false){              //invalid shot
                    wrong = 1;
                    continue;
                }

                show_map(shot_map_1, name[turn % 2]);
                save_map(shot_map_1, turn % 2);

                printf("%s score = %d\n", name[turn % 2], score[turn % 2]);
                Sleep(1000);
                system("cls");
            } while (shot_map_1[p.x][p.y] != 'W' && ships_list_1 != NULL && ships_list_2 != NULL);
            
            turn ++;
        }
        else                                                                    //bot turn
        {
             do
            {
                do
                {
                    p.x = rand() % col;
                    p.y = rand() % row;
                } while (shot_map_2[p.x][p.y] != '\0');                       //not chosen before

                shot(shot_map_2, &ships_list_1, turn, p);
                show_map(shot_map_2, name[turn % 2]);
                save_map(shot_map_2, turn % 2);

                printf("%s score = %d\n", name[turn % 2], score[turn % 2]);
                Sleep(1000);
                system("cls");
            } while (shot_map_2[p.x][p.y] != 'W' && ships_list_1 != NULL && ships_list_2 != NULL);
            
            turn ++;
            
        }
    }
    end_of_game();
}

bool check_setting(){
    int sum = 0;
    for (int i = 0; i < number; i ++){
        sum += ( (ShipTypeInfo[i].len + 2) * 3 ) * ShipTypeInfo[i].num;
    }

    if((row + 2) * (col + 2) >= sum )
        return true;

    return false;
}

void setting (){
    printf("1)ships\n2)map size\n");
    int choice;
    bool check;
    scanf("%d", &choice);
    Sleep(500);
    system("cls");

    if(choice == 1){
        check = true;
        do{
            if(check == false)
                printf("Wong!Try again\n");

            printf("Enter info:\n");            
            ShipTypeInfo =(ship_info *) malloc(sizeof(ship_info));
            number = 1;
            int max = 0;                                                                            //to calculate score
            char done;
            do{
                ShipTypeInfo =(ship_info *) realloc(ShipTypeInfo,number * sizeof(ship_info));
                printf("Enter new len: ");
                scanf("%d" , &ShipTypeInfo[number - 1].len );

                printf("Enter number of ships with len %d: ", ShipTypeInfo[number - 1].len);
                scanf("%d" , &ShipTypeInfo[number - 1].num);

                if (max < ShipTypeInfo[number - 1].len)
                    max = ShipTypeInfo[number - 1].len;
                number ++;

                getchar();
                printf("done?(y/n) ");
                scanf("%c", &done);

                Sleep(200);
                system("cls");

            }while(done == 'n');
            number --;

            for(int i = 0; i < number; i++)                                                      //calculate score
                ShipTypeInfo[i].score = 5 * max / ShipTypeInfo[i].len;

            check = check_setting();
        }while(check == false);
         
    }

    else if(choice == 2){
        check = true;
        do{
            if(check == false)
                printf("Wong! Try again\n");

            printf("Enter # of rows: \n");
            scanf("%d", &row);
            printf("Enter # of columns: \n");
            scanf("%d", &col);
            Sleep(500);
            system("cls");

            check = check_setting();
        }while(check == false);
     }

    else{
        printf("Wrong choice! Try again\n");
    }

}

void player_setting(struct node ** ships_list, char ship_map[row][col], char * name, int * score){
    
    int choice1 = 0, choice2, n = 0;                                                                // n is for checking if the user enter both ships and nameor not
    char IsOkay;                                                                                    //to check if the Auto map is satisfying or not?
    
    while(n != 2){
        printf("1) choose user\n2) put ships\n");
        scanf("%d", &choice1);
        system("cls");
        switch (choice1)
        {
        case 1:                                                                                 //user

            printf("1) choose from available users\n2) new user\n");
            scanf("%d", &choice2);
            system("cls");

            switch (choice2)
            {
            case 1:                                                                             //choose from available users
                if(choose_from_user(name, score))
                    n ++;
                break;
            case 2:                                                                             //new user
            while (1)
            {
                printf("Enter your name: ");
                fflush(stdin);
                gets(name);
                if(search_in_players(name) != -1)
                    printf("your is already there try agian\n");
                else
                    break;
            }
                Sleep(1000);
                system("cls");
                n ++;
            default:
                break;
            }

            break;

        case 2:                                                                             //ship
           
            printf("1) Auto\n2) Manual\n");
            scanf("%d", &choice2);
            
            switch (choice2)
            {
            case 1:                                                                         //auto
                do{ 
                    get_ships_auto(ships_list, ship_map, name);
                    printf("Is it okay?(y/n) : ");
                    scanf(" %c", &IsOkay);
                    system("cls");
                }while(IsOkay == 'n');
                n ++;
                break;

            case 2:                                                                       //manual
                get_ships(ships_list, ship_map, name);
                n ++;
                break;
            break;
            
        default:
            printf("Wrong choice! Try agian\n");
            break;
            }
        }
    }
}

void game_loop(){
    int choice;
    char * game_name, PlayBack;
    char last[10] = "last";

    while(1){
        menu();
        scanf("%d", &choice);
        Sleep(500);
        system("cls");

        switch (choice)
        {
        case 1:                                                                 //play with friend
            players = 2;
            
            remove("playback1.bin");
            remove("playback2.bin");
            
            empty_map(shot_map_1);                                                                                          //play with a friend
            empty_map(shot_map_2);
            score[0] = score[1] = 0;
            turn = 0;

            printf("First player:\n");
            player_setting(&ships_list_1, ship_map_1, name[0], &score[0]);
            printf("Second player:\n");
            player_setting(&ships_list_2, ship_map_2, name[1], &score[1]);

            shot_loop_players();
            
            save_score(name[0], score[0]);
            save_score(name[1], score[1]);

            break;

        case 2:                                                             //play with bot
            players = 1;

            remove("playback1.bin");
            remove("playback2.bin");

            empty_map(shot_map_1);
            empty_map(shot_map_2);
            score[0] = score[1] = 0;
            turn = 0;

            printf("First player:\n");
            player_setting(&ships_list_1,ship_map_1,name[0], &score[0]);                                          

            strcpy(name[1] , "bot");
            get_ships_auto(&ships_list_2, ship_map_2, name[1]);

            Sleep(500);
            system("cls");
            
            shot_loop_playerbot();

            save_score(name[0], score[0]);

            break;

        case 3:                                                          //load gmae   

            remove("playback1.bin");
            remove("playback2.bin");
            
            game_name = print_game();

            if(strcmp(game_name,"\0") == 0)                             //no saved game
                break;
            else{
            load(game_name);
                if(players == 2)
                        shot_loop_players();
                    else
                        shot_loop_playerbot();
                break;
            }

        case 4:                                                       //load last gmae 
            remove("playback1.bin");
            remove("playback2.bin");
            
            load(last);
            if(players == 2)
                shot_loop_players();
            else
                shot_loop_playerbot();                                                                       
            break;

        case 5:                                                    //setting
            setting();
            break;
        case 6:
            score_board();                                        //score board                              
            break;
        case 7:
            exit(0);
        default:
            printf("Wrong choice! Try agian\n");
            break;
        }
    }
}

int main(){
    
    help();

    ShipTypeInfo = (ship_info*) malloc(number * sizeof(ship_info));
    init_ship_info(ShipTypeInfo);
    
    game_loop();

}