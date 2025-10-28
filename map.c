#include "map.h"
#include <stdio.h>

char map_data[MAP_ROWS][MAP_COLS];

void map_load(const char *filename) {

    FILE *file = fopen(filename, "r");

    if (file == NULL) {
        printf("Failed to open map file");
        return;
    }

    for (int row = 0; row < MAP_ROWS; row++) {
        for (int col = 0; col < MAP_COLS; col++) {
            int ch = fgetc(file);
            if (ch == EOF) {
                fclose(file);
                return;
            }
            if (ch == '\n') {
                col--;
                continue;
            }
            map_data[row][col] = (char)ch;
        }
    }
    fclose(file);
}

bool map_in_bounds(int row, int col) {
    return row >= 0 && row < MAP_ROWS && col >= 0 && col < MAP_COLS;
}

void map_print(int player_row, int player_col) {
    for (int row = 0; row < MAP_ROWS; row++) {
        for (int col = 0; col < MAP_COLS; col++) {
            if (row == player_row && col == player_col)
                printf("🧙");
            else if (map_data[row][col] == TILE_WALL)
                printf("🟥");
            else if (map_data[row][col] == TILE_FLOOR)
                printf("⬜️");
            else if (map_data[row][col] == TILE_POTION)
                printf("🍔");
            else
                printf("%c", map_data[row][col]);
        }
        printf("\n");
    }
}

bool map_is_walkable(int row, int col) {
    if (!map_in_bounds(row, col))
        return false;
    return map_data[row][col] != TILE_WALL;
}

char map_get_tile(int row, int col) {
    if (!map_in_bounds(row, col))
        return TILE_WALL;
    return map_data[row][col];
}

void map_set_tile(int row, int col, char tile) {
    if (!map_in_bounds(row, col))
        return;
    map_data[row][col] = tile;
}
