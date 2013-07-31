#include <SDL/SDL.h>
#include <assert.h>
#include <stdint.h>
#include <stdlib.h>

#include <stdint.h>
#include <stdbool.h>

#define WIDTH 98
#define HEIGHT 70

static uint8_t buffer[WIDTH * HEIGHT * 3];
static uint32_t i;
static SDL_Surface * data_sf;

/*
 * Based on http://stackoverflow.com/questions/1391314
 */
#define mask32(BYTE) (*(uint32_t *)(uint8_t [4]){ [BYTE] = 0xff })

static _Bool init_app(const char * name, SDL_Surface * icon, uint32_t flags)
{
    atexit(SDL_Quit);
    if(SDL_Init(flags) < 0)
        return 0;

    SDL_WM_SetCaption(name, name);
    SDL_WM_SetIcon(icon, NULL);

    return 1;
}

void lcd_init(void)
{
    _Bool ok =
        init_app("LCD", NULL, SDL_INIT_VIDEO) &&
        SDL_SetVideoMode(WIDTH, HEIGHT, 24, SDL_HWSURFACE);

    assert(ok);

    data_sf = SDL_CreateRGBSurfaceFrom(
        buffer, WIDTH, HEIGHT, 24, WIDTH * 3,
        mask32(0), mask32(1), mask32(2), 0);
}

static void render(SDL_Surface * sf)
{
    SDL_Surface * screen = SDL_GetVideoSurface();
    if(SDL_BlitSurface(sf, NULL, screen, NULL) == 0)
        SDL_UpdateRect(screen, 0, 0, 0, 0);
}

void lcd_startStream(void)
{
    i = 0;
}

void lcd_stream(uint8_t r, uint8_t g, uint8_t b)
{
    buffer[i++] = r;
    buffer[i++] = g;
    buffer[i++] = b;
}

void lcd_stopStream(void)
{
    render(data_sf);
}

