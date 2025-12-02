#ifndef SDL_UTILS_H
#define SDL_UTILS_H

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <stdbool.h>

// =======================================================
// SOUND
// =======================================================
typedef struct Sound
{
    Uint8 *wav_data;
    Uint32 wav_data_len;
    SDL_AudioStream *stream;
} Sound;

int sdl_initialize_audio();
bool init_sound(const char *fname, Sound *sound);
void playSound(Sound *sound);

// =======================================================
// TEXTURE
// =======================================================

// Load a texture from a PNG/JPG/etc in the same folder as the executable.
// Returns NULL on error.
SDL_Texture *sdl_load_texture(SDL_Renderer *renderer, const char *file_path);


#endif // SDL_UTILS_H