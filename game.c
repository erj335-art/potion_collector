
#include <stdio.h>
#include <stdbool.h>

// Configurations
#define MAP_ROWS 12
#define MAP_COLS 12

// Tile codes
#define TILE_WALL 'W'
#define TILE_FLOOR 'F'
#define TILE_POTION 'P'

// Player structure
struct player
{
    int level;
    int defense;
    int attack;

    int position_x;
    int position_y;

};

// Globals (Map and Player)

char map[MAP_ROWS][MAP_COLS] = {
    {'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W'},
    {'W', 'F', 'W', 'F', 'F', 'F', 'F', 'F', 'F', 'F', 'F', 'W'},
    {'W', 'F', 'W', 'F', 'W', 'W', 'W', 'W', 'W', 'W', 'F', 'W'},
    {'W', 'F', 'W', 'F', 'F', 'F', 'F', 'F', 'F', 'W', 'F', 'W'},
    {'W', 'F', 'W', 'W', 'W', 'W', 'W', 'F', 'F', 'W', 'F', 'W'},
    {'W', 'F', 'F', 'W', 'F', 'F', 'F', 'F', 'F', 'W', 'F', 'W'},
    {'W', 'F', 'F', 'W', 'P', 'F', 'W', 'W', 'W', 'W', 'F', 'W'},
    {'W', 'W', 'F', 'W', 'F', 'F', 'W', 'F', 'F', 'F', 'F', 'W'},
    {'W', 'F', 'F', 'W', 'F', 'F', 'W', 'F', 'W', 'W', 'W', 'W'},
    {'W', 'F', 'F', 'W', 'F', 'F', 'W', 'F', 'W', 'P', 'F', 'W'},
    {'W', 'F', 'F', 'F', 'F', 'F', 'W', 'F', 'F', 'F', 'F', 'W'},
    {'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W'},
   
};
    struct player player = {1, 3, 3, 1, 1};

// Functions to be implemented
void printMenu(void);
void printMap(void);
void printPlayerStats(void);

char readUserInput(void);
void movePlayer(char dir);

bool isTileWalkable(char t);
int inBounds(int y, int x);

void collectItem(void);

// ====== Main ======
int main(void) {
    printf("Hello Wizard! 🧙‍♂️\n");
    while (1 == 1)
    {
        
        printMap();
        printPlayerStats();
        char input = readUserInput();
        movePlayer(input);
        printf("\n\n\n\n\n\n\n\n");
        collectItem();
        
    }
    
    return 0;
}



    


bool isTileWalkable(char t) {
    return(t != 'W');
}

void collectItem() {
    if (map[player.position_x][player.position_y] == 'P')  {
        map[player.position_x][player.position_y] = 'F';
        player.attack += 2;
        player.defense += 2;
        player.level++;
    }
    
}



void printPlayerStats() {
    printf("🗡️ Attack: %d\n", player.attack);
    printf("🛡️ Defense: %d\n", player.defense);
    printf("💡 Level: %d\n", player.level);
    printf("⌖ player position: [%d][%d]\n", player.position_x, player.position_y);
}

char readUserInput(void) {

    printf("\n\t   W - up");
    printf("\nA - Left  S - Down  D - Right\n");
    printf("Direction:");
    char input;
    scanf(" %c", &input);
    return input;
}


void movePlayer(char dir) {
    printf("Moving player to %c\n\n", dir);

    switch (dir)
    {
    case 'A':
    case 'a':
    if (isTileWalkable(map[player.position_x][player.position_y-1]))
        player.position_y--;
        break;
     case 'D':
     case 'd':
     if (isTileWalkable(map[player.position_x][player.position_y+1]))
        player.position_y++;
        break;
    case 'W':
    case 'w':
    if (isTileWalkable(map[player.position_x-1][player.position_y]))
        player.position_x--;
        break; 
    case 'S':
    case 's':
    if (isTileWalkable(map[player.position_x+1][player.position_y]))
        player.position_x++;
        break;
    }
    
}

// Implementations
void printMap(void) {
    for (int x = 0; x < MAP_ROWS; x++) {
        for (int y = 0; y < MAP_COLS; y++) {
            if (x == player.position_x && y == player.position_y)
                printf("🧙");
            else if (map[x][y] == 'W')
                printf("🟥");
            else if (map[x][y] == 'F')
                printf("🟦");
            else if (map[x][y] == 'P')
                printf("🍾");
            else
                printf("%c", map[x][y]);
            
        }
        printf("\n");
    }
    
}