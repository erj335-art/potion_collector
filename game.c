#include <stdio.h>
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include "sdl_utils.h"
#include "map.h"
#include "player.h"
#include "potion.h"

#define APP_NAME "Potion Collector"

#define TEXTURE_WIDTH 32
#define TEXTURE_HEIGHT 32

// 12 by 12 tiles. Each tile having 16 pixels by 16 pixels.
#define APP_WIDTH 12 * TEXTURE_WIDTH
#define APP_HEIGHT 12 * TEXTURE_HEIGHT

static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;

static SDL_Texture *grassTexture = NULL;
static SDL_Texture *wallTexture = NULL;
static SDL_Texture *wizardTexture = NULL;
static SDL_Texture *potionTexture = NULL;

void renderTextures(void);
char readUserInput(void);

int main(void)
{
    // Initialize SDL Systems.
    window = sdl_initialize_window(APP_NAME, APP_HEIGHT, APP_WIDTH);
    renderer = sdl_initialize_renderer(window);
    sdl_initialize_audio();

    player_initialize();
    grassTexture = sdl_load_texture(renderer, "sprites/tiles/tile000.png");
    wallTexture = sdl_load_texture(renderer, "sprites/tiles/tile001.png");
    wizardTexture = sdl_load_texture(renderer, "sprites/characters/wizard.png");
    potionTexture = sdl_load_texture(renderer, "sprites/items/potion.png");

    map_load("maps/level1.txt");

    // Game Loop
    int running = 1;
    const Uint32 FRAME_MS = 16; // ~60 FPS
    while (running)
    {
        Uint32 frame_start = SDL_GetTicks();

        // Capture Events
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_EVENT_QUIT)
                running = 0;

            if (event.type == SDL_EVENT_KEY_DOWN)
            {
                if (event.key.key == SDLK_W)
                    player_move('W');
                if (event.key.key == SDLK_A)
                    player_move('A');
                if (event.key.key == SDLK_S)
                    player_move('S');
                if (event.key.key == SDLK_D)
                    player_move('D');

                if(player_collect_items()){
                    printf("All potions collected! You win!\n");
                    running = 0;
                }
            }
        }

        // Render textures
        renderTextures();

        // map_print(player_get_row(), player_get_col());

        // Delay to maintain frame rate
        Uint32 elapsed = SDL_GetTicks() - frame_start;
        if (elapsed < FRAME_MS)
            SDL_Delay(FRAME_MS - elapsed);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

void renderMap()
{
    for (int x = 0; x < MAP_ROWS; x++)
    {
        for (int y = 0; y < MAP_COLS; y++)
        {
            SDL_FRect dst_rect = {TEXTURE_WIDTH * y, TEXTURE_HEIGHT * x, TEXTURE_WIDTH, TEXTURE_HEIGHT};

            if (map_get_tile(x, y) == TILE_FLOOR)
            {
                SDL_RenderTexture(renderer, grassTexture, NULL, &dst_rect);
            }
            else if (map_get_tile(x, y) == TILE_WALL)
            {
                SDL_RenderTexture(renderer, wallTexture, NULL, &dst_rect);
            }
        }
    }
}

void renderPlayer()
{
    // Render the wizard at the player's position
    SDL_FRect wizard_rect = {TEXTURE_WIDTH * player_get_col(), TEXTURE_WIDTH * player_get_row(), TEXTURE_WIDTH, TEXTURE_HEIGHT};
    SDL_RenderTexture(renderer, wizardTexture, NULL, &wizard_rect);
}

void renderPotions()
{
    // Render all potions
    Potion *potions = get_potions();
    for (int i = 0; i < 10; i++)
    {
        if (!potions[i].visible)
            continue;

        SDL_FRect potionRect = {TEXTURE_WIDTH * potions[i].x, TEXTURE_HEIGHT * potions[i].y, TEXTURE_WIDTH, TEXTURE_HEIGHT};
        SDL_RenderTexture(renderer, potionTexture, NULL, &potionRect);
    }
}

void renderTextures(void)
{
    const int charsize = SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE;

    /* as you can see from this, rendering draws over whatever was drawn before it. */
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE); /* black, full alpha */
    SDL_RenderClear(renderer);                                   /* start with a blank canvas. */

    renderMap();
    renderPlayer();
    renderPotions();

    showText(renderer, 100, 0, "Potion Collector!", (SDL_Color){255, 255, 255, SDL_ALPHA_OPAQUE});
    SDL_RenderDebugTextFormat(renderer, (float)((APP_WIDTH - (charsize * 46)) / 2), APP_HEIGHT - charsize, "(This program has been running for %" SDL_PRIu64 " seconds.)", SDL_GetTicks() / 1000);

    SDL_RenderPresent(renderer); /* put it all on the screen! */
}