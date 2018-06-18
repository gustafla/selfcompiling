#ifdef DEBUG
#include "includes.c"
#include "music.xm.c"
#include "xmplayer.c"
#include "demo.c"
#endif

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

int main() {
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);

    SDL_Window *w = SDL_CreateWindow("test", 0, 0, 1280, 720, SDL_WINDOW_OPENGL);
    SDL_GL_CreateContext(w);

    demo_t *demo = demo_init(w);

    // Start playing music
    xm_context_t *xm = start_music(MUSIC_XM, MUSIC_XM_LEN);

    SDL_Event e;
    while(1) {
        //glClear(GL_COLOR_BUFFER_BIT);

        demo_render(demo);

        SDL_GL_SwapWindow(w);

        SDL_PollEvent(&e);
#ifdef DEBUG
        if (e.type == SDL_QUIT) {
            break;
        } else if (e.type == SDL_KEYDOWN) {
            if (e.key.keysym.sym == SDLK_ESCAPE || e.key.keysym.sym == SDLK_q) {
                break;
            } else if (e.key.keysym.sym == SDLK_r) {
                demo_recompile(demo);
            } else if (e.key.keysym.sym == SDLK_n) {
                demo_next(demo);
            }
        }
#else
        if (e.type == SDL_QUIT || e.type == SDL_KEYDOWN) break;
#endif
    }

    return EXIT_SUCCESS;
}
