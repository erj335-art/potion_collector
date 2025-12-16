#include <stdio.h>

#include "map.h"
#include "player.h"

char readUserInput(void);

// ====== Main ======
int main(void)
{
    printf("Hello Wizard! 🧙‍♂️\n");
    while (1 == 1) {
        // Windows Terminal Clean Function (Uses stdlib)
        // Don't forget if you use system('cls'), OS like MacOS or Linux will NOT run.
        // system("cls");
        player_print_stats();
        map_print(player_get_row(), player_get_col());
        char input = readUserInput();
        player_move(input);

        player_collect_items();

        printf("\n\n\n\n\n\n\n");
    }

    return 0;
}

char readUserInput(void) {
    printf("\n\t   W - UP");
    printf("\nA - LEFT   S - DOWN  D - RIGHT\n");
    printf("Direction: ");
    char input;
    scanf(" %c", &input);
    return input;
}
