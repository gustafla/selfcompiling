#include <stdint.h>

// SDL API
typedef struct SDL_Window W;

typedef union {
    uint32_t type;
    uint8_t p[56];
} E;

int SDL_Init(uint32_t);
W *SDL_CreateWindow(const char*, int, int, int, int, uint32_t);
uint32_t SDL_GetTicks(void);
int SDL_GL_SetAttribute(char, int);
void *SDL_GL_CreateContext(W*);
void SDL_GL_SwapWindow(W*);
int SDL_PollEvent(E*);

// EGL API
void *eglGetProcAddress(const char*);

// OpenGL API

// typedef unsigned int GLenum;
// typedef unsigned char GLboolean;
// typedef signed char GLbyte;
// typedef unsigned int GLbitfiled;
// typedef int GLsizei;
void glClearColor(float, float, float, float); // rgba
void glClear(unsigned); // mask
void glDrawArrays(unsigned, int, int); //mode, first, count
