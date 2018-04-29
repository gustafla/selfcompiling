#include <stdint.h>
#include <stddef.h>

// APIENTRY is not needed as this code is not meant for Winblows

// ------------------------------------ SDL API --------------------------------
typedef struct SDL_Window W;

typedef union {
    uint32_t type;
    uint8_t p[56];
} E;

typedef struct {
    int freq;
    uint16_t format;
    uint8_t channels;
    uint8_t silence;
    uint16_t samples;
    uint16_t padding;
    uint32_t size;
    void (*callback)(void*, uint8_t*, int);
    void *userdata;
} A;

int SDL_Init(uint32_t);
W *SDL_CreateWindow(const char*, int, int, int, int, uint32_t);
uint32_t SDL_GetTicks(void);
int SDL_GL_SetAttribute(char, int);
void *SDL_GL_CreateContext(W*);
void SDL_GL_SwapWindow(W*);
void *SDL_GL_GetProcAddress(const char*);
int SDL_PollEvent(E*);
uint32_t SDL_OpenAudioDevice(const char*, int, const A*, const A*, int);
void SDL_PauseAudioDevice(uint32_t, int);

// ------------------------------------ OpenGL API -----------------------------
//
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

// GetProcAddress function pointers
// These could be implemented as short functions with custom names which call
// the function directly from GetProcAddress. Consider it if size becomes scarce

void (*glGenBuffers)(unsigned, unsigned*); // size, buffers
void (*glBindBuffer)(unsigned, unsigned); // target, buffer
void (*glBufferData)(unsigned, ptrdiff_t, const void*, unsigned);
unsigned (*glCreateShader)(unsigned);
void (*glShaderSource)(unsigned, int, const char**, const int*);
void (*glCompileShader)(unsigned);
void (*glGetShaderiv)(unsigned, unsigned, int*);
void (*glGetShaderInfoLog)(unsigned, int, int*, char*);
unsigned (*glCreateProgram)(void);
void (*glAttachShader)(unsigned, unsigned);
void (*glLinkProgram)(unsigned);
void (*glGetProgramiv)(unsigned, unsigned, int*);
void (*glGetProgramInfoLog)(unsigned, int, int*, char*);
void (*glUseProgram)(unsigned);

void gl() {
    glGenBuffers = (void(*)(unsigned, unsigned*))
        SDL_GL_GetProcAddress("glGenBuffers");
    glBindBuffer = (void(*)(unsigned, unsigned))
        SDL_GL_GetProcAddress("glBindBuffer");
    glBufferData = (void(*)(unsigned, ptrdiff_t, const void*, unsigned))
        SDL_GL_GetProcAddress("glBufferData");
    glCreateShader = (unsigned(*)(unsigned))
        SDL_GL_GetProcAddress("glCreateShader");
    glShaderSource = (void(*)(unsigned, int, const char**, const int*))
        SDL_GL_GetProcAddress("glShaderSource");
    glCompileShader = (void(*)(unsigned))
        SDL_GL_GetProcAddress("glCompileShader");
    glGetShaderiv = (void(*)(unsigned, unsigned, int*))
        SDL_GL_GetProcAddress("glGetShaderiv");
    glGetShaderInfoLog = (void(*)(unsigned, int, int*, char*))
        SDL_GL_GetProcAddress("glGetShaderInfoLog");
    glCreateProgram = (unsigned(*)(void))
        SDL_GL_GetProcAddress("glCreateProgram");
    glAttachShader = (void(*)(unsigned, unsigned))
        SDL_GL_GetProcAddress("glAttachShader");
    glLinkProgram = (void(*)(unsigned))
        SDL_GL_GetProcAddress("glLinkProgram");
    glGetProgramiv = (void(*)(unsigned, unsigned, int*))
        SDL_GL_GetProcAddress("glGetProgramiv");
    glGetProgramInfoLog = (void(*)(unsigned, int, int*, char*))
        SDL_GL_GetProcAddress("glGetProgramInfoLog");
    glUseProgram = (void(*)(unsigned))
        SDL_GL_GetProcAddress("glUseProgram");
}
