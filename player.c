#include "player.h"

#include <stdio.h>

#include "map.h"

struct Player {
    int level;
    int attack;
    int defense;

    int position_x;
    int position_y;
};

struct Player player = {1, 5, 5, 3, 3};

void player_gain_potion_rewards(void) {
    player.level++;
    player.attack += 5;
    player.defense += 3;
}

void player_print_stats(void) {
    printf("🗡️ Attack: %d\n", player.attack);
    printf("🛡️ Defense: %d\n", player.defense);
    printf("💡 Level: %d\n", player.level);
    printf("🆇 Player Position: [%d,%d]\n", player.position_x, player.position_y);
}

void player_move(char direction) {
    printf("Moving player to %c\n\n", direction);
    switch (direction) {
        case 'W':
        case 'w':
            if (map_is_walkable(player.position_x - 1, player.position_y))
                player.position_x--;
            break;
        case 'A':
        case 'a':
            if (map_is_walkable(player.position_x, player.position_y - 1))
                player.position_y--;
            break;
        case 'S':
        case 's':
            if (map_is_walkable(player.position_x + 1, player.position_y))
                player.position_x++;
            break;
        case 'D':
        case 'd':
            if (map_is_walkable(player.position_x, player.position_y + 1))
                player.position_y++;
            break;
        default:
            break;
    }
}

void player_collect_items(void) {
    char tile = map_get_tile(player.position_x, player.position_y);
    if (tile == TILE_POTION) {
        map_set_tile(player.position_x, player.position_y, TILE_FLOOR);
        player_gain_potion_rewards();
    }
    //if (map_get_tile(player.position_x, player.position_y) == 'M') {
        // FIGHT HERE!
    //}
}

int player_get_row(void) {
    return player.position_x;
}

int player_get_col(void) {
    return player.position_y;
}
