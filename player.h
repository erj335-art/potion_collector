#ifndef PLAYER_H
#define PLAYER_H

void player_print_stats(void);
void player_move(char direction);
void player_collect_items(void);
int player_get_row(void);
int player_get_col(void);

#endif /* PLAYER_H */
