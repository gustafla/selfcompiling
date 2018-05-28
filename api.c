#include <stdint.h>
#include <stddef.h>

// APIENTRY is not needed as this code is not meant for Winblows

// ------------------------------------ SDL API --------------------------------
typedef struct SDL_Window SDL_Window;

typedef union {
    uint32_t type;
    uint8_t p[56];
} SDL_Event;

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
} SDL_AudioSpec;

int SDL_Init(uint32_t);
SDL_Window *SDL_CreateWindow(const char*, int, int, int, int, uint32_t);
uint32_t SDL_GetTicks(void);
int SDL_GL_SetAttribute(char, int);
void *SDL_GL_CreateContext(SDL_Window*);
void SDL_GL_SwapWindow(SDL_Window*);
int SDL_PollEvent(SDL_Event*);
uint32_t SDL_OpenAudioDevice(const char*, int,
        const SDL_AudioSpec*, const SDL_AudioSpec*, int);
void SDL_PauseAudioDevice(uint32_t, int);

// ------------------------------------ OpenGL API -----------------------------
//
// SDL_GL_GetProcAddress is not needed on most sane unixlike systems,
// especially for this programs as simple as this
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
void glGenBuffers(unsigned, unsigned*); // size, buffers
void glBindBuffer(unsigned, unsigned); // target, buffer
void glBufferData(unsigned, ptrdiff_t, const void*, unsigned);
unsigned glCreateShader(unsigned);
void glShaderSource(unsigned, int, const char**, const int*);
void glCompileShader(unsigned);
void glGetShaderiv(unsigned, unsigned, int*);
void glGetShaderInfoLog(unsigned, int, int*, char*);
unsigned glCreateProgram(void);
void glAttachShader(unsigned, unsigned);
void glLinkProgram(unsigned);
void glGetProgramiv(unsigned, unsigned, int*);
void glGetProgramInfoLog(unsigned, int, int*, char*);
void glUseProgram(unsigned);
void glGenVertexArrays(int, unsigned*);
void glBindVertexArray(unsigned);
void glVertexAttribPointer(unsigned, int, unsigned, unsigned char, int, const void*);
void glEnableVertexAttribArray(unsigned);
int glGetUniformLocation(unsigned, const char*);
void glUniform1f(int, float);
