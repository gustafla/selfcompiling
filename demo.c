#ifdef DEBUG
#include "includes.c"
#include "gl.c"
#include "shaders.h.out"
#endif

char **SCENES[] = {&TESTI_FRAG};

typedef struct {
    unsigned scenes;
    unsigned current_scene;
    SDL_Window *window;
    GLuint shader;
} demo_t;


demo_t *demo_init(SDL_Window *window) {
    demo_t *demo = malloc(sizeof(demo));

    demo->scenes = (sizeof(SCENES)/sizeof(SCENES[0]));
    demo->current_scene = 0;
    demo->window = window;
    demo->shader = link_program(TRIVIAL_VERT, *SCENES[demo->current_scene]);
    if (!demo->shader) {
        free(demo);
        return NULL;
    }

    return demo;
}

void demo_recompile(demo_t *demo) {
    glDeleteProgram(demo->shader);
    demo->shader = link_program(TRIVIAL_VERT, *SCENES[demo->current_scene]);
}

void demo_next(demo_t *demo) {
    demo->current_scene = (demo->current_scene + 1) % demo->scenes;
    demo_recompile(demo);
}

void demo_render(demo_t *demo) {
    GLfloat const vertices[] = {
        -1., -1., 0., 0., 0.,
        1., -1., 0., 1., 0.,
        1., 1., 0., 1., 1.,
        1., 1., 0., 1., 1.,
        -1., 1., 0., 0., 1.,
        -1., -1., 0., 0., 0.
    };
    size_t sf = sizeof(GLfloat);
    int w, h;
    SDL_GetWindowSize(demo->window, &w, &h);

    glUseProgram(demo->shader);
    glUniform1f(glGetUniformLocation(demo->shader, "u_time"), (GLfloat)SDL_GetTicks()/1000.f);
    glUniform1f(glGetUniformLocation(demo->shader, "u_aspect_ratio"), (GLfloat)w/h);

    GLint a_pos = glGetAttribLocation(demo->shader, "a_pos");
    GLint a_texpos = glGetAttribLocation(demo->shader, "a_texpos");
    glVertexAttribPointer(a_pos, 3, GL_FLOAT, 0, 5 * sf, (void*)vertices);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(a_texpos, 2, GL_FLOAT, 0, 5 * sf, (void*)(vertices+3));
    glEnableVertexAttribArray(1);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}
