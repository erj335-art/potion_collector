#include <stdio.h>
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include "utils/sdl_utils.h"
#include "source/map.h"
#include "source/player.h"
#include "source/potion.h"
#include "source/gamestate.h"

#define APP_NAME "Potion Collector"

#define TEXTURE_WIDTH 32
#define TEXTURE_HEIGHT 32

// 12 by 12 tiles.
#define APP_WIDTH (12 * TEXTURE_WIDTH)   // 384
#define APP_HEIGHT (12 * TEXTURE_HEIGHT) // 384

#define APP_MAINMENU_WIDTH 512
#define APP_MAINMENU_HEIGHT 512

static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;

static SDL_Texture *grassTexture = NULL;
static SDL_Texture *wallTexture = NULL;
static SDL_Texture *wizardTexture = NULL;
static SDL_Texture *potionTexture = NULL;
static SDL_Texture *logoTexture = NULL;
static SDL_Texture *bgTexture = NULL;

static Sound inGameMusic;

void renderGame(void);
void renderMainMenu(void);
void renderGameFinished(void);
void renderInitialScreen(void);

static void set_menu_presentation(void)
{
    // Scale menu to window/fullscreen while keeping aspect ratio.
    SDL_SetRenderLogicalPresentation(renderer,
                                     APP_MAINMENU_WIDTH, APP_MAINMENU_HEIGHT,
                                     SDL_LOGICAL_PRESENTATION_LETTERBOX);

    // Windowed mode for menus.
    SDL_SetWindowFullscreen(window, false);
}

static void set_game_presentation(void)
{
    // Scale game to window/fullscreen with pixel-art-friendly integer scaling.
    SDL_SetRenderLogicalPresentation(renderer,
                                     APP_WIDTH, APP_HEIGHT,
                                     SDL_LOGICAL_PRESENTATION_LETTERBOX);

    // Fullscreen (borderless desktop)
    SDL_SetWindowFullscreen(window, true);
}

static void set_nearest(SDL_Texture *t)
{
    if (t)
        SDL_SetTextureScaleMode(t, SDL_SCALEMODE_NEAREST);
}

int opSelected = 0;

int main(void)
{
    // Initialize SDL Systems.
    window = sdl_initialize_window(APP_NAME, APP_MAINMENU_WIDTH, APP_MAINMENU_HEIGHT);
    renderer = sdl_initialize_renderer(window);
    sdl_initialize_audio();

    // IMPORTANT: set presentation once (start in menu sizing).
    set_menu_presentation();

    grassTexture = sdl_load_texture(renderer, "sprites/tiles/tile000.png");
    wallTexture = sdl_load_texture(renderer, "sprites/tiles/tile001.png");
    wizardTexture = sdl_load_texture(renderer, "sprites/characters/wizard.png");
    potionTexture = sdl_load_texture(renderer, "sprites/items/potion.png");
    logoTexture = sdl_load_texture(renderer, "sprites/logo.png");
    bgTexture = sdl_load_texture(renderer, "sprites/bg.png");

    // Keep scaled pixel art crisp (default is linear).
    set_nearest(grassTexture);
    set_nearest(wallTexture);
    set_nearest(wizardTexture);
    set_nearest(potionTexture);
    set_nearest(logoTexture);
    set_nearest(bgTexture);

    init_sound("sounds/wizardBackground.wav", &inGameMusic);

    // Game Loop
    int running = 1;
    const Uint32 FRAME_MS = 16; // ~60 FPS
    while (running)
    {
        Uint32 frame_start = SDL_GetTicks();

        // Capture events (like key presses).
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_EVENT_QUIT)
                running = 0;

            if (event.type == SDL_EVENT_KEY_DOWN)
            {
                if (gameState() == INITIAL_SCREEN && event.key.key == SDLK_SPACE)
                {
                    setGameState(MAIN_MENU);
                    SDL_SetWindowSize(window, APP_MAINMENU_WIDTH, APP_MAINMENU_HEIGHT);
                    SDL_SetWindowTitle(window, "Potion Collector - Main Menu");
                    set_menu_presentation();
                }
                else if (gameState() == MAIN_MENU)
                {
                    if (event.key.key == SDLK_SPACE)
                    {
                        if (opSelected == 0)
                        {
                            player_initialize();
                            reset_potions();
                            map_load("maps/level1.txt");

                            setGameState(INGAME);

                            // Switch to game logical resolution BEFORE presenting frames.
                            set_game_presentation();

                            SDL_SetWindowTitle(window, "Potion Collector - In Game");
                            playSound(&inGameMusic);
                        }
                        else if (opSelected == 1)
                        {
                            // Level selection not implemented yet
                        }
                        else if (opSelected == 2)
                        {
                            // Options not implemented yet
                        }
                        else if (opSelected == 3)
                        {
                            // Exit game if op 3 selected
                            running = 0;
                        }
                    }

                    if (event.key.key == SDLK_UP || event.key.key == SDLK_W)
                    {
                        opSelected--;
                        if (opSelected < 0)
                            opSelected = 3;
                    }

                    if (event.key.key == SDLK_DOWN || event.key.key == SDLK_S)
                    {
                        opSelected++;
                        if (opSelected > 3)
                            opSelected = 0;
                    }
                }
                else if (gameState() == FINISHED && event.key.key == SDLK_SPACE)
                {
                    setGameState(MAIN_MENU);

                    SDL_SetWindowFullscreen(window, false);
                    SDL_SetWindowSize(window, APP_MAINMENU_WIDTH, APP_MAINMENU_HEIGHT);
                    SDL_SetWindowTitle(window, "Potion Collector - Main Menu");

                    set_menu_presentation();
                    stopSound(&inGameMusic);
                }
                else if (gameState() == INGAME)
                {
                    if (event.key.key == SDLK_W)
                        player_move('W');
                    if (event.key.key == SDLK_A)
                        player_move('A');
                    if (event.key.key == SDLK_S)
                        player_move('S');
                    if (event.key.key == SDLK_D)
                        player_move('D');

                    if (player_collect_items())
                    {
                        printf("All potions collected! You win!\n");
                        setGameState(FINISHED);

                        SDL_SetWindowFullscreen(window, false);
                        SDL_SetWindowSize(window, APP_MAINMENU_WIDTH, APP_MAINMENU_HEIGHT);
                        SDL_SetWindowTitle(window, "Potion Collector - Game Finished");

                        // Finished screen uses menu-style logical sizing.
                        set_menu_presentation();
                    }
                }
            }
        }

        if (gameState() == INITIAL_SCREEN)
            renderInitialScreen();
        else if (gameState() == MAIN_MENU)
            renderMainMenu();
        else if (gameState() == INGAME)
            renderGame();
        else if (gameState() == FINISHED)
            renderGameFinished();

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
                SDL_RenderTexture(renderer, grassTexture, NULL, &dst_rect);
            else if (map_get_tile(x, y) == TILE_WALL)
                SDL_RenderTexture(renderer, wallTexture, NULL, &dst_rect);
        }
    }
}

void renderPlayer()
{
    // FIX: Y uses TEXTURE_HEIGHT, not TEXTURE_WIDTH.
    SDL_FRect wizard_rect = {
        TEXTURE_WIDTH * player_get_col(),
        TEXTURE_HEIGHT * player_get_row(),
        TEXTURE_WIDTH, TEXTURE_HEIGHT};
    SDL_RenderTexture(renderer, wizardTexture, NULL, &wizard_rect);
}

void renderPotions()
{
    Potion *potions = get_potions();
    for (int i = 0; i < 10; i++)
    {
        if (!potions[i].visible)
            continue;

        SDL_FRect potionRect = {TEXTURE_WIDTH * potions[i].x, TEXTURE_HEIGHT * potions[i].y, TEXTURE_WIDTH, TEXTURE_HEIGHT};
        SDL_RenderTexture(renderer, potionTexture, NULL, &potionRect);
    }
}

void renderGame(void)
{
    const int charsize = SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE;

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);

    renderMap();
    renderPlayer();
    renderPotions();

    showText(renderer, 100, 0, "Potion Collector!", (SDL_Color){255, 255, 255, SDL_ALPHA_OPAQUE});
    SDL_RenderDebugTextFormat(renderer,
                              (float)((APP_WIDTH - (charsize * 46)) / 2),
                              APP_HEIGHT - charsize,
                              "(This program has been running for %" SDL_PRIu64 " seconds.)",
                              SDL_GetTicks() / 1000);

    SDL_RenderPresent(renderer);
}

void renderMainMenu(void)
{
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);

    SDL_FRect logoRect = {0, 0, 512, 512};
    SDL_RenderTexture(renderer, bgTexture, NULL, &logoRect);

    if (opSelected == 0)
        showText(renderer, (float)((APP_MAINMENU_WIDTH - (SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE * 15)) / 2), 50,
                 "-> START GAME", (SDL_Color){255, 255, 0, SDL_ALPHA_OPAQUE});
    else
        showText(renderer, (float)((APP_MAINMENU_WIDTH - (SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE * 15)) / 2), 50,
                 "   START GAME", (SDL_Color){255, 255, 255, SDL_ALPHA_OPAQUE});

    if (opSelected == 1)
        showText(renderer, (float)((APP_MAINMENU_WIDTH - (SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE * 15)) / 2), 100,
                 "-> SELECT LEVEL", (SDL_Color){255, 255, 0, SDL_ALPHA_OPAQUE});
    else
        showText(renderer, (float)((APP_MAINMENU_WIDTH - (SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE * 15)) / 2), 100,
                 "   SELECT LEVEL", (SDL_Color){255, 255, 255, SDL_ALPHA_OPAQUE});

    if (opSelected == 2)
        showText(renderer, (float)((APP_MAINMENU_WIDTH - (SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE * 15)) / 2), 150,
                 "-> OPTIONS", (SDL_Color){255, 255, 0, SDL_ALPHA_OPAQUE});
    else
        showText(renderer, (float)((APP_MAINMENU_WIDTH - (SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE * 15)) / 2), 150,
                 "   OPTIONS", (SDL_Color){255, 255, 255, SDL_ALPHA_OPAQUE});

    if (opSelected == 3)
        showText(renderer, (float)((APP_MAINMENU_WIDTH - (SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE * 15)) / 2), 200,
                 "-> EXIT", (SDL_Color){255, 255, 0, SDL_ALPHA_OPAQUE});
    else
        showText(renderer, (float)((APP_MAINMENU_WIDTH - (SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE * 15)) / 2), 200,
                 "   EXIT", (SDL_Color){255, 255, 255, SDL_ALPHA_OPAQUE});

    showText(renderer, (float)((APP_MAINMENU_WIDTH - (SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE * 34)) / 2), 475,
             "<< PRESS SPACE TO SELECT OPTION >>", (SDL_Color){255, 255, 255, SDL_ALPHA_OPAQUE});

    SDL_RenderPresent(renderer);
}

void renderInitialScreen(void)
{
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);

    SDL_FRect logoRect = {0, 0, 512, 512};
    SDL_RenderTexture(renderer, logoTexture, NULL, &logoRect);

    showText(renderer, (float)((APP_MAINMENU_WIDTH - (SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE * 25)) / 2), 475,
             "<< PRESS SPACE TO START>>", (SDL_Color){255, 255, 255, SDL_ALPHA_OPAQUE});

    SDL_RenderPresent(renderer);
}

void renderGameFinished(void)
{
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);

    showText(renderer, 100, 50, "You Won!", (SDL_Color){255, 255, 255, SDL_ALPHA_OPAQUE});
    showText(renderer, 100, 75, "Press SpaceBar to go to Main Menu!", (SDL_Color){255, 255, 255, SDL_ALPHA_OPAQUE});

    SDL_RenderPresent(renderer);
}