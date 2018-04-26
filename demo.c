/* SDL API declarations */

#define SDL_INIT_VIDEO 0x00000020
#define SDL_WINDOW_FULLSCREEN 0x00000001
#define SDL_WINDOW_OPENGL 0x00000002

typedef struct SDL_Window SDL_Window;

int SDL_Init(unsigned flags);
SDL_Window *SDL_CreateWindow(const char *title, int x, int y,
        int w, int h, unsigned flags);
unsigned SDL_GetTicks(void);

/* main */

int main() {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_CreateWindow("testi", 0, 0, 640, 480, SDL_WINDOW_OPENGL);
    while (SDL_GetTicks() < 10000);
    return 0;
}
