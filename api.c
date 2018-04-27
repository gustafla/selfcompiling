#include <stdint.h>

// SDL API
typedef struct SDL_Window W;

typedef union {
    uint32_t type;
    uint8_t p[56];
} E;

int SDL_Init(uint32_t flags);
W *SDL_CreateWindow(const char *title, int x, int y, int w, int h, uint32_t flags);
uint32_t SDL_GetTicks(void);
int SDL_GL_SetAttribute(char attr, int value);
void *SDL_GL_CreateContext(W *window);
void SDL_GL_SwapWindow(W *window);
int SDL_PollEvent(E *event);

// EGL API
void *eglGetProcAddress(const char *name);

// libGL.so API
void glClearColor(float r, float g, float b, float a);
void glClear(unsigned mask);
