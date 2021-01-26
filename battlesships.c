#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<math.h>
#define COL 10
#define ROW 10

//global variable


int row = ROW,col = COL;
int ship_map1[ROW][COL];
char shot_map1[ROW][COL];
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


void delete_node(struct node * list, struct node * del){
    
    if(del == list){
        list = del->next;
        free(del);
        return;
    }
    struct node * curr = list ->next;
    struct node * prev = list;
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

    for (int i = 0; i < row; i++)
    {
        printf("|%2d ", i);
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

bool isvalid_ship_point(point start, point end, int len, int ship_map[row][col]){
    if(end.x - start.x + 1 != len && end.y - start.y + 1 != len)                //check len
        return false;
    
    for (int i = start.x; i <= end.x ; i++){
        for(int j = start.y; j <= end.y ; j++){                                 //check 9 box for each point
            if(ship_map[i][j] != 0)
                return false;
            if(i + 1 < row && ship_map[i + 1][j] != 0)
                return false;
            if(j + 1 < col && ship_map[i][j + 1] != 0)
                return false;
            if(i - 1 > 0 && ship_map[i - 1][j] != 0)
                return false;
            if(j - 1 > 0 && ship_map[i][j - 1] != 0)
                return false;
            if(j - 1 > 0 && i - 1 > 0 && ship_map[i - 1][j - 1] != 0)
                return false;
            if(j - 1 > 0 && i + 1 < row && ship_map[i + 1][j - 1] != 0)
                return false;
            if(j + 1 < col && i - 1 > 0 && ship_map[i - 1][j + 1] != 0)
                return false;
            if(j + 1 < col && i + 1 < row && ship_map[i + 1][j + 1] != 0)
                return false;
        }
    }
    return true;

}
void get_ships(struct node **ships_list, int ship_num[4][2], int ship_map[row][col]){                    //ship_num[4 = num of different lens][number of ship with <- len]
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
                printf("Start point of ship #%d with len %d (x, y): ", j + 1,ship_num[i][0]);
                scanf("%d %d",&(start.x), &(start.y));
                printf("End point of ship #%d with len %d (x, y): ", j + 1,ship_num[i][0]);
                scanf("%d %d",&(end.x), &(end.y));
                
                if(start.x > end.x || start.y > end.y)
                    swap(&start, &end);

                if(isvalid_ship_point(start, end, ship_num[i][0], ship_map)){
                    struct node * ship = (struct node *)malloc(sizeof(struct node));        //create new node
                    ship->info.len = ship_num[i][0];
                    ship->info.destroy = 0;
                    ship->info.start = start;
                    ship->info.end = end;
                    ship->info.num = ship_index;
                    ship->next = NULL;
                    for (int k = start.x; k <= end.x ; k++)                                //mark ships place in matrix ship_map 
                        for(int m = start.y; m <= end.y ; m++)
                            ship_map[k][m] = ship_index;
                    IsValid = 1;
                    add_end(ships_list,ship);                                             //add to list
                    ship_index ++;
                }
                else
                    printf("Invalid point! try again\n");
            }
        
        }
    
    }
}

char is_on_ship(char shot_map[row][col], point p, struct node * ships_list){
    struct node *curr = ships_list;
    while (curr != NULL)
    {
        if((p.x <= curr->info.end.x && p.x >= curr->info.start.x && p.y == curr->info.start.y) || (p.y <= curr->info.end.y && p.y >= curr->info.start.y && p.x == curr->info.start.x)){
            //^ check if the point is on a ship
            (curr->info.destroy) ++;
            if(curr->info.destroy == curr->info.len){
                return 'C';
                delete_node(ships_list, curr);
            }
            return 'E';
        }
        curr = curr->next;

    }
    return 'W';
    
}

void shot(char shot_map[row][col],struct node * ships_list){
    point p;
    scanf("%d %d", &p.x,&p.y);
    if (shot_map[p.x][p.y] == '\0')
        shot_map[p.x][p.y] = is_on_ship(shot_map, p, ships_list);
    else
        printf("wrong point !");                                                        //was chosen=) already
}

int main(){
    /*int num[4][2] = {{1,1},{2,1},{3,1},{5,1}};
    struct node * ships_list = NULL;
    for(int i =0 ; i<row;i++){
        for(int j =0;j<col;j++)
        printf("%d ", ship_map1[i][j]);
        printf("\n");
    }
    get_ships(&ships_list, num,ship_map1);
    for(int i =0 ; i<row;i++){
        for(int j =0;j<col;j++)
        printf("%d ", ship_map1[i][j]);
        printf("\n");
    }
    
    shot(shot_map1, ships_list);
    show_map(shot_map1);*/

}