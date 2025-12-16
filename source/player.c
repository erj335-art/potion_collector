#include "player.h"
#include "../utils/sdl_utils.h"

#include <stdio.h>

#include "map.h"
#include "potion.h"

struct Player
{
    int level;
    int attack;
    int defense;

    int position_x;
    int position_y;
};

struct Player player = {1, 5, 5, 3, 3};
static Sound player_dash_sound;
static Sound player_grab_potion_sound;

void player_initialize(void)
{
    init_sound("sounds/dash.wav", &player_dash_sound);
    init_sound("sounds/yummy.wav", &player_grab_potion_sound);
    player.level = 1;
    player.attack = 5;
    player.defense = 5;
    player.position_x = 3;
    player.position_y = 3;
}

void player_gain_potion_rewards(void)
{
    player.level++;
    player.attack += 5;
    player.defense += 3;
}

void player_print_stats(void)
{
    printf("🗡️ Attack: %d\n", player.attack);
    printf("🛡️ Defense: %d\n", player.defense);
    printf("💡 Level: %d\n", player.level);
    printf("🆇 Player Position: [%d,%d]\n", player.position_x, player.position_y);
}

void player_move(char direction)
{
    switch (direction)
    {
    case 'W':
    case 'w':
        if (map_is_walkable(player.position_x - 1, player.position_y))
        {
            player.position_x--;
            playSound(&player_dash_sound);
        }
        break;
    case 'A':
    case 'a':
        if (map_is_walkable(player.position_x, player.position_y - 1))
        {
            player.position_y--;
            playSound(&player_dash_sound);
        }
        break;
    case 'S':
    case 's':
        if (map_is_walkable(player.position_x + 1, player.position_y))
        {
            player.position_x++;
            playSound(&player_dash_sound);
        }
        break;
    case 'D':
    case 'd':
        if (map_is_walkable(player.position_x, player.position_y + 1))
        {
            player.position_y++;
            playSound(&player_dash_sound);
        }
        break;
    default:
        break;
    }
}

// Returns true if all potions were collected
bool player_collect_items(void)
{

    if (potion_tryCollect(player.position_y, player.position_x))
        playSound(&player_grab_potion_sound);

    return are_all_potions_collected();
}

int player_get_row(void)
{
    return player.position_x;
}

int player_get_col(void)
{
    return player.position_y;
}
