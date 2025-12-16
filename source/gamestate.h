#ifndef GAMESTATE_H
#define GAMESTATE_H

#include <stdbool.h>

typedef enum {
    INITIAL_SCREEN,
    MAIN_MENU,
    INGAME,
    FINISHED
} GameState;

GameState gameState();
void setGameState(GameState newState);

#endif /* GAMESTATE_H */