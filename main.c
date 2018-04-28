int main() {
    // SDL_INIT_VIDEO = 0x20, SDL_INIT_AUDIO = 0x10
    SDL_Init(0x30);

    // MAJOR_VERSION = 17, MINOR_VERSION = 18, PROFILE_MASK = 21
    SDL_GL_SetAttribute(17, 3);
    SDL_GL_SetAttribute(18, 3);
    // SDL_GL_CONTEXT_PROFILE_CORE == 0x1
    SDL_GL_SetAttribute(21, 1);

    // SDL_WINDOW_FULLSCREEN = 0x1, SDL_WINDOW_OPENGL = 0x2
    W *w = SDL_CreateWindow("testi", 0, 0, 640, 480, 2);
    SDL_GL_CreateContext(w);

    // Load OpenGL functions
    gl();

    // Event struct
    E e;
    while(1) {
        glClearColor(1.f, 0.f, 0.f, 1.f);
        // GL_COLOR_BUFFER_BIT = 0x4000
        glClear(0x4000);

        SDL_GL_SwapWindow(w);

        SDL_PollEvent(&e);
        // SDL_QUIT = 0x100, SDL_KEYDOWN = 0x300
        if (e.type == 0x100 || e.type == 0x300) break;
    }

    return 0;
}

