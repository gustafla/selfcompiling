// These concatenations are done by the makefile
// #include "api.h"
// #include "shaders.h"

int main() {
    SDL_Init(SDL_INIT_VIDEO);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    // SDL_GL_CONTEXT_PROFILE_CORE == 0x0001
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, 1);

    SDL_Window *w = SDL_CreateWindow("testi", 0, 0, 640, 480, SDL_WINDOW_OPENGL);
    SDL_GL_CreateContext(w);

    SDL_Event e;
    while(1) {
        SDL_GL_SwapWindow(w);

        SDL_PollEvent(&e);
        if (e.type == SDL_QUIT || e.type == SDL_KEYDOWN) {
            break;
        }
    }

    return 0;
}

