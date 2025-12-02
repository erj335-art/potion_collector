#include <stdio.h>
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

#include "map.h"
#include "player.h"
#include "potion.h"

#define APP_NAME "Potion Collector"

#define APP_WIDTH 800
#define APP_HEIGHT 600

static Uint8 *wav_data = NULL;
static Uint32 wav_data_len = 0;
static SDL_AudioStream *stream = NULL;

static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;
static SDL_AudioDeviceID audio_device = 0;

static SDL_Texture *texture = NULL;
static SDL_Texture *wizardTexture = NULL;
static SDL_Texture *potionTexture = NULL;
static int texture_width = 0;
static int texture_height = 0;

/* things that are playing sound (the audiostream itself, plus the original data, so we can refill to loop. */
typedef struct Sound
{
    Uint8 *wav_data;
    Uint32 wav_data_len;
    SDL_AudioStream *stream;
} Sound;

static Sound sounds[2];

void renderTextures(void);
void playSound(Sound *sound);

static bool init_sound(const char *fname, Sound *sound)
{
    bool retval = false;
    SDL_AudioSpec spec;
    char *wav_path = NULL;

    /* Load the .wav files from wherever the app is being run from. */
    SDL_asprintf(&wav_path, "%s%s", SDL_GetBasePath(), fname); /* allocate a string of the full file path */
    if (!SDL_LoadWAV(wav_path, &spec, &sound->wav_data, &sound->wav_data_len))
    {
        SDL_Log("Couldn't load .wav file: %s", SDL_GetError());
        return false;
    }

    /* Create an audio stream. Set the source format to the wav's format (what
       we'll input), leave the dest format NULL here (it'll change to what the
       device wants once we bind it). */
    sound->stream = SDL_CreateAudioStream(&spec, NULL);
    if (!sound->stream)
    {
        SDL_Log("Couldn't create audio stream: %s", SDL_GetError());
    }
    else if (!SDL_BindAudioStream(audio_device, sound->stream))
    { /* once bound, it'll start playing when there is data available! */
        SDL_Log("Failed to bind '%s' stream to device: %s", fname, SDL_GetError());
    }
    else
    {
        retval = true; /* success! */
    }

    SDL_free(wav_path); /* done with this string. */
    return retval;
}

char readUserInput(void);
void renderTextures(void);
void playSound(Sound *sound);

int main(void)
{
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO))
    {
        SDL_Log("SDL_Init error: %s", SDL_GetError());
        return 1;
    }

    window = SDL_CreateWindow(
        APP_NAME,
        APP_WIDTH,
        APP_HEIGHT,
        SDL_WINDOW_RESIZABLE);

    if (!window)
    {
        SDL_Log("SDL_CreateWindow error: %s", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);

    renderer = SDL_CreateRenderer(window, NULL);
    if (!renderer)
    {
        SDL_Log("SDL_CreateRenderer error: %s", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    /* open the default audio device in whatever format it prefers; our audio streams will adjust to it. */
    audio_device = SDL_OpenAudioDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, NULL);
    if (audio_device == 0)
    {
        SDL_Log("Couldn't open audio device: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    if (!init_sound("sounds/dash.wav", &sounds[0]))
        return SDL_APP_FAILURE;
    else if (!init_sound("sounds/yummy.wav", &sounds[1]))
        return SDL_APP_FAILURE;

    // Load textures
    SDL_Surface *surface = NULL;
    char *png_path = NULL;

    SDL_asprintf(&png_path, "%ssprites/tiles/tile000.png", SDL_GetBasePath());
    surface = IMG_Load(png_path);
    if (!surface)
    {
        SDL_Log("IMG_LoadPNG_IO error: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }
    SDL_free(png_path);

    texture_width = surface->w;
    texture_height = surface->h;

    texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture)
    {
        SDL_Log("Couldn't create static texture: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    SDL_DestroySurface(surface);

    // Create the wizard texture
    SDL_asprintf(&png_path, "%ssprites/characters/wizard.png", SDL_GetBasePath());
    surface = IMG_Load(png_path);
    if (!surface)
    {
        SDL_Log("IMG_LoadPNG_IO error: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }
    SDL_free(png_path); 
    wizardTexture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!wizardTexture)
    {
        SDL_Log("Couldn't create wizard texture: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }
    SDL_DestroySurface(surface);

    // Create the potion texture
    SDL_asprintf(&png_path, "%ssprites/items/potion.png", SDL_GetBasePath());
    surface = IMG_Load(png_path);
    if (!surface)
    {
        SDL_Log("IMG_LoadPNG_IO error: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }
    SDL_free(png_path); 
    potionTexture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!potionTexture)
    {
        SDL_Log("Couldn't create potion texture: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }
    SDL_DestroySurface(surface);

    map_load("maps/level1.txt");

    // Game Loop
    int running = 1;
    const Uint32 FRAME_MS = 16; // ~60 FPS
    while (running)
    {
        Uint32 frame_start = SDL_GetTicks();

        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_EVENT_QUIT)
                running = 0;

            if (event.type == SDL_EVENT_KEY_DOWN)
            {
                playSound(&sounds[0]);

                if (event.key.key == SDLK_W)
                    player_move('W');
                if (event.key.key == SDLK_A)
                    player_move('A');
                if (event.key.key == SDLK_S)
                    player_move('S');
                if (event.key.key == SDLK_D)
                    player_move('D');

                if (player_collect_items())
                    playSound(&sounds[1]);
            }
        }

        renderTextures();
        // map_print(player_get_row(), player_get_col());

        Uint32 elapsed = SDL_GetTicks() - frame_start;
        if (elapsed < FRAME_MS)
            SDL_Delay(FRAME_MS - elapsed);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

void renderTextures(void)
{
    const int charsize = SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE;

    /* as you can see from this, rendering draws over whatever was drawn before it. */
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE); /* black, full alpha */
    SDL_RenderClear(renderer);                                   /* start with a blank canvas. */

    for (int x = 0; x < MAP_ROWS; x++)
    {
        for (int y = 0; y < MAP_COLS; y++)
        {
            SDL_FRect dst_rect;
            float scale = 1.0f;
            dst_rect.x = 32 * x;
            dst_rect.y = 32 * y;
            dst_rect.w = (float)texture_width;
            dst_rect.h = (float)texture_height;
            SDL_RenderTexture(renderer, texture, NULL, &dst_rect);
        }
    }

    // Render the wizard at the player's position
    SDL_FRect wizard_rect;
    float scale = 1.0f;
    wizard_rect.x = 32 * player_get_col();
    wizard_rect.y = 32 * player_get_row();
    wizard_rect.w = (float)texture_width;
    wizard_rect.h = (float)texture_height;
    SDL_RenderTexture(renderer, wizardTexture, NULL, &wizard_rect);

    // Render all potions
    Potion* potions = get_potions();
    for(int i = 0; i < 10; i++) {
        if (!potions[i].visible) continue;

            SDL_FRect potionRect;
            float scale = 1.0f;
            potionRect.x = 32 * potions[i].x;
            potionRect.y = 32 * potions[i].y;
            potionRect.w = (float)texture_width;
            potionRect.h = (float)texture_height;
            SDL_RenderTexture(renderer, potionTexture, NULL, &potionRect);
    }

     SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE); /* white, full alpha */
    SDL_RenderDebugText(renderer, 272, 100, "Hello world!");
    SDL_RenderDebugText(renderer, 224, 150, "This is some debug text.");

    SDL_SetRenderDrawColor(renderer, 51, 102, 255, SDL_ALPHA_OPAQUE); /* light blue, full alpha */
    SDL_RenderDebugText(renderer, 184, 200, "You can do it in different colors.");
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE); /* white, full alpha */

    SDL_SetRenderScale(renderer, 4.0f, 4.0f);
    SDL_RenderDebugText(renderer, 14, 65, "It can be scaled.");
    SDL_SetRenderScale(renderer, 1.0f, 1.0f);
    SDL_RenderDebugText(renderer, 64, 350, "This only does ASCII chars. So this laughing emoji won't draw: 🤣");

    SDL_RenderDebugTextFormat(renderer, (float)((APP_WIDTH - (charsize * 46)) / 2), (APP_HEIGHT / 2), "(This program has been running for %" SDL_PRIu64 " seconds.)", SDL_GetTicks() / 1000);


    SDL_RenderPresent(renderer); /* put it all on the screen! */
}

void playSound(Sound *sound)
{
    SDL_ClearAudioStream(sound->stream);
    if (SDL_GetAudioStreamQueued(sound->stream) < ((int)sound->wav_data_len))
        SDL_PutAudioStreamData(sound->stream, sound->wav_data, (int)sound->wav_data_len);
}