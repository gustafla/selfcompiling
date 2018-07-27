#ifdef DEBUG
#include "includes.c"
#endif

typedef struct {
    size_t tick; // Current seq position
    int speed; // Ticks per beat
    int bps; // Beats per second
    int out_channels; // Audio ouput channels
    int out_rate; // Audio output sample rate
} synth_t;

synth_t *synth_init(const char *track) {
    synth_t *synth = malloc(sizeof(synth_t));
    memset(synth, 0, sizeof(synth_t));

    return synth;
}

void synth_render(synth_t *synth, float *buffer, size_t samples) {
    
}
