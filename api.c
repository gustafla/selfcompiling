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

typedef enum {
    SDL_GL_CONTEXT_MAJOR_VERSION = 17,
    SDL_GL_CONTEXT_MINOR_VERSION = 18,
    SDL_GL_CONTEXT_PROFILE_MASK = 21
} SDL_GLattr;

typedef enum {
    SDL_GL_CONTEXT_PROFILE_CORE = 0x1,
    SDL_GL_CONTEXT_PROFILE_ES = 0x4
} SDL_GLprofile;

int SDL_Init(uint32_t);
SDL_Window *SDL_CreateWindow(const char*, int, int, int, int, uint32_t);
uint32_t SDL_GetTicks(void);
int SDL_GL_SetAttribute(SDL_GLattr, int);
void *SDL_GL_CreateContext(SDL_Window*);
void SDL_GL_SwapWindow(SDL_Window*);
int SDL_PollEvent(SDL_Event*);
uint32_t SDL_OpenAudioDevice(const char*, int,
const SDL_AudioSpec*, const SDL_AudioSpec*, int);
void SDL_PauseAudioDevice(uint32_t, int);

const uint16_t AUDIO_F32 = 0x8120;
const uint32_t SDL_QUIT = 0x100, SDL_KEYDOWN = 0x300;
const uint32_t SDL_INIT_VIDEO = 0x20, SDL_INIT_AUDIO = 0x10;
const uint32_t SDL_WINDOW_FULLSCREEN = 0x1, SDL_WINDOW_OPENGL = 0x2;

// ------------------------------------ OpenGL API -----------------------------
//
// SDL_GL_GetProcAddress is not needed on most sane unixlike systems,
// especially for this programs as simple as this
//
typedef int GLsizei;
typedef int GLint;
typedef ptrdiff_t GLsizeiptr;
typedef unsigned int GLenum;
typedef unsigned int GLuint;
typedef unsigned int GLbitfiled;
typedef unsigned char GLboolean;
typedef signed char GLbyte;
typedef char GLchar;
typedef float GLfloat;

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
int glGetAttribLocation(unsigned, const char*);
void glUniform1f(int, float);

const unsigned GL_COLOR_BUFFER_BIT = 0x4000;
const unsigned GL_FLOAT = 0x1406;
const unsigned GL_ARRAY_BUFFER = 0x8892;
const unsigned GL_TRIANGLES = 0x0004;
const unsigned GL_STATIC_DRAW = 0x88E4;
const unsigned GL_VERTEX_SHADER = 0x8B31, GL_FRAGMENT_SHADER = 0x8B30;
const unsigned GL_COMPILE_STATUS = 0x8B81, GL_LINK_STATUS = 0x8B82;
