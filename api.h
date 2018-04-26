#include <stdint.h>

// SDL API
typedef uint32_t Uint32;

Uint32 SDL_INIT_VIDEO=0x00000020;
Uint32 SDL_WINDOW_FULLSCREEN=0x00000001;
Uint32 SDL_WINDOW_OPENGL=0x00000002;

typedef struct SDL_Window SDL_Window;

int SDL_Init(Uint32 flags);
SDL_Window *SDL_CreateWindow(const char *title, int x, int y,
        int w, int h, Uint32 flags);
Uint32 SDL_GetTicks(void);

// EGL API
void *eglGetProcAddress(const char *procname);
