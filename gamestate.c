#include "gamestate.h"

GameState current_game_state = MAIN_MENU;

GameState gameState() {
    return current_game_state;
}

void setGameState(GameState newState) {
    current_game_state = newState;
}