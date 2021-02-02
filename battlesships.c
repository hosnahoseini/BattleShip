#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<math.h>
#include<time.h>
#include<windows.h>
#define COL 10
#define ROW 10

//global variable

int ship_num[4][2] = {{1,4},{2,3},{3,2},{5,1}};
int row = ROW,col = COL;
char ship_map_1[ROW][COL];                                                    //it was declared here just because of init, in the main prog it shoul be in main;
char shot_map_1[ROW][COL];
char ship_map_2[ROW][COL];                                                    //it was declared here just because of init, in the main prog it shoul be in main;
char shot_map_2[ROW][COL];
struct node * ships_list_1 = NULL,* ships_list_2 = NULL;
char name_1[100], name_2[100];
int score[2];                                                               //scores of two players save here --> score[0] = player one scores & score[1] = player 2 scores
int ship_score[6] = {0, 25, 12, 8, 0, 5};                                   //scores for each ship --> [0->ND, 1->25, 2->12, 3->8, 4->ND, 5->5]
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
            if(i - 1 > 0 && ship_map[i - 1][j] != '\0')
                return false;
            if(j - 1 > 0 && ship_map[i][j - 1] != '\0')
                return false;
            if(j - 1 > 0 && i - 1 > 0 && ship_map[i - 1][j - 1] != '\0')
                return false;
            if(j - 1 > 0 && i + 1 < row && ship_map[i + 1][j - 1] != '\0')
                return false;
            if(j + 1 < col && i - 1 > 0 && ship_map[i - 1][j + 1] != '\0')
                return false;
            if(j + 1 < col && i + 1 < row && ship_map[i + 1][j + 1] != '\0')
                return false;
        }
    }
    return true;

}
void get_ships(struct node **ships_list, char ship_map[row][col]){                    //ship_num[4 = num of different lens][number of ship with <- len]
    int ship_index = 1;
    printf("Enter start and end point of your ships:\n");

    
    for (int i = 0; i < 4; i++)                                               //loop for differrnt len
    {
        printf("Info of ships with len = %d:\n",ship_num[i][0]);

        for (int j = 0; j < ship_num[i][1]; j++)                             //loop for ships with same len
        {

            int IsValid = 0;                                                 //for cheking validation of points
            while(IsValid == 0){                                             //getting start point ship[ship_index]
                point start, end;
                char start_y, end_y;
                printf("Start point of ship #%d with len %d (x, y): ", j + 1,ship_num[i][0]);
                scanf("%d %c",&(start.x), &(start_y));
                printf("End point of ship #%d with len %d (x, y): ", j + 1,ship_num[i][0]);
                scanf("%d %c",&(end.x), &(end_y));
                system("cls");
                start.y = start_y - 'A';
                end.y = end_y - 'A';
                start.x --;
                end.x --;

                if(start.x > end.x || start.y > end.y)
                    swap(&start, &end);

                if(isvalid_ship_point(start, end, ship_num[i][0], ship_map)){
                    ship new_ship;        //create new node
                    new_ship.len = ship_num[i][0];
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
                else
                    printf("Invalid point! try again\n");
            }
        
        }
    
    }
    show_map(ship_map);
}

void get_ships_auto(struct node **ships_list, char ship_map[row][col]){
    int ship_index = 1;
    time_t t=time(NULL);
    srand(t);
    for (int i = 0; i < 4; i++)                                               //loop for differrnt len
    {
        //Info of ships with len ship_num[i][0]

        for (int j = 0; j < ship_num[i][1]; j++)                             //loop for ships with same len
        {

            int IsValid = 0;                                                 //for cheking validation of points
            while(IsValid == 0){                                             //getting start point ship[ship_index]
                point start, end;
                int horizontal_or_vetical = rand() % 2;                        //horizontal_or_vetical = 0 --> horizontal
                                                                             //horizontal_or_vetical = 1 -->vertical

                if(horizontal_or_vetical){
                    start.x = end.x = rand() % row;
                    start.y = rand() % (col - ship_num[i][0] + 1);
                    end.y = start.y + ship_num[i][0] - 1;
                }
                else
                {
                    start.y = end.y = rand() % col;
                    start.x = rand() % (row - ship_num[i][0] + 1);
                    end.x = start.x + ship_num[i][0] - 1;
                }

                if(isvalid_ship_point(start, end, ship_num[i][0], ship_map)){
                    ship new_ship;        //create new node
                    new_ship.len = ship_num[i][0];
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

point shot(char shot_map[row][col],struct node ** ships_list, int turn){
    point p;
    char p_y;
    printf("Enter your shot: ");
    scanf("%d %c", &p.x,&p_y);
    p.y = p_y - 'A';
    (p.x)--;
    
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
                    delete_node(ships_list, curr);                         //edit for first node!!!
                    score[turn] += (1 + ship_score[curr->info.len]);       //score for complete destruction of shops

                    return p;

                    
                }
                shot_map[p.x][p.y] = 'E';
                score[turn] ++;                                           //one point for each successful shot
                return p;
            }
            curr = curr->next;

        }
        shot_map[p.x][p.y] = 'W';
        return p;
    }
    else
        printf("wrong point !\n");                                                  //was chosen=) already
}

void shot_loop(struct node ** ships_list_1, struct node ** ships_list_2, char shot_map_1[row][col], char shot_map_2[row][col]){
    int turn = 0;
    point p;
    while(*ships_list_1 != NULL && *ships_list_2 != NULL){
        if(turn % 2 == 0){
            do
            {
                show_map(shot_map_1);
                printf("Player 1 is your turn:\n");
                p = shot(shot_map_1, ships_list_1, turn);
                show_map(shot_map_1);
                printf("score = %d\n", score[turn]);
            } while (shot_map_1[p.x][p.y] != 'W');
            printf("next\n");
            turn ++;
        }
        else
        {
             do
            {
                show_map(shot_map_2);
                printf("Player 2 is your turn:\n");
                p = shot(shot_map_2, ships_list_2, turn);
                show_map(shot_map_2);
                printf("score = %d\n", score[turn]);
            } while (shot_map_2[p.x][p.y] != 'W');
            printf("next\n");
            turn ++;
            
        }
    }
}
void setting (){
    printf("1)ships\n2)map size\n");
    int choice;
    scanf("%d", &choice);
    system("cls");
    switch (choice)
    {
    case 1:
        for(int j = 0; j < 4; j++){
            printf("Enter number of ships with len = %d:\n", ship_num[j][0]);
            scanf("%d", &ship_num[j][1]);
            Sleep(500);
            system("cls");
        }
        break;
    case 2:
        printf("Enter # of rows: \n");
        scanf("%d", &row);
        printf("Enter # of columns: \n");
        scanf("%d", &col);
        Sleep(500);
        system("cls");
        break;

    default:
        printf("Wrong choice!");
    }

}
void each_ship_score(int ship_score[]){

}
void player_setting(struct node ** ships_list, char ship_map[row][col], char * name){
    //printf("\t1) choose user\n\t2) put ships\n\t3) done!\n");
    int choice1 = 0, choice2;
    //scanf("%d", &choice1);
    while(choice1 != 3){
        printf("1) choose user\n2) put ships\n3) done!\n");
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
            break;

        case 2:

            printf("1) Auto\n2) Manual\n");
            scanf("%d", &choice2);
            system("cls");
            switch (choice2)
            {
            case 1:                     
                get_ships_auto(ships_list,ship_map);
                break;
            case 2:
                get_ships(ships_list, ship_map);

        case 3:
        choice1 = 3;
            break;
        default:
            break;
            }
        }
    }
}
void menu_loop(struct node ** ships_list_1, struct node ** ships_list_2, char shot_map_1[row][col], char shot_map_2[row][col]){
    
    int choice = 0;
    while(choice != 7){
        menu();
        scanf("%d", &choice);
        system("cls");
        switch (choice)
        {
        case 1:                                                 //play with a friend
            printf("First player:\n");
            player_setting(ships_list_1, ship_map_1, name_1);
            printf("Second player:\n");
            player_setting(ships_list_2, ship_map_2, name_2);
            break;
        case 2:
            printf("First player:\n");
            player_setting(ships_list_1,ship_map_1,name_1);                                                 //play with a bot
            get_ships_auto(ships_list_2, ship_map_2);
            break;
        case 3:                                                 //load gmae                                 -->fil
            break;
        case 4:                                                 //load last gmae                            -->file
            break;
        case 5:                                                 //setting
            setting();
            break;
        case 6:                                                 //score board                               -->file
            break;
        case 7:                                                 //exit
            break;
        default:
            break;
        }
    }
}

int main(){
    
    
    help();
    menu_loop(&ships_list_1, &ships_list_2, shot_map_1, shot_map_2);
    shot_loop(&ships_list_1, &ships_list_2, shot_map_1, shot_map_2);
    /*for(int i =0 ; i<row;i++){
        for(int j =0;j<col;j++)
        printf("%d ", ship_map_1[i][j]);
        printf("\n");
    }*/
    //printf("----------------");
    /*get_ships_auto(&ships_list,ship_map_1);
    for(int i =0 ; i<row;i++){
        for(int j =0;j<col;j++)
        printf("%d ", ship_map_1[i][j]);
        printf("\n");
    }
    
    shot_loop(&ships_list,NULL,shot_map_1,shot_map_1);
    show_map(shot_map_1);*/

}