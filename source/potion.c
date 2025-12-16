#include <stdio.h>
#include "potion.h"

Potion potions[POTION_SIZE];
int potionQuantity = 0;

void reset_potions() {
    for (int i = 0; i < POTION_SIZE; i++) {
        potions[i].visible = false;
    }
    potionQuantity = 0;
}

void create_potion(int x, int y)
{
    potions[potionQuantity].x = x;
    potions[potionQuantity].y = y;
    potions[potionQuantity].visible = true;
    potionQuantity++;
}

bool potion_tryCollect(int player_x, int player_y)
{
    for (int i = 0; i < POTION_SIZE; i++)
    {
        if (!potions[i].visible)
            continue;

        if (potions[i].x == player_x && potions[i].y == player_y)
        {
            potions[i].visible = false;
            return true;
        }
    }

    return false;
}

Potion *get_potions()
{
    return potions;
}

int are_all_potions_collected()
{
    int quantity = 0;
    for (int i = 0; i < POTION_SIZE; i++)
    {
        if (!potions[i].visible)
            quantity++;
    }
    return quantity == POTION_SIZE;
}