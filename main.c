// These concatenations are done by the makefile
// #include "api.h"
// #include "shaders.h"

// main
int main() {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_CreateWindow("testi", 0, 0, 640, 480, SDL_WINDOW_OPENGL);
    while (SDL_GetTicks() < 10000);
    return 0;
}

