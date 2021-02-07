#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<math.h>
#include<time.h>
#include<windows.h>
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

//global variable


int row = ROW,col = COL;
char ship_map_1[ROW][COL];                                                    //it was declared here just because of init, in the main prog it shoul be in main;
char shot_map_1[ROW][COL];
char ship_map_2[ROW][COL];                                                    //it was declared here just because of init, in the main prog it shoul be in main;
char shot_map_2[ROW][COL];
struct node * ships_list_1 = NULL,* ships_list_2 = NULL;
char name[2][100];
int score[2];                                                               //scores of two players save here --> score[0] = player one scores & score[1] = player 2 scores
ship_info *ShipTypeInfo;                                                        //len, number, scores of each ship type
int number = 4;                                                             //number of ships type
int players;                                                               // if players == 1 --> play with bot //if players == 2 --> play with friends
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

void menu (){
    printf("1)Play with a friend\n2)Play with a bot\n3)Load game\n4)Load last game\n5)Setting\n6)Score board\n7)Exit\n");
}

void init_ship_info (ship_info *ShipTypeInfo){

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

// save
FILE * search_in_players(char name[100]){
    FILE * fp = fopen("score.bin","r+b");
    if(fp == NULL)
        return NULL;
    char file_name [100];
    while (feof(fp) == 0)
    {

        fread(file_name, sizeof(char), 100, fp);
        if(strcmp(name, file_name) == 0){
            return fp;
        }
        if(feof(fp))
            break;
        fseek(fp, sizeof(int), SEEK_CUR);
        
    }
    fclose(fp);
    //printf(":(\n");
    return NULL;
}
void save_score(char name[100], int score){
    FILE * fp = search_in_players(name);
    
    if(fp != NULL){
        //fseek(fp, search_in_players(name), SEEK_SET);
        int file_score;
        fread(&file_score, sizeof(int), 1, fp);
        score += file_score;
        fseek(fp, -1 * sizeof(int), SEEK_CUR);
        fwrite(&score,sizeof(int), 1, fp);
        fclose(fp);
    }
    else
    {
        //printf("HERE!\n");
        fp = fopen("score.bin","ab");
        fwrite(name, sizeof(char), 100, fp);
        fwrite(&score, sizeof(int), 1, fp);
        //cnt ++;
        fclose(fp);
    }
    
}
void score_board(){
    FILE * fp = fopen("score.bin","rb");
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
int getLinkedListSize(struct node * list){
    int cnt = 0;
    struct node * curr = list;
    while(curr != NULL){
        curr = curr -> next;
        cnt ++;
    }
    return cnt;
}

void save(){
    char save;
    //char filename[100];
    printf("Do you want to save the game? ");
    scanf(" %c", &save);
    if (save == 'y'){
        //printf("Enter game name: ");
        //scanf(" %s", filename);
        //strcat(filename,".bin");
        if (players == 2){
            save_score(name[0], score[0]);
            save_score(name[1], score[1]);
        }
        else
            save_score(name[0], score[0]);

        FILE * fp = fopen("load.bin","ab");

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
        fclose(fp);
        
    }
}
void print_game(){
    FILE * fp = fopen("load.bin", "rb");
    char load_name [2][100];
    int len;
    while (1)
    {
        if(feof(fp))
            break;

        fread(load_name, sizeof(char), 100 * 2, fp);
        printf("|%2d|%25s vs %25s|", load_name[0], load_name[1]);
        fread(&row, sizeof(int), 1, fp);
        fread(&col, sizeof(int), 1, fp);

        fread(&number, sizeof(int), 1, fp);

        int ListSize1 ;
        int ListSize2 ;
        fread(&ListSize1, sizeof(int), 1, fp);
        fread(&ListSize2, sizeof(int), 1, fp);
        len = ((sizeof(int) * row * col) * 2) + sizeof(int) * 3 + number * sizeof(ship_info) + sizeof(ship) * (ListSize1 + ListSize2);
        fseek(fp, len, SEEK_CUR);

    }
    
}
void load(){
        FILE * fp = fopen("load.bin","rb");

        fread(name,sizeof(char), 100 * 2, fp);

        fread(&row, sizeof(int), 1, fp);
        fread(&col, sizeof(int), 1, fp);

        fread(&number, sizeof(int), 1, fp);

        int ListSize1 ;
        int ListSize2 ;
        fread(&ListSize1, sizeof(int), 1, fp);
        fread(&ListSize2, sizeof(int), 1, fp);

        fread(&score, sizeof(int), 2 * 1, fp);

        ShipTypeInfo = malloc(sizeof(ship_info) * number);
        fread(ShipTypeInfo, sizeof(ship_info), number, fp);
    
        fread(shot_map_1, sizeof(char), row * col, fp);
        fread(shot_map_2, sizeof(char), row * col, fp);

        ship new_info;
        for(int i =0 ;i< ListSize1; i++){
            fread(&new_info, sizeof(ship), 1, fp);
            add_end(&ships_list_1,create_node(new_info));
        }

        for(int i =0 ;i< ListSize1; i++){
            fread(&new_info, sizeof(ship), 1, fp);
            add_end(&ships_list_2,create_node(new_info));
        }
        fread(&players, sizeof(int), 1, fp);
        
        fclose(fp);
    
}
//
void swap(point *p1, point *p2){
    point tmp = *p1;
    *p1 = *p2;
    *p2 = *p1;
}

bool isvalid_ship_point(point start, point end, int len, char ship_map[row][col]){
    if(end.x - start.x + 1 != len && end.y - start.y + 1 != len)                //check len
        return false;
    
    for (int i = start.x; i <= end.x ; i++){
        for(int j = start.y; j <= end.y ; j++){                                 //check 9 box for each point
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
void get_ships(struct node **ships_list, char ship_map[row][col]){                    
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
                start.y = start_y - 'A';
                end.y = end_y - 'A';
                start.x --;
                end.x --;

                if(start.x > end.x || start.y > end.y)
                    swap(&start, &end);

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
    show_map(ship_map);
    Sleep(500);
    system("cls");
}

void get_ships_auto(struct node **ships_list, char ship_map[row][col]){

    (*ships_list) = NULL;                                                       //delete previous one!
                    
    for(int i = 0; i < row; i ++)
        for(int j = 0; j < col; j ++)
            ship_map[i][j] = '\0';
    int ship_index = 1;
    time_t t=time(NULL);
    srand(t);
    for (int i = 0; i < number; i++)                                               //loop for differrnt len
    {
        //Info of ships with len ShipTypeInfo[i].len
        for (int j = 0; j < ShipTypeInfo[i].num; j++)                            //loop for ships with same len
        {

            int IsValid = 0;                                                 //for cheking validation of points
            while(IsValid == 0){                                             //getting start point ship[ship_index]
                point start, end;
                int horizontal_or_vetical = rand() % 2;                        //horizontal_or_vetical = 0 --> horizontal
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
                    ship new_ship;        //create new node
                    new_ship.len = ShipTypeInfo[i].len;
                    new_ship.destroy = 0;
                    new_ship.start = start;
                    new_ship.end = end;
                    new_ship.num = ship_index;
                    add_end(ships_list,create_node(new_ship));                            //add to list
                    for (int k = start.x; k <= end.x ; k++)                                //mark ships place in matrix ship_map 
                        for(int m = start.y; m <= end.y ; m++)
                            ship_map[k][m] = 'X';
                    IsValid = 1;
                    
                    ship_index ++;
                }
            }
        
        }
    
    }
    show_map(ship_map);
    
}

void shot(char shot_map[row][col],struct node ** ships_list, int turn, point p){                                    //apply changes
    
    if (shot_map[p.x][p.y] == '\0'){
        
        struct node *curr = *ships_list;
        while (curr != NULL)
        {
            if((p.x <= curr->info.end.x && p.x >= curr->info.start.x && p.y == curr->info.start.y)
                 || (p.y <= curr->info.end.y && p.y >= curr->info.start.y && p.x == curr->info.start.x)){
                //^ check if the point is on a ship
                (curr->info.destroy) ++;
                
                if(curr->info.destroy == curr->info.len){
                    
                    for (int i = curr->info.start.x; i <= curr->info.end.x ; i++)
                        for(int j = curr->info.start.y; j <= curr->info.end.y ; j++)
                            shot_map[i][j] = 'C';

                    
                    for (int i = curr->info.start.x - 1; i <= curr->info.end.x + 1 ; i++)
                        for(int j = curr->info.start.y - 1; j <= curr->info.end.y + 1; j++){
                            if(i >= 0 && j >= 0 && i < col && j < row && shot_map[i][j] != 'C')
                                shot_map[i][j] = 'W';
                        }

                    delete_node(ships_list, curr); 
                    score[turn % 2] += (1 + ShipTypeInfo[curr->info.len].score);       //score for complete destruction of shops

                    return;

                    
                }
                shot_map[p.x][p.y] = 'E';
                score[turn % 2] ++;                                           //one point for each successful shot
                return;
            }
            curr = curr->next;

        }
        shot_map[p.x][p.y] = 'W';
        return;
    }
    else
        printf("wrong point !\n");                                                  //was chosen=) already
}

void shot_loop_players(struct node ** ships_list_1, struct node ** ships_list_2, char shot_map_1[row][col], char shot_map_2[row][col]){
    int turn = 0;
    point p;
    char p_y;
    while(*ships_list_1 != NULL && *ships_list_2 != NULL){
        if(turn % 2 == 0){
            do
            {
                show_map(shot_map_1);

                printf("%s is your turn:\n", name[turn % 2]);
                save();
                printf("Enter your shot: ");
                scanf("%d %c", &p.x,&p_y);
                system("cls");
                p.y = p_y - 'A';
                (p.x)--;

                shot(shot_map_1, ships_list_2, turn, p);
                show_map(shot_map_1);
                printf("%s score = %d\n", name[turn % 2], score[turn % 2]);
                Sleep(500);
                system("cls");
            } while (shot_map_1[p.x][p.y] != 'W' && *ships_list_1 != NULL && *ships_list_2 != NULL);
            printf("next\n");
            turn ++;
        }
        else
        {
             do
            {
                show_map(shot_map_2);

                printf("%s is your turn:\n", name[turn % 2]);
                save();
                printf("Enter your shot: ");
                scanf("%d %c", &p.x,&p_y);
                system("cls");
                p.y = p_y - 'A';
                (p.x)--;

                shot(shot_map_2, ships_list_1, turn, p);
                show_map(shot_map_2);
                printf("%s score = %d\n", name[turn % 2], score[turn % 2]);
                Sleep(500);
                system("cls");
            } while (shot_map_2[p.x][p.y] != 'W' && *ships_list_1 != NULL && *ships_list_2 != NULL);
            printf("next\n");
            turn ++;
            
        }
    }
    printf("%s wins!!!\n",name[(turn-1) % 2] );
    Sleep(500);
    system("cls");
}

void shot_loop_playerbot(struct node ** ships_list_1, struct node ** ships_list_2, char shot_map_1[row][col], char shot_map_2[row][col]){
    int turn = 0;
    point p;
    char p_y;
    time_t t=time(NULL);
    srand(t);
    while(*ships_list_1 != NULL && *ships_list_2 != NULL){
        if(turn % 2 == 0){
            do
            {
                show_map(shot_map_1);
                printf("%s is your turn:\n", name[turn % 2]);
                save();
                printf("Enter your shot: ");
                scanf("%d %c", &p.x,&p_y);
                system("cls");
                p.y = p_y - 'A';
                (p.x)--;

                shot(shot_map_1, ships_list_2, turn, p);
                show_map(shot_map_1);
                Sleep(500);
                system("cls");
                printf("%s score = %d\n", name[turn % 2], score[turn % 2]);
            } while (shot_map_1[p.x][p.y] != 'W' && *ships_list_1 != NULL && *ships_list_2 != NULL);
            
            turn ++;
        }
        else
        {
             do
            {
                do
                {
                    p.x = rand() % col;
                    p.y = rand() % row;
                } while (shot_map_2[p.x][p.y] != '\0');

                shot(shot_map_2, ships_list_1, turn, p);
                show_map(shot_map_2);
                Sleep(500);
                system("cls");
                printf("%s score = %d\n", name[turn % 2], score[turn % 2]);
            } while (shot_map_2[p.x][p.y] != 'W' && *ships_list_1 != NULL && *ships_list_2 != NULL);
            
            turn ++;
            
        }
    }
    printf("%s wins!!!\n",name[(turn-1) % 2] );
    Sleep(500);
    system("cls");
}

bool check_setting(int row, int col, ship_info *ShipTypeInfo){
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
            int max = 0;
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

            for(int i = 0; i < number; i++)
                ShipTypeInfo[i].score = 5 * max / ShipTypeInfo[i].len;

            check = check_setting(row, col, ShipTypeInfo);
        }while(check == false);
        
            
    }
    else if(choice == 2){
        check = true;
        do{
            if(check == false)
                printf("Wong!Try again\n");
            printf("Enter # of rows: \n");
            scanf("%d", &row);
            printf("Enter # of columns: \n");
            scanf("%d", &col);
            Sleep(500);
            system("cls");
            check = check_setting(row, col, ShipTypeInfo);
        }while(check == false);
     }

    else{
        printf("Wrong choice! Try again\n");
    }

}

void player_setting(struct node ** ships_list, char ship_map[row][col], char * name){
    
    int choice1 = 0, choice2, n = 0;                                                                // n is for checking if the user enter both ships and nameor not
    char IsOkay;                                                                                    //to check if the Auto map is satisfying or not?
    
    while(n != 2){
        printf("1) choose user\n2) put ships\n");
        scanf("%d", &choice1);
        system("cls");
        switch (choice1)
        {
        case 1:

            printf("1) choose from available users\n2) new user\n");
            scanf("%d", &choice2);
            system("cls");
            switch (choice2)
            {
            case 1:                     //choose from available users           -->file
                break;
            case 2:
                printf("Enter your name: ");
                getchar();
                gets(name);
                Sleep(1000);
                system("cls");
            default:
                break;
            }

            n ++;
            break;

        case 2:
           
            printf("1) Auto\n2) Manual\n");
            scanf("%d", &choice2);
            //system("cls");
            
            switch (choice2)
            {
            case 1:
                do{ 
                    get_ships_auto(ships_list,ship_map);
                    printf("Is it okay?(y/n) : ");
                    scanf(" %c", &IsOkay);
                    Sleep(500);
                    system("cls");
                }while(IsOkay == 'n');
                n ++;
                break;
            case 2:
                get_ships(ships_list, ship_map);
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

void game_loop(struct node ** ships_list_1, struct node ** ships_list_2, char shot_map_1[row][col], char shot_map_2[row][col]){
    
    int choice = 0,PlayAgain;
    while(choice != 7){
        menu();
        scanf("%d", &choice);
        Sleep(500);
        system("cls");
        switch (choice)
        {
        case 1:
             players = 2;                                                                                          //play with a friend
            printf("First player:\n");
            player_setting(ships_list_1, ship_map_1, name[0]);
            printf("Second player:\n");
            player_setting(ships_list_2, ship_map_2, name[1]);
            shot_loop_players(ships_list_1, ships_list_2, shot_map_1, shot_map_2);
            
            save_score(name[0], score[0]);
            save_score(name[1], score[1]);
            //printf("Do you want to play again?(y/n): ");
            //scanf(" %c", &PlayAgain);
            //choice = PlayAgain == 'n' ? 7 : 1;

            break;
        case 2:
            players = 1;
            printf("First player:\n");
            player_setting(ships_list_1,ship_map_1,name[0]);                                                 //play with a bot
            strcpy(name[1] , "bot");
            get_ships_auto(ships_list_2, ship_map_2);
            shot_loop_playerbot(ships_list_1, ships_list_2, shot_map_1, shot_map_2);

            save_score(name[0], score[0]);
            break;
        case 3:                                                  //load gmae                                 -->fil;
            print_game();
            break;
        case 4:                                                 //load last gmae                            -->file
            break;
        case 5:                                                 //setting
            setting();
            break;
        case 6:
            score_board();                                      //score board                               -->file
            break;
        case 7:
            break;
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
    
    game_loop(&ships_list_1, &ships_list_2, shot_map_1, shot_map_2);
    printf("bye!");

}