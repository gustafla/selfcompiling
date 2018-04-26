#include <stdint.h>

// SDL API
uint32_t SDL_INIT_VIDEO=0x00000020;
uint32_t SDL_WINDOW_FULLSCREEN=0x00000001;
uint32_t SDL_WINDOW_OPENGL=0x00000002;
uint32_t SDL_QUIT=0x100;
uint32_t SDL_KEYDOWN=0x300;
int32_t SDLK_ESCAPE='\033';

typedef struct SDL_Window SDL_Window;

typedef enum {
    SDL_GL_CONTEXT_MAJOR_VERSION=17,
    SDL_GL_CONTEXT_MINOR_VERSION=18,
    SDL_GL_CONTEXT_PROFILE_MASK=21
} SDL_GLattr;

typedef union SDL_Event {
    uint32_t type;
    uint8_t padding[56];
} SDL_Event;

int SDL_Init(uint32_t flags);
SDL_Window *SDL_CreateWindow(const char *title, int x, int y, int w, int h, uint32_t flags);
uint32_t SDL_GetTicks(void);
int SDL_GL_SetAttribute(SDL_GLattr attr, int value);
void *SDL_GL_CreateContext(SDL_Window *window);
void SDL_GL_SwapWindow(SDL_Window *window);
int SDL_PollEvent(SDL_Event *event);

// EGL API
void *eglGetProcAddress(const char *procname);

// libGL.so API

