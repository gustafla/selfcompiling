#include <stdint.h>
#include <stddef.h>

// ------------------------------------ SDL API --------------------------------
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
void *SDL_GL_GetProcAddress(const char*);
int SDL_PollEvent(E*);

// ------------------------------------ OpenGL API -----------------------------
//
// APIENTRY is not needed as this code is not meant for Winblows
//
// typedef int GLsizei;
// typedef int GLint;
// typedef ptrdiff_t GLsizeiptr;
// typedef unsigned int GLenum;
// typedef unsigned int GLuint;
// typedef unsigned int GLbitfiled;
// typedef unsigned char GLboolean;
// typedef signed char GLbyte;
void glClearColor(float, float, float, float); // rgba
void glClear(unsigned); // mask
void glDrawArrays(unsigned, int, int); // mode, first, count

// GetProcAddress calls
// These could be implemented as short functions with custom names which call
// the function directly from GetProcAddress. Consider it if size becomes scarce

void (*glGenBuffers)(unsigned, unsigned*); // size, buffers
void (*glBindBuffer)(unsigned, unsigned); // target, buffer
void (*glBufferData)(unsigned, ptrdiff_t, const void*, unsigned);
void gl() {
    glGenBuffers = (void(*)(unsigned, unsigned*))
        SDL_GL_GetProcAddress("glGenBuffers");
    glBindBuffer = (void(*)(unsigned, unsigned))
        SDL_GL_GetProcAddress("glBindBuffer");
    glBufferData = (void(*)(unsigned, ptrdiff_t, const void*, unsigned))
        SDL_GL_GetProcAddress("glBufferData");
}
