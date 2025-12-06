#include "gamestate.h"

GameState current_game_state = INITIAL_SCREEN;

GameState gameState() {
    return current_game_state;
}

void setGameState(GameState newState) {
    current_game_state = newState;
}