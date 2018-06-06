#include <stdio.h>

#include <SDL.h>
#include <GLES2/gl2.h>

#ifdef DEBUG
#include "read_file.c"
#include "shaders.h.out"
#include "music.xm.c"
#include "xmplayer.c"
#endif

char **SCENES[] = {&PALLOSCENE_FRAG, &TESTI_FRAG};

void play(void *d, uint8_t *stream, int len) {
    xm_generate_samples((xm_context_t*)d, (float*)stream, (len/8));
}

xm_context_t *start_music(const char *moddata, size_t moddata_len) {
    xm_context_t *xm;
    xm_create_context_safe(&xm, moddata, moddata_len, 48000);

    SDL_AudioSpec want, have;
    want.freq = 48000;
    want.format = AUDIO_F32;
    want.channels = 2;
    want.samples = 2048;
    want.callback = play;
    want.userdata = (void*)xm;

    uint32_t dev = SDL_OpenAudioDevice(NULL, 0, &want, &have, 0);
    SDL_PauseAudioDevice(dev, 0);
    return xm;
}

GLuint compile_shader(GLenum type, char *src) {
    GLuint s = glCreateShader(type);

    // add correct header to shader
    char *full_src[3];
    full_src[0] = "#version 100\nprecision mediump float;\n";
    switch (type) {
        case GL_VERTEX_SHADER:
            full_src[1] = "attribute vec3 a_pos;\n"
                "attribute vec2 a_texpos;\n"
                "varying vec3 v_pos;\n"
                "varying vec2 v_texpos;\n";
            break;
        case GL_FRAGMENT_SHADER:
            full_src[1] = "uniform float u_time;\n"
                "varying vec2 v_texpos;\n";
    }
#ifdef DEBUG
    read_file_to_str(&full_src[2], src);
#else
    full_src[2] = src;
#endif

    glShaderSource(s, 3, (const GLchar**)full_src, NULL);
    glCompileShader(s);
    GLint success; GLchar info[512];
    glGetShaderiv(s, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(s, 512, NULL, info);
        printf("%s", info);
    }
    return s;
}

// Params are filenames in debug builds, but shader source code otherwise
GLuint link_program(char *vertex_src, char *fragment_src) {
    GLuint p = glCreateProgram();
    glAttachShader(p, compile_shader(GL_VERTEX_SHADER, vertex_src));
    glAttachShader(p, compile_shader(GL_FRAGMENT_SHADER, fragment_src));
    glLinkProgram(p);
    GLint success; GLchar info[512];
    glGetProgramiv(p, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(p, 512, NULL, info);
        printf("%s", info);
        return 0;
    }
    return p;
}

void render(GLuint s) {
    GLfloat const vertices[] = {
        -1., -1., 0., 0., 0.,
        1., -1., 0., 1., 0.,
        1., 1., 0., 1., 1.,
        1., 1., 0., 1., 1.,
        -1., 1., 0., 0., 1.,
        -1., -1., 0., 0., 0.
    };
    size_t sf = sizeof(GLfloat);
    GLint a_pos = glGetAttribLocation(s, "a_pos");
    GLint a_texpos = glGetAttribLocation(s, "a_texpos");
    glVertexAttribPointer(a_pos, 3, GL_FLOAT, 0, 5 * sf, (void*)vertices);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(a_texpos, 2, GL_FLOAT, 0, 5 * sf, (void*)(vertices+3));
    glEnableVertexAttribArray(1);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

int main() {
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);

    SDL_Window *w = SDL_CreateWindow("test", 0, 0, 640, 480, SDL_WINDOW_OPENGL);
    SDL_GL_CreateContext(w);

    // Compile and link shaders
    unsigned scene = 0;
    GLuint s = link_program(TRIVIAL_VERT, *SCENES[scene]);
    if (!s) {
        return EXIT_FAILURE;
    }

    // Start playing music
    //xm_context_t *xm = start_music(MUSIC_XM, MUSIC_XM_LEN);

    SDL_Event e;
    while(1) {
        //glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(s);
        glUniform1f(glGetUniformLocation(s, "u_time"), (GLfloat)SDL_GetTicks()/1000.f);

        render(s);

        SDL_GL_SwapWindow(w);

        SDL_PollEvent(&e);
#ifdef DEBUG
        if (e.type == SDL_QUIT) {
            break;
        } else if (e.type == SDL_KEYDOWN) {
            if (e.key.keysym.sym == SDLK_ESCAPE || e.key.keysym.sym == SDLK_q) {
                break;
            } else if (e.key.keysym.sym == SDLK_r) {
                s = link_program(TRIVIAL_VERT, *SCENES[scene]);
            } else if (e.key.keysym.sym == SDLK_n) {
                scene = (scene + 1) % (sizeof(SCENES)/sizeof(SCENES[0]));
                s = link_program(TRIVIAL_VERT, *SCENES[scene]);
            }
        }
#else
        if (e.type == SDL_QUIT || e.type == SDL_KEYDOWN) break;
#endif
    }

    return 0;
}
