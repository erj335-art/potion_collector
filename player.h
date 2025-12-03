#ifndef PLAYER_H
#define PLAYER_H

#include <stdbool.h>

#define POTION_SIZE 10

void player_initialize(void);
void player_print_stats(void);
void player_move(char direction);
bool player_collect_items(void);
int player_get_row(void);
int player_get_col(void);
void player_sound_dash(void);
void player_sound_grab_potion(void);

#endif /* PLAYER_H */
