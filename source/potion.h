#ifndef POTION_H
#define POTION_H

#include <stdbool.h>

typedef struct Potion {
    int x;
    int y;
    bool visible;
} Potion;

#define POTION_SIZE 10


void create_potion(int x, int y);
Potion* get_potions();
bool potion_tryCollect(int player_x, int player_y);
int are_all_potions_collected();
void reset_potions();

#endif /* POTION_H */
