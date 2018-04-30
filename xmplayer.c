
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>

void memcpy_pad(void* dst, size_t dst_len, const void* src, size_t src_len, size_t offset) {
    uint8_t* dst_c = dst;
    const uint8_t* src_c = src;

    size_t copy_bytes = (src_len >= offset) ? (src_len - offset) : 0;
    copy_bytes = copy_bytes > dst_len ? dst_len : copy_bytes;

    memcpy(dst_c, src_c + offset, copy_bytes);
    memset(dst_c + copy_bytes, 0, dst_len - copy_bytes);
}

// ----- XM constants -----

#define SAMPLE_NAME_LENGTH 22
#define INSTRUMENT_NAME_LENGTH 22
#define MODULE_NAME_LENGTH 20
#define TRACKER_NAME_LENGTH 20
#define PATTERN_ORDER_TABLE_LENGTH 256
#define NUM_NOTES 96
#define NUM_ENVELOPE_POINTS 12
#define MAX_NUM_ROWS 256

#define XM_SAMPLE_RAMPING_POINTS 0x20

#define READ_U8(offset) (((offset) < moddata_length) ? (*(uint8_t*)(moddata + (offset))) : 0)
#define READ_U16(offset) ((uint16_t)READ_U8(offset) | ((uint16_t)READ_U8((offset) + 1) << 8))
#define READ_U32(offset) ((uint32_t)READ_U16(offset) | ((uint32_t)READ_U16((offset) + 2) << 16))
#define READ_MEMCPY(ptr, offset, length) memcpy_pad(ptr, length, moddata, moddata_length, offset)

#define XM_LINEAR_INTERPOLATION (1)

// ----- Data types -----

enum xm_waveform_type_e {
    XM_SINE_WAVEFORM = 0,
    XM_RAMP_DOWN_WAVEFORM = 1,
    XM_SQUARE_WAVEFORM = 2,
    XM_RANDOM_WAVEFORM = 3,
    XM_RAMP_UP_WAVEFORM = 4,
};
typedef enum xm_waveform_type_e xm_waveform_type_t;

enum xm_loop_type_e {
    XM_NO_LOOP,
    XM_FORWARD_LOOP,
    XM_PING_PONG_LOOP,
};
typedef enum xm_loop_type_e xm_loop_type_t;

enum xm_frequency_type_e {
    XM_LINEAR_FREQUENCIES,
    XM_AMIGA_FREQUENCIES,
};
typedef enum xm_frequency_type_e xm_frequency_type_t;

struct xm_envelope_point_s {
    uint16_t frame;
    uint16_t value;
};
typedef struct xm_envelope_point_s xm_envelope_point_t;

struct xm_envelope_s {
    xm_envelope_point_t points[NUM_ENVELOPE_POINTS];
    uint8_t num_points;
    uint8_t sustain_point;
    uint8_t loop_start_point;
    uint8_t loop_end_point;
    bool enabled;
    bool sustain_enabled;
    bool loop_enabled;
};
typedef struct xm_envelope_s xm_envelope_t;

struct xm_sample_s {
    uint8_t bits;

    uint32_t length;
    uint32_t loop_start;
    uint32_t loop_length;
    uint32_t loop_end;
    float volume;
    int8_t finetune;
    xm_loop_type_t loop_type;
    float panning;
    int8_t relative_note;
    uint64_t latest_trigger;

    union {
        int8_t* data8;
        int16_t* data16;
    };
};
typedef struct xm_sample_s xm_sample_t;

struct xm_instrument_s {
    uint16_t num_samples;
    uint8_t sample_of_notes[NUM_NOTES];
    xm_envelope_t volume_envelope;
    xm_envelope_t panning_envelope;
    xm_waveform_type_t vibrato_type;
    uint8_t vibrato_sweep;
    uint8_t vibrato_depth;
    uint8_t vibrato_rate;
    uint16_t volume_fadeout;
    uint64_t latest_trigger;
    bool muted;

    xm_sample_t* samples;
};
typedef struct xm_instrument_s xm_instrument_t;

struct xm_pattern_slot_s {
    uint8_t note;
    uint8_t instrument;
    uint8_t volume_column;
    uint8_t effect_type;
    uint8_t effect_param;
};
typedef struct xm_pattern_slot_s xm_pattern_slot_t;

struct xm_pattern_s {
    uint16_t num_rows;
    xm_pattern_slot_t* slots;
};
typedef struct xm_pattern_s xm_pattern_t;

struct xm_module_s {
    uint16_t length;
    uint16_t restart_position;
    uint16_t num_channels;
    uint16_t num_patterns;
    uint16_t num_instruments;
    xm_frequency_type_t frequency_type;
    uint8_t pattern_table[PATTERN_ORDER_TABLE_LENGTH];

    xm_pattern_t* patterns;
    xm_instrument_t* instruments;
};
typedef struct xm_module_s xm_module_t;

struct xm_channel_context_s {
    float note;
    float orig_note;
    xm_instrument_t* instrument;
    xm_sample_t* sample;
    xm_pattern_slot_t* current;

    float sample_position;
    float period;
    float frequency;
    float step;
    bool ping;

    float volume;
    float panning;

    uint16_t autovibrato_ticks;

    bool sustained;
    float fadeout_volume;
    float volume_envelope_volume;
    float panning_envelope_panning;
    uint16_t volume_envelope_frame_count;
    uint16_t panning_envelope_frame_count;

    float autovibrato_note_offset;

    bool arp_in_progress;
    uint8_t arp_note_offset;
    uint8_t volume_slide_param;
    uint8_t fine_volume_slide_param;
    uint8_t global_volume_slide_param;
    uint8_t panning_slide_param;
    uint8_t portamento_up_param;
    uint8_t portamento_down_param;
    uint8_t fine_portamento_up_param;
    uint8_t fine_portamento_down_param;
    uint8_t extra_fine_portamento_up_param;
    uint8_t extra_fine_portamento_down_param;
    uint8_t tone_portamento_param;
    float tone_portamento_target_period;
    uint8_t multi_retrig_param;
    uint8_t note_delay_param;
    uint8_t pattern_loop_origin;
    uint8_t pattern_loop_count;
    bool vibrato_in_progress;
    xm_waveform_type_t vibrato_waveform;
    bool vibrato_waveform_retrigger;
    uint8_t vibrato_param;
    uint16_t vibrato_ticks;
    float vibrato_note_offset;
    xm_waveform_type_t tremolo_waveform;
    bool tremolo_waveform_retrigger;
    uint8_t tremolo_param;
    uint8_t tremolo_ticks;
    float tremolo_volume;
    uint8_t tremor_param;
    bool tremor_on;

    uint64_t latest_trigger;
    bool muted;

    float target_panning;
    float target_volume;

    unsigned long frame_count;
    float end_of_previous_sample[XM_SAMPLE_RAMPING_POINTS];

    float actual_panning;
    float actual_volume;
};
typedef struct xm_channel_context_s xm_channel_context_t;

struct xm_context_s {
    size_t ctx_size;
    xm_module_t module;
    uint32_t rate;

    uint16_t tempo;
    uint16_t bpm;
    float global_volume;
    float amplification;

    float volume_ramp;
    float panning_ramp;

    uint8_t current_table_index;
    uint8_t current_row;
    uint16_t current_tick;
    float remaining_samples_in_tick;
    uint64_t generated_samples;

    bool position_jump;
    bool pattern_break;
    uint8_t jump_dest;
    uint8_t jump_row;

    uint16_t extra_ticks;

    uint8_t* row_loop_count;
    uint8_t loop_count;
    uint8_t max_loop_count;

    xm_channel_context_t* channels;
};
typedef struct xm_context_s xm_context_t;

size_t xm_get_memory_needed_for_context(const char* moddata, size_t moddata_length) {
    size_t memory_needed = 0;
    size_t offset = 60;
    uint16_t num_channels;
    uint16_t num_patterns;
    uint16_t num_instruments;

    num_channels = READ_U16(offset + 8);
    num_channels = READ_U16(offset + 8);

    num_patterns = READ_U16(offset + 10);
    memory_needed += num_patterns * sizeof(xm_pattern_t);

    num_instruments = READ_U16(offset + 12);
    memory_needed += num_instruments * sizeof(xm_instrument_t);

    memory_needed += MAX_NUM_ROWS * READ_U16(offset + 4) * sizeof(uint8_t);

    offset += READ_U32(offset);

    for(uint16_t i = 0; i < num_patterns; ++i) {
        uint16_t num_rows;

        num_rows = READ_U16(offset + 5);
        memory_needed += num_rows * num_channels * sizeof(xm_pattern_slot_t);

        offset += READ_U32(offset) + READ_U16(offset + 7);
    }

    for(uint16_t i = 0; i < num_instruments; ++i) {
        uint16_t num_samples;
        uint32_t sample_header_size = 0;
        uint32_t sample_size_aggregate = 0;

        num_samples = READ_U16(offset + 27);
        memory_needed += num_samples * sizeof(xm_sample_t);

        if(num_samples > 0) {
            sample_header_size = READ_U32(offset + 29);
        }

        offset += READ_U32(offset);

        for(uint16_t j = 0; j < num_samples; ++j) {
            uint32_t sample_size;

            sample_size = READ_U32(offset);
            sample_size_aggregate += sample_size;
            memory_needed += sample_size;
            offset += sample_header_size;
        }

        offset += sample_size_aggregate;
    }

    memory_needed += num_channels * sizeof(xm_channel_context_t);
    memory_needed += sizeof(xm_context_t);

    return memory_needed;
}

char* xm_load_module(xm_context_t* ctx, const char* moddata, size_t moddata_length, char* mempool) {
    size_t offset = 0;
    xm_module_t* mod = &(ctx->module);

    offset += 60;

    uint32_t header_size = READ_U32(offset);

    mod->length = READ_U16(offset + 4);
    mod->restart_position = READ_U16(offset + 6);
    mod->num_channels = READ_U16(offset + 8);
    mod->num_patterns = READ_U16(offset + 10);
    mod->num_instruments = READ_U16(offset + 12);

    mod->patterns = (xm_pattern_t*)mempool;
    mempool += mod->num_patterns * sizeof(xm_pattern_t);

    mod->instruments = (xm_instrument_t*)mempool;
    mempool += mod->num_instruments * sizeof(xm_instrument_t);

    uint16_t flags = READ_U32(offset + 14);
    mod->frequency_type = (flags & (1 << 0)) ? XM_LINEAR_FREQUENCIES : XM_AMIGA_FREQUENCIES;

    ctx->tempo = READ_U16(offset + 16);
    ctx->bpm = READ_U16(offset + 18);

    READ_MEMCPY(mod->pattern_table, offset + 20, PATTERN_ORDER_TABLE_LENGTH);
    offset += header_size;

    for(uint16_t i = 0; i < mod->num_patterns; ++i) {
        uint16_t packed_patterndata_size = READ_U16(offset + 7);
        xm_pattern_t* pat = mod->patterns + i;

        pat->num_rows = READ_U16(offset + 5);

        pat->slots = (xm_pattern_slot_t*)mempool;
        mempool += mod->num_channels * pat->num_rows * sizeof(xm_pattern_slot_t);

        offset += READ_U32(offset);

        if(packed_patterndata_size == 0) {
            memset(pat->slots, 0, sizeof(xm_pattern_slot_t) * pat->num_rows * mod->num_channels);
        } else {
            for(uint16_t j = 0, k = 0; j < packed_patterndata_size; ++k) {
                uint8_t note = READ_U8(offset + j);
                xm_pattern_slot_t* slot = pat->slots + k;

                if(note & (1 << 7)) {
                    ++j;

                    if(note & (1 << 0)) {
                        slot->note = READ_U8(offset + j);
                        ++j;
                    } else {
                        slot->note = 0;
                    }

                    if(note & (1 << 1)) {
                        slot->instrument = READ_U8(offset + j);
                        ++j;
                    } else {
                        slot->instrument = 0;
                    }

                    if(note & (1 << 2)) {
                        slot->volume_column = READ_U8(offset + j);
                        ++j;
                    } else {
                        slot->volume_column = 0;
                    }

                    if(note & (1 << 3)) {
                        slot->effect_type = READ_U8(offset + j);
                        ++j;
                    } else {
                        slot->effect_type = 0;
                    }

                    if(note & (1 << 4)) {
                        slot->effect_param = READ_U8(offset + j);
                        ++j;
                    } else {
                        slot->effect_param = 0;
                    }
                } else {
                    slot->note = note;
                    slot->instrument = READ_U8(offset + j + 1);
                    slot->volume_column = READ_U8(offset + j + 2);
                    slot->effect_type = READ_U8(offset + j + 3);
                    slot->effect_param = READ_U8(offset + j + 4);
                    j += 5;
                }
            }
        }

        offset += packed_patterndata_size;
    }

    for(uint16_t i = 0; i < ctx->module.num_instruments; ++i) {
        uint32_t sample_header_size = 0;
        xm_instrument_t* instr = mod->instruments + i;

        instr->num_samples = READ_U16(offset + 27);

        if(instr->num_samples > 0) {
            sample_header_size = READ_U32(offset + 29);
            READ_MEMCPY(instr->sample_of_notes, offset + 33, NUM_NOTES);

            instr->volume_envelope.num_points = READ_U8(offset + 225);
            instr->panning_envelope.num_points = READ_U8(offset + 226);

            for(uint8_t j = 0; j < instr->volume_envelope.num_points; ++j) {
                instr->volume_envelope.points[j].frame = READ_U16(offset + 129 + 4 * j);
                instr->volume_envelope.points[j].value = READ_U16(offset + 129 + 4 * j + 2);
            }

            for(uint8_t j = 0; j < instr->panning_envelope.num_points; ++j) {
                instr->panning_envelope.points[j].frame = READ_U16(offset + 177 + 4 * j);
                instr->panning_envelope.points[j].value = READ_U16(offset + 177 + 4 * j + 2);
            }

            instr->volume_envelope.sustain_point = READ_U8(offset + 227);
            instr->volume_envelope.loop_start_point = READ_U8(offset + 228);
            instr->volume_envelope.loop_end_point = READ_U8(offset + 229);

            instr->panning_envelope.sustain_point = READ_U8(offset + 230);
            instr->panning_envelope.loop_start_point = READ_U8(offset + 231);
            instr->panning_envelope.loop_end_point = READ_U8(offset + 232);

            uint8_t flags = READ_U8(offset + 233);
            instr->volume_envelope.enabled = flags & (1 << 0);
            instr->volume_envelope.sustain_enabled = flags & (1 << 1);
            instr->volume_envelope.loop_enabled = flags & (1 << 2);

            flags = READ_U8(offset + 234);
            instr->panning_envelope.enabled = flags & (1 << 0);
            instr->panning_envelope.sustain_enabled = flags & (1 << 1);
            instr->panning_envelope.loop_enabled = flags & (1 << 2);

            instr->vibrato_type = READ_U8(offset + 235);
            if(instr->vibrato_type == 2) {
                instr->vibrato_type = 1;
            } else if(instr->vibrato_type == 1) {
                instr->vibrato_type = 2;
            }
            instr->vibrato_sweep = READ_U8(offset + 236);
            instr->vibrato_depth = READ_U8(offset + 237);
            instr->vibrato_rate = READ_U8(offset + 238);
            instr->volume_fadeout = READ_U16(offset + 239);

            instr->samples = (xm_sample_t*)mempool;
            mempool += instr->num_samples * sizeof(xm_sample_t);
        } else {
            instr->samples = NULL;
        }

        offset += READ_U32(offset);

        for(uint16_t j = 0; j < instr->num_samples; ++j) {
            xm_sample_t* sample = instr->samples + j;

            sample->length = READ_U32(offset);
            sample->loop_start = READ_U32(offset + 4);
            sample->loop_length = READ_U32(offset + 8);
            sample->loop_end = sample->loop_start + sample->loop_length;
            sample->volume = (float)READ_U8(offset + 12) / (float)0x40;
            sample->finetune = (int8_t)READ_U8(offset + 13);

            uint8_t flags = READ_U8(offset + 14);
            if((flags & 3) == 0) {
                sample->loop_type = XM_NO_LOOP;
            } else if((flags & 3) == 1) {
                sample->loop_type = XM_FORWARD_LOOP;
            } else {
                sample->loop_type = XM_PING_PONG_LOOP;
            }

            sample->bits = (flags & (1 << 4)) ? 16 : 8;

            sample->panning = (float)READ_U8(offset + 15) / (float)0xFF;
            sample->relative_note = (int8_t)READ_U8(offset + 16);
            sample->data8 = (int8_t*)mempool;
            mempool += sample->length;

            if(sample->bits == 16) {
                sample->loop_start >>= 1;
                sample->loop_length >>= 1;
                sample->loop_end >>= 1;
                sample->length >>= 1;
            }

            offset += sample_header_size;
        }

        for(uint16_t j = 0; j < instr->num_samples; ++j) {
            xm_sample_t* sample = instr->samples + j;
            uint32_t length = sample->length;

            if(sample->bits == 16) {
                int16_t v = 0;
                for(uint32_t k = 0; k < length; ++k) {
                    v = v + (int16_t)READ_U16(offset + (k << 1));
                    sample->data16[k] = v;
                }
                offset += sample->length << 1;
            } else {
                int8_t v = 0;
                for(uint32_t k = 0; k < length; ++k) {
                    v = v + (int8_t)READ_U8(offset + k);
                    sample->data8[k] = v;
                }
                offset += sample->length;
            }
        }
    }

    return mempool;
}

int xm_create_context_safe(xm_context_t** ctxp, const char* moddata, size_t moddata_length, uint32_t rate) {
    size_t bytes_needed;
    char* mempool;
    xm_context_t* ctx;

    bytes_needed = xm_get_memory_needed_for_context(moddata, moddata_length);
    mempool = malloc(bytes_needed);
    if(mempool == NULL && bytes_needed > 0) {
        return 2;
    }

    memset(mempool, 0, bytes_needed);

    ctx = (*ctxp = (xm_context_t*)mempool);
    ctx->ctx_size = bytes_needed;
    mempool += sizeof(xm_context_t);

    ctx->rate = rate;
    mempool = xm_load_module(ctx, moddata, moddata_length, mempool);

    ctx->channels = (xm_channel_context_t*)mempool;
    mempool += ctx->module.num_channels * sizeof(xm_channel_context_t);

    ctx->global_volume = 1.f;
    ctx->amplification = .25f;

    ctx->volume_ramp = (1.f / 128.f);
    ctx->panning_ramp = (1.f / 128.f);

    for(uint8_t i = 0; i < ctx->module.num_channels; ++i) {
        xm_channel_context_t* ch = ctx->channels + i;

        ch->ping = true;
        ch->vibrato_waveform = XM_SINE_WAVEFORM;
        ch->vibrato_waveform_retrigger = true;
        ch->tremolo_waveform = XM_SINE_WAVEFORM;
        ch->tremolo_waveform_retrigger = true;

        ch->volume = ch->volume_envelope_volume = ch->fadeout_volume = 1.0f;
        ch->panning = ch->panning_envelope_panning = .5f;
        ch->actual_volume = .0f;
        ch->actual_panning = .5f;
    }

    ctx->row_loop_count = (uint8_t*)mempool;
    mempool += MAX_NUM_ROWS * sizeof(uint8_t);

    return 0;
}

// ----- Playback -----

float xm_waveform(xm_waveform_type_t, uint8_t);
void xm_autovibrato(xm_context_t*, xm_channel_context_t*);
void xm_vibrato(xm_context_t*, xm_channel_context_t*, uint8_t, uint16_t);
void xm_tremolo(xm_context_t*, xm_channel_context_t*, uint8_t, uint16_t);
void xm_arpeggio(xm_context_t*, xm_channel_context_t*, uint8_t, uint16_t);
void xm_tone_portamento(xm_context_t*, xm_channel_context_t*);
void xm_pitch_slide(xm_context_t*, xm_channel_context_t*, float);
void xm_panning_slide(xm_channel_context_t*, uint8_t);
void xm_volume_slide(xm_channel_context_t*, uint8_t);

float xm_envelope_lerp(xm_envelope_point_t*, xm_envelope_point_t*, uint16_t);
void xm_envelope_tick(xm_channel_context_t*, xm_envelope_t*, uint16_t*, float*);
void xm_envelopes(xm_channel_context_t*);

float xm_linear_period(float);
float xm_linear_frequency(float);
float xm_amiga_period(float);
float xm_amiga_frequency(float);
float xm_period(xm_context_t*, float);
float xm_frequency(xm_context_t*, float, float);
void xm_update_frequency(xm_context_t*, xm_channel_context_t*);

void xm_handle_note_and_instrument(xm_context_t*, xm_channel_context_t*, xm_pattern_slot_t*);
void xm_trigger_note(xm_context_t*, xm_channel_context_t*, unsigned int flags);
void xm_cut_note(xm_channel_context_t*);
void xm_key_off(xm_channel_context_t*);

void xm_post_pattern_change(xm_context_t*);
void xm_row(xm_context_t*);
void xm_tick(xm_context_t*);

float xm_sample_at(xm_sample_t*, size_t);
float xm_next_of_sample(xm_channel_context_t*);
void xm_sample(xm_context_t*, float*, float*);

#define XM_TRIGGER_KEEP_VOLUME (1 << 0)
#define XM_TRIGGER_KEEP_PERIOD (1 << 1)
#define XM_TRIGGER_KEEP_SAMPLE_POSITION (1 << 2)

const uint16_t amiga_frequencies[] = {
    1712, 1616, 1525, 1440,
    1357, 1281, 1209, 1141,
    1077, 1017,  961,  907,
    856
};

const float multi_retrig_add[] = {
    0.f,  -1.f,  -2.f,  -4.f,
    -8.f, -16.f,   0.f,   0.f,
    0.f,   1.f,   2.f,   4.f,
    8.f,  16.f,   0.f,   0.f
};

const float multi_retrig_multiply[] = {
    1.f,   1.f,  1.f,        1.f,
    1.f,   1.f,   .6666667f,  .5f,
    1.f,   1.f,  1.f,        1.f,
    1.f,   1.f,  1.5f,       2.f
};

#define XM_CLAMP_UP1F(vol, limit) do{if((vol) > (limit)) (vol) = (limit);}while(0)
#define XM_CLAMP_UP(vol) XM_CLAMP_UP1F((vol), 1.f)
#define XM_CLAMP_DOWN1F(vol, limit) do{if((vol) < (limit)) (vol) = (limit);}while(0)
#define XM_CLAMP_DOWN(vol) XM_CLAMP_DOWN1F((vol), .0f)
#define XM_CLAMP2F(vol, up, down) do{if((vol) > (up)) (vol) = (up);else if((vol) < (down)) (vol) = (down);}while(0)
#define XM_CLAMP(vol) XM_CLAMP2F((vol), 1.f, .0f)
#define XM_SLIDE_TOWARDS(val, goal, incr) do{if((val) > (goal)) {(val) -= (incr);XM_CLAMP_DOWN1F((val), (goal));} else if((val) < (goal)) {(val) += (incr);XM_CLAMP_UP1F((val), (goal));}}while(0)
#define XM_LERP(u, v, t) ((u) + (t) * ((v) - (u)))
#define XM_INVERSE_LERP(u, v, lerp) (((lerp) - (u)) / ((v) - (u)))
#define HAS_TONE_PORTAMENTO(s) ((s)->effect_type == 3 || (s)->effect_type == 5 || ((s)->volume_column >> 4) == 0xF)
#define HAS_ARPEGGIO(s) ((s)->effect_type == 0 && (s)->effect_param != 0)
#define HAS_VIBRATO(s) ((s)->effect_type == 4 || (s)->effect_param == 6 || ((s)->volume_column >> 4) == 0xB)
#define NOTE_IS_VALID(n) ((n) > 0 && (n) < 97)

float xm_waveform(xm_waveform_type_t waveform, uint8_t step) {
    unsigned int next_rand = 24492;
    step %= 0x40;

    switch(waveform) {

        case XM_SINE_WAVEFORM:
            return -sinf(2.f * 3.141592f * (float)step / (float)0x40);

        case XM_RAMP_DOWN_WAVEFORM:
            return (float)(0x20 - step) / 0x20;

        case XM_SQUARE_WAVEFORM:
            return (step >= 0x20) ? 1.f : -1.f;

        case XM_RANDOM_WAVEFORM:
            next_rand = next_rand * 1103515245 + 12345;
            return (float)((next_rand >> 16) & 0x7FFF) / (float)0x4000 - 1.f;

        case XM_RAMP_UP_WAVEFORM:
            return (float)(step - 0x20) / 0x20;

        default:
            break;

    }

    return .0f;
}

void xm_autovibrato(xm_context_t* ctx, xm_channel_context_t* ch) {
    if(ch->instrument == NULL || ch->instrument->vibrato_depth == 0) return;
    xm_instrument_t* instr = ch->instrument;
    float sweep = 1.f;

    if(ch->autovibrato_ticks < instr->vibrato_sweep) {
        sweep = XM_LERP(0.f, 1.f, (float)ch->autovibrato_ticks / (float)instr->vibrato_sweep);
    }

    unsigned int step = ((ch->autovibrato_ticks++) * instr->vibrato_rate) >> 2;
    ch->autovibrato_note_offset = .25f * xm_waveform(instr->vibrato_type, step)
        * (float)instr->vibrato_depth / (float)0xF * sweep;
    xm_update_frequency(ctx, ch);
}

void xm_vibrato(xm_context_t* ctx, xm_channel_context_t* ch, uint8_t param, uint16_t pos) {
    unsigned int step = pos * (param >> 4);
    ch->vibrato_note_offset =
        2.f
        * xm_waveform(ch->vibrato_waveform, step)
        * (float)(param & 0x0F) / (float)0xF;
    xm_update_frequency(ctx, ch);
}

void xm_tremolo(xm_context_t* ctx, xm_channel_context_t* ch, uint8_t param, uint16_t pos) {
    unsigned int step = pos * (param >> 4);
    ch->tremolo_volume = -1.f * xm_waveform(ch->tremolo_waveform, step)
        * (float)(param & 0x0F) / (float)0xF;
}

void xm_arpeggio(xm_context_t* ctx, xm_channel_context_t* ch, uint8_t param, uint16_t tick) {
    switch(tick % 3) {
        case 0:
            ch->arp_in_progress = false;
            ch->arp_note_offset = 0;
            break;
        case 2:
            ch->arp_in_progress = true;
            ch->arp_note_offset = param >> 4;
            break;
        case 1:
            ch->arp_in_progress = true;
            ch->arp_note_offset = param & 0x0F;
            break;
    }

    xm_update_frequency(ctx, ch);
}

void xm_tone_portamento(xm_context_t* ctx, xm_channel_context_t* ch) {
    if(ch->tone_portamento_target_period == 0.f) return;

    if(ch->period != ch->tone_portamento_target_period) {
        XM_SLIDE_TOWARDS(ch->period,
                ch->tone_portamento_target_period,
                (ctx->module.frequency_type == XM_LINEAR_FREQUENCIES ?
                 4.f : 1.f) * ch->tone_portamento_param
                );
        xm_update_frequency(ctx, ch);
    }
}

void xm_pitch_slide(xm_context_t* ctx, xm_channel_context_t* ch, float period_offset) {
    if(ctx->module.frequency_type == XM_LINEAR_FREQUENCIES) {
        period_offset *= 4.f;
    }

    ch->period += period_offset;
    XM_CLAMP_DOWN(ch->period);

    xm_update_frequency(ctx, ch);
}

void xm_panning_slide(xm_channel_context_t* ch, uint8_t rawval) {
    float f;

    if((rawval & 0xF0) && (rawval & 0x0F)) {
        return;
    }

    if(rawval & 0xF0) {
        f = (float)(rawval >> 4) / (float)0xFF;
        ch->panning += f;
        XM_CLAMP_UP(ch->panning);
    } else {
        f = (float)(rawval & 0x0F) / (float)0xFF;
        ch->panning -= f;
        XM_CLAMP_DOWN(ch->panning);
    }
}

void xm_volume_slide(xm_channel_context_t* ch, uint8_t rawval) {
    float f;

    if((rawval & 0xF0) && (rawval & 0x0F)) {
        return;
    }

    if(rawval & 0xF0) {
        f = (float)(rawval >> 4) / (float)0x40;
        ch->volume += f;
        XM_CLAMP_UP(ch->volume);
    } else {
        f = (float)(rawval & 0x0F) / (float)0x40;
        ch->volume -= f;
        XM_CLAMP_DOWN(ch->volume);
    }
}

float xm_envelope_lerp(xm_envelope_point_t* restrict a, xm_envelope_point_t* restrict b, uint16_t pos) {
    if(pos <= a->frame) return a->value;
    else if(pos >= b->frame) return b->value;
    else {
        float p = (float)(pos - a->frame) / (float)(b->frame - a->frame);
        return a->value * (1 - p) + b->value * p;
    }
}

void xm_post_pattern_change(xm_context_t* ctx) {
    if(ctx->current_table_index >= ctx->module.length) {
        ctx->current_table_index = ctx->module.restart_position;
    }
}

float xm_linear_period(float note) {
    return 7680.f - note * 64.f;
}

float xm_linear_frequency(float period) {
    return 8363.f * powf(2.f, (4608.f - period) / 768.f);
}

float xm_amiga_period(float note) {
    unsigned int intnote = note;
    uint8_t a = intnote % 12;
    int8_t octave = note / 12.f - 2;
    uint16_t p1 = amiga_frequencies[a], p2 = amiga_frequencies[a + 1];

    if(octave > 0) {
        p1 >>= octave;
        p2 >>= octave;
    } else if(octave < 0) {
        p1 <<= (-octave);
        p2 <<= (-octave);
    }

    return XM_LERP(p1, p2, note - intnote);
}

float xm_amiga_frequency(float period) {
    if(period == .0f) return .0f;
    return 7093789.2f / (period * 2.f);
}

float xm_period(xm_context_t* ctx, float note) {
    switch(ctx->module.frequency_type) {
        case XM_LINEAR_FREQUENCIES:
            return xm_linear_period(note);
        case XM_AMIGA_FREQUENCIES:
            return xm_amiga_period(note);
    }
    return .0f;
}

float xm_frequency(xm_context_t* ctx, float period, float note_offset) {
    uint8_t a;
    int8_t octave;
    float note;
    uint16_t p1, p2;

    switch(ctx->module.frequency_type) {

        case XM_LINEAR_FREQUENCIES:
            return xm_linear_frequency(period - 64.f * note_offset);

        case XM_AMIGA_FREQUENCIES:
            if(note_offset == 0) {
                return xm_amiga_frequency(period);
            }

            a = octave = 0;

            if(period > amiga_frequencies[0]) {
                --octave;
                while(period > (amiga_frequencies[0] << (-octave))) --octave;
            } else if(period < amiga_frequencies[12]) {
                ++octave;
                while(period < (amiga_frequencies[12] >> octave)) ++octave;
            }

            for(uint8_t i = 0; i < 12; ++i) {
                p1 = amiga_frequencies[i], p2 = amiga_frequencies[i + 1];

                if(octave > 0) {
                    p1 >>= octave;
                    p2 >>= octave;
                } else if(octave < 0) {
                    p1 <<= (-octave);
                    p2 <<= (-octave);
                }

                if(p2 <= period && period <= p1) {
                    a = i;
                    break;
                }
            }

            note = 12.f * (octave + 2) + a + XM_INVERSE_LERP(p1, p2, period);

            return xm_amiga_frequency(xm_amiga_period(note + note_offset));

    }

    return .0f;
}

void xm_update_frequency(xm_context_t* ctx, xm_channel_context_t* ch) {
    ch->frequency = xm_frequency(ctx, ch->period, (ch->arp_note_offset > 0 ? ch->arp_note_offset : (ch->vibrato_note_offset + ch->autovibrato_note_offset)));
    ch->step = ch->frequency / ctx->rate;
}

void xm_handle_note_and_instrument(xm_context_t* ctx, xm_channel_context_t* ch,
        xm_pattern_slot_t* s) {
    if(s->instrument > 0) {
        if(HAS_TONE_PORTAMENTO(ch->current) && ch->instrument != NULL && ch->sample != NULL) {
            xm_trigger_note(ctx, ch, XM_TRIGGER_KEEP_PERIOD | XM_TRIGGER_KEEP_SAMPLE_POSITION);
        } else if(s->instrument > ctx->module.num_instruments) {
            xm_cut_note(ch);
            ch->instrument = NULL;
            ch->sample = NULL;
        } else {
            ch->instrument = ctx->module.instruments + (s->instrument - 1);
            if(s->note == 0 && ch->sample != NULL) {
                xm_trigger_note(ctx, ch, XM_TRIGGER_KEEP_SAMPLE_POSITION);
            }
        }
    }

    if(NOTE_IS_VALID(s->note)) {
        xm_instrument_t* instr = ch->instrument;

        if(HAS_TONE_PORTAMENTO(ch->current) && instr != NULL && ch->sample != NULL) {
            ch->note = s->note + ch->sample->relative_note + ch->sample->finetune / 128.f - 1.f;
            ch->tone_portamento_target_period = xm_period(ctx, ch->note);
        } else if(instr == NULL || ch->instrument->num_samples == 0) {
            xm_cut_note(ch);
        } else {
            if(instr->sample_of_notes[s->note - 1] < instr->num_samples) {
                for(unsigned int z = 0; z < XM_SAMPLE_RAMPING_POINTS; ++z) {
                    ch->end_of_previous_sample[z] = xm_next_of_sample(ch);
                }
                ch->frame_count = 0;
                ch->sample = instr->samples + instr->sample_of_notes[s->note - 1];
                ch->orig_note = ch->note = s->note + ch->sample->relative_note
                    + ch->sample->finetune / 128.f - 1.f;
                if(s->instrument > 0) {
                    xm_trigger_note(ctx, ch, 0);
                } else {
                    xm_trigger_note(ctx, ch, XM_TRIGGER_KEEP_VOLUME);
                }
            } else {
                xm_cut_note(ch);
            }
        }
    } else if(s->note == 97) {
        xm_key_off(ch);
    }

    switch(s->volume_column >> 4) {

        case 0x5:
            if(s->volume_column > 0x50) break;
        case 0x1:
        case 0x2:
        case 0x3:
        case 0x4:
            ch->volume = (float)(s->volume_column - 0x10) / (float)0x40;
            break;

        case 0x8:
            xm_volume_slide(ch, s->volume_column & 0x0F);
            break;

        case 0x9:
            xm_volume_slide(ch, s->volume_column << 4);
            break;

        case 0xA:
            ch->vibrato_param = (ch->vibrato_param & 0x0F) | ((s->volume_column & 0x0F) << 4);
            break;

        case 0xC:
            ch->panning = (float)(
                    ((s->volume_column & 0x0F) << 4) | (s->volume_column & 0x0F)
                    ) / (float)0xFF;
            break;

        case 0xF:
            if(s->volume_column & 0x0F) {
                ch->tone_portamento_param = ((s->volume_column & 0x0F) << 4)
                    | (s->volume_column & 0x0F);
            }
            break;

        default:
            break;

    }

    switch(s->effect_type) {

        case 1:
            if(s->effect_param > 0) {
                ch->portamento_up_param = s->effect_param;
            }
            break;

        case 2:
            if(s->effect_param > 0) {
                ch->portamento_down_param = s->effect_param;
            }
            break;

        case 3:
            if(s->effect_param > 0) {
                ch->tone_portamento_param = s->effect_param;
            }
            break;

        case 4:
            if(s->effect_param & 0x0F) {
                ch->vibrato_param = (ch->vibrato_param & 0xF0) | (s->effect_param & 0x0F);
            }
            if(s->effect_param >> 4) {
                ch->vibrato_param = (s->effect_param & 0xF0) | (ch->vibrato_param & 0x0F);
            }
            break;

        case 5:
            if(s->effect_param > 0) {
                ch->volume_slide_param = s->effect_param;
            }
            break;

        case 6:
            if(s->effect_param > 0) {
                ch->volume_slide_param = s->effect_param;
            }
            break;

        case 7:
            if(s->effect_param & 0x0F) {
                ch->tremolo_param = (ch->tremolo_param & 0xF0) | (s->effect_param & 0x0F);
            }
            if(s->effect_param >> 4) {
                ch->tremolo_param = (s->effect_param & 0xF0) | (ch->tremolo_param & 0x0F);
            }
            break;

        case 8:
            ch->panning = (float)s->effect_param / (float)0xFF;
            break;

        case 9:
            if(ch->sample != NULL && NOTE_IS_VALID(s->note)) {
                uint32_t final_offset = s->effect_param << (ch->sample->bits == 16 ? 7 : 8);
                if(final_offset >= ch->sample->length) {
                    ch->sample_position = -1;
                    break;
                }
                ch->sample_position = final_offset;
            }
            break;

        case 0xA:
            if(s->effect_param > 0) {
                ch->volume_slide_param = s->effect_param;
            }
            break;

        case 0xB:
            if(s->effect_param < ctx->module.length) {
                ctx->position_jump = true;
                ctx->jump_dest = s->effect_param;
            }
            break;

        case 0xC:
            ch->volume = (float)((s->effect_param > 0x40)
                    ? 0x40 : s->effect_param) / (float)0x40;
            break;

        case 0xD:
            ctx->pattern_break = true;
            ctx->jump_row = (s->effect_param >> 4) * 10 + (s->effect_param & 0x0F);
            break;

        case 0xE:
            switch(s->effect_param >> 4) {

                case 1:
                    if(s->effect_param & 0x0F) {
                        ch->fine_portamento_up_param = s->effect_param & 0x0F;
                    }
                    xm_pitch_slide(ctx, ch, -ch->fine_portamento_up_param);
                    break;

                case 2:
                    if(s->effect_param & 0x0F) {
                        ch->fine_portamento_down_param = s->effect_param & 0x0F;
                    }
                    xm_pitch_slide(ctx, ch, ch->fine_portamento_down_param);
                    break;

                case 4:
                    ch->vibrato_waveform = s->effect_param & 3;
                    ch->vibrato_waveform_retrigger = !((s->effect_param >> 2) & 1);
                    break;

                case 5:
                    if(NOTE_IS_VALID(ch->current->note) && ch->sample != NULL) {
                        ch->note = ch->current->note + ch->sample->relative_note +
                            (float)(((s->effect_param & 0x0F) - 8) << 4) / 128.f - 1.f;
                        ch->period = xm_period(ctx, ch->note);
                        xm_update_frequency(ctx, ch);
                    }
                    break;

                case 6:
                    if(s->effect_param & 0x0F) {
                        if((s->effect_param & 0x0F) == ch->pattern_loop_count) {
                            ch->pattern_loop_count = 0;
                            break;
                        }

                        ch->pattern_loop_count++;
                        ctx->position_jump = true;
                        ctx->jump_row = ch->pattern_loop_origin;
                        ctx->jump_dest = ctx->current_table_index;
                    } else {
                        ch->pattern_loop_origin = ctx->current_row;
                        ctx->jump_row = ch->pattern_loop_origin;
                    }
                    break;

                case 7:
                    ch->tremolo_waveform = s->effect_param & 3;
                    ch->tremolo_waveform_retrigger = !((s->effect_param >> 2) & 1);
                    break;

                case 0xA:
                    if(s->effect_param & 0x0F) {
                        ch->fine_volume_slide_param = s->effect_param & 0x0F;
                    }
                    xm_volume_slide(ch, ch->fine_volume_slide_param << 4);
                    break;

                case 0xB:
                    if(s->effect_param & 0x0F) {
                        ch->fine_volume_slide_param = s->effect_param & 0x0F;
                    }
                    xm_volume_slide(ch, ch->fine_volume_slide_param);
                    break;

                case 0xD:
                    if(s->note == 0 && s->instrument == 0) {
                        unsigned int flags = XM_TRIGGER_KEEP_VOLUME;

                        if(ch->current->effect_param & 0x0F) {
                            ch->note = ch->orig_note;
                            xm_trigger_note(ctx, ch, flags);
                        } else {
                            xm_trigger_note(
                                    ctx, ch,
                                    flags
                                    | XM_TRIGGER_KEEP_PERIOD
                                    | XM_TRIGGER_KEEP_SAMPLE_POSITION
                                    );
                        }
                    }
                    break;

                case 0xE:
                    ctx->extra_ticks = (ch->current->effect_param & 0x0F) * ctx->tempo;
                    break;

                default:
                    break;

            }
            break;

        case 0xF:
            if(s->effect_param > 0) {
                if(s->effect_param <= 0x1F) {
                    ctx->tempo = s->effect_param;
                } else {
                    ctx->bpm = s->effect_param;
                }
            }
            break;

        case 16:
            ctx->global_volume = (float)((s->effect_param > 0x40)
                    ? 0x40 : s->effect_param) / (float)0x40;
            break;

        case 17:
            if(s->effect_param > 0) {
                ch->global_volume_slide_param = s->effect_param;
            }
            break;

        case 21:
            ch->volume_envelope_frame_count = s->effect_param;
            ch->panning_envelope_frame_count = s->effect_param;
            break;

        case 25:
            if(s->effect_param > 0) {
                ch->panning_slide_param = s->effect_param;
            }
            break;

        case 27:
            if(s->effect_param > 0) {
                if((s->effect_param >> 4) == 0) {
                    ch->multi_retrig_param = (ch->multi_retrig_param & 0xF0) | (s->effect_param & 0x0F);
                } else {
                    ch->multi_retrig_param = s->effect_param;
                }
            }
            break;

        case 29:
            if(s->effect_param > 0) {
                ch->tremor_param = s->effect_param;
            }
            break;

        case 33:
            switch(s->effect_param >> 4) {

                case 1:
                    if(s->effect_param & 0x0F) {
                        ch->extra_fine_portamento_up_param = s->effect_param & 0x0F;
                    }
                    xm_pitch_slide(ctx, ch, -1.0f * ch->extra_fine_portamento_up_param);
                    break;

                case 2:
                    if(s->effect_param & 0x0F) {
                        ch->extra_fine_portamento_down_param = s->effect_param & 0x0F;
                    }
                    xm_pitch_slide(ctx, ch, ch->extra_fine_portamento_down_param);
                    break;

                default:
                    break;

            }
            break;

        default:
            break;

    }
}

void xm_trigger_note(xm_context_t* ctx, xm_channel_context_t* ch, unsigned int flags) {
    if(!(flags & XM_TRIGGER_KEEP_SAMPLE_POSITION)) {
        ch->sample_position = 0.f;
        ch->ping = true;
    }

    if(ch->sample != NULL) {
        if(!(flags & XM_TRIGGER_KEEP_VOLUME)) {
            ch->volume = ch->sample->volume;
        }

        ch->panning = ch->sample->panning;
    }

    ch->sustained = true;
    ch->fadeout_volume = ch->volume_envelope_volume = 1.0f;
    ch->panning_envelope_panning = .5f;
    ch->volume_envelope_frame_count = ch->panning_envelope_frame_count = 0;
    ch->vibrato_note_offset = 0.f;
    ch->tremolo_volume = 0.f;
    ch->tremor_on = false;

    ch->autovibrato_ticks = 0;

    if(ch->vibrato_waveform_retrigger) {
        ch->vibrato_ticks = 0;
    }
    if(ch->tremolo_waveform_retrigger) {
        ch->tremolo_ticks = 0;
    }

    if(!(flags & XM_TRIGGER_KEEP_PERIOD)) {
        ch->period = xm_period(ctx, ch->note);
        xm_update_frequency(ctx, ch);
    }

    ch->latest_trigger = ctx->generated_samples;
    if(ch->instrument != NULL) {
        ch->instrument->latest_trigger = ctx->generated_samples;
    }
    if(ch->sample != NULL) {
        ch->sample->latest_trigger = ctx->generated_samples;
    }
}

void xm_cut_note(xm_channel_context_t* ch) {
    ch->volume = .0f;
}

void xm_key_off(xm_channel_context_t* ch) {
    ch->sustained = false;

    if(ch->instrument == NULL || !ch->instrument->volume_envelope.enabled) {
        xm_cut_note(ch);
    }
}

void xm_row(xm_context_t* ctx) {
    if(ctx->position_jump) {
        ctx->current_table_index = ctx->jump_dest;
        ctx->current_row = ctx->jump_row;
        ctx->position_jump = false;
        ctx->pattern_break = false;
        ctx->jump_row = 0;
        xm_post_pattern_change(ctx);
    } else if(ctx->pattern_break) {
        ctx->current_table_index++;
        ctx->current_row = ctx->jump_row;
        ctx->pattern_break = false;
        ctx->jump_row = 0;
        xm_post_pattern_change(ctx);
    }

    xm_pattern_t* cur = ctx->module.patterns + ctx->module.pattern_table[ctx->current_table_index];
    bool in_a_loop = false;

    for(uint8_t i = 0; i < ctx->module.num_channels; ++i) {
        xm_pattern_slot_t* s = cur->slots + ctx->current_row * ctx->module.num_channels + i;
        xm_channel_context_t* ch = ctx->channels + i;

        ch->current = s;

        if(s->effect_type != 0xE || s->effect_param >> 4 != 0xD) {
            xm_handle_note_and_instrument(ctx, ch, s);
        } else {
            ch->note_delay_param = s->effect_param & 0x0F;
        }

        if(!in_a_loop && ch->pattern_loop_count > 0) {
            in_a_loop = true;
        }
    }

    if(!in_a_loop) {
        ctx->loop_count = (ctx->row_loop_count[MAX_NUM_ROWS * ctx->current_table_index + ctx->current_row]++);
    }

    ctx->current_row++;
    if(!ctx->position_jump && !ctx->pattern_break &&
            (ctx->current_row >= cur->num_rows || ctx->current_row == 0)) {
        ctx->current_table_index++;
        ctx->current_row = ctx->jump_row;
        ctx->jump_row = 0;
        xm_post_pattern_change(ctx);
    }
}

void xm_envelope_tick(xm_channel_context_t* ch,
        xm_envelope_t* env,
        uint16_t* counter,
        float* outval) {
    if(env->num_points < 2) {
        if(env->num_points == 1) {
            *outval = (float)env->points[0].value / (float)0x40;
            if(*outval > 1) {
                *outval = 1;
            }
        }

        return;
    } else {
        uint8_t j;

        if(env->loop_enabled) {
            uint16_t loop_start = env->points[env->loop_start_point].frame;
            uint16_t loop_end = env->points[env->loop_end_point].frame;
            uint16_t loop_length = loop_end - loop_start;

            if(*counter >= loop_end) {
                *counter -= loop_length;
            }
        }

        for(j = 0; j < (env->num_points - 2); ++j) {
            if(env->points[j].frame <= *counter &&
                    env->points[j+1].frame >= *counter) {
                break;
            }
        }

        *outval = xm_envelope_lerp(env->points + j, env->points + j + 1, *counter) / (float)0x40;

        if(!ch->sustained || !env->sustain_enabled ||
                *counter != env->points[env->sustain_point].frame) {
            (*counter)++;
        }
    }
}

void xm_envelopes(xm_channel_context_t* ch) {
    if(ch->instrument != NULL) {
        if(ch->instrument->volume_envelope.enabled) {
            if(!ch->sustained) {
                ch->fadeout_volume -= (float)ch->instrument->volume_fadeout / 65536.f;
                XM_CLAMP_DOWN(ch->fadeout_volume);
            }

            xm_envelope_tick(ch,
                    &(ch->instrument->volume_envelope),
                    &(ch->volume_envelope_frame_count),
                    &(ch->volume_envelope_volume));
        }

        if(ch->instrument->panning_envelope.enabled) {
            xm_envelope_tick(ch,
                    &(ch->instrument->panning_envelope),
                    &(ch->panning_envelope_frame_count),
                    &(ch->panning_envelope_panning));
        }
    }
}

void xm_tick(xm_context_t* ctx) {
    if(ctx->current_tick == 0) {
        xm_row(ctx);
    }

    for(uint8_t i = 0; i < ctx->module.num_channels; ++i) {
        xm_channel_context_t* ch = ctx->channels + i;

        xm_envelopes(ch);
        xm_autovibrato(ctx, ch);

        if(ch->arp_in_progress && !HAS_ARPEGGIO(ch->current)) {
            ch->arp_in_progress = false;
            ch->arp_note_offset = 0;
            xm_update_frequency(ctx, ch);
        }
        if(ch->vibrato_in_progress && !HAS_VIBRATO(ch->current)) {
            ch->vibrato_in_progress = false;
            ch->vibrato_note_offset = 0.f;
            xm_update_frequency(ctx, ch);
        }

        switch(ch->current->volume_column >> 4) {

            case 0x6:
                if(ctx->current_tick == 0) break;
                xm_volume_slide(ch, ch->current->volume_column & 0x0F);
                break;

            case 0x7:
                if(ctx->current_tick == 0) break;
                xm_volume_slide(ch, ch->current->volume_column << 4);
                break;

            case 0xB:
                if(ctx->current_tick == 0) break;
                ch->vibrato_in_progress = false;
                xm_vibrato(ctx, ch, ch->vibrato_param, ch->vibrato_ticks++);
                break;

            case 0xD:
                if(ctx->current_tick == 0) break;
                xm_panning_slide(ch, ch->current->volume_column & 0x0F);
                break;

            case 0xE:
                if(ctx->current_tick == 0) break;
                xm_panning_slide(ch, ch->current->volume_column << 4);
                break;

            case 0xF:
                if(ctx->current_tick == 0) break;
                xm_tone_portamento(ctx, ch);
                break;

            default:
                break;

        }

        switch(ch->current->effect_type) {

            case 0:
                if(ch->current->effect_param > 0) {
                    char arp_offset = ctx->tempo % 3;
                    switch(arp_offset) {
                        case 2:
                            if(ctx->current_tick == 1) {
                                ch->arp_in_progress = true;
                                ch->arp_note_offset = ch->current->effect_param >> 4;
                                xm_update_frequency(ctx, ch);
                                break;
                            }
                        case 1:
                            if(ctx->current_tick == 0) {
                                ch->arp_in_progress = false;
                                ch->arp_note_offset = 0;
                                xm_update_frequency(ctx, ch);
                                break;
                            }
                        case 0:
                            xm_arpeggio(ctx, ch, ch->current->effect_param, ctx->current_tick - arp_offset);
                        default:
                            break;
                    }
                }
                break;

            case 1:
                if(ctx->current_tick == 0) break;
                xm_pitch_slide(ctx, ch, -ch->portamento_up_param);
                break;

            case 2:
                if(ctx->current_tick == 0) break;
                xm_pitch_slide(ctx, ch, ch->portamento_down_param);
                break;

            case 3:
                if(ctx->current_tick == 0) break;
                xm_tone_portamento(ctx, ch);
                break;

            case 4:
                if(ctx->current_tick == 0) break;
                ch->vibrato_in_progress = true;
                xm_vibrato(ctx, ch, ch->vibrato_param, ch->vibrato_ticks++);
                break;

            case 5:
                if(ctx->current_tick == 0) break;
                xm_tone_portamento(ctx, ch);
                xm_volume_slide(ch, ch->volume_slide_param);
                break;

            case 6:
                if(ctx->current_tick == 0) break;
                ch->vibrato_in_progress = true;
                xm_vibrato(ctx, ch, ch->vibrato_param, ch->vibrato_ticks++);
                xm_volume_slide(ch, ch->volume_slide_param);
                break;

            case 7:
                if(ctx->current_tick == 0) break;
                xm_tremolo(ctx, ch, ch->tremolo_param, ch->tremolo_ticks++);
                break;

            case 0xA:
                if(ctx->current_tick == 0) break;
                xm_volume_slide(ch, ch->volume_slide_param);
                break;

            case 0xE:
                switch(ch->current->effect_param >> 4) {

                    case 0x9:
                        if(ctx->current_tick != 0 && ch->current->effect_param & 0x0F) {
                            if(!(ctx->current_tick % (ch->current->effect_param & 0x0F))) {
                                xm_trigger_note(ctx, ch, 0);
                                xm_envelopes(ch);
                            }
                        }
                        break;

                    case 0xC:
                        if((ch->current->effect_param & 0x0F) == ctx->current_tick) {
                            xm_cut_note(ch);
                        }
                        break;

                    case 0xD:
                        if(ch->note_delay_param == ctx->current_tick) {
                            xm_handle_note_and_instrument(ctx, ch, ch->current);
                            xm_envelopes(ch);
                        }
                        break;

                    default:
                        break;

                }
                break;

            case 17:
                if(ctx->current_tick == 0) break;
                if((ch->global_volume_slide_param & 0xF0) &&
                        (ch->global_volume_slide_param & 0x0F)) {
                    break;
                }
                if(ch->global_volume_slide_param & 0xF0) {
                    float f = (float)(ch->global_volume_slide_param >> 4) / (float)0x40;
                    ctx->global_volume += f;
                    XM_CLAMP_UP(ctx->global_volume);
                } else {
                    float f = (float)(ch->global_volume_slide_param & 0x0F) / (float)0x40;
                    ctx->global_volume -= f;
                    XM_CLAMP_DOWN(ctx->global_volume);
                }
                break;

            case 20:
                if(ctx->current_tick == ch->current->effect_param) {
                    xm_key_off(ch);
                }
                break;

            case 25:
                if(ctx->current_tick == 0) break;
                xm_panning_slide(ch, ch->panning_slide_param);
                break;

            case 27:
                if(ctx->current_tick == 0) break;
                if(((ch->multi_retrig_param) & 0x0F) == 0) break;
                if((ctx->current_tick % (ch->multi_retrig_param & 0x0F)) == 0) {
                    float v = ch->volume * multi_retrig_multiply[ch->multi_retrig_param >> 4]
                        + multi_retrig_add[ch->multi_retrig_param >> 4];
                    XM_CLAMP(v);
                    xm_trigger_note(ctx, ch, 0);
                    ch->volume = v;
                }
                break;

            case 29:
                if(ctx->current_tick == 0) break;
                ch->tremor_on = (
                        (ctx->current_tick - 1) % ((ch->tremor_param >> 4) + (ch->tremor_param & 0x0F) + 2)
                        >
                        (ch->tremor_param >> 4)
                        );
                break;

            default:
                break;

        }

        float panning, volume;

        panning = ch->panning +
            (ch->panning_envelope_panning - .5f) * (.5f - fabsf(ch->panning - .5f)) * 2.0f;

        if(ch->tremor_on) {
            volume = .0f;
        } else {
            volume = ch->volume + ch->tremolo_volume;
            XM_CLAMP(volume);
            volume *= ch->fadeout_volume * ch->volume_envelope_volume;
        }

        ch->target_panning = panning;
        ch->target_volume = volume;
    }

    ctx->current_tick++;
    if(ctx->current_tick >= ctx->tempo + ctx->extra_ticks) {
        ctx->current_tick = 0;
        ctx->extra_ticks = 0;
    }

    ctx->remaining_samples_in_tick += (float)ctx->rate / ((float)ctx->bpm * 0.4f);
}

float xm_sample_at(xm_sample_t* sample, size_t k) {
    return sample->bits == 8 ? (sample->data8[k] / 128.f) : (sample->data16[k] / 32768.f);
}

float xm_next_of_sample(xm_channel_context_t* ch) {
    if(ch->instrument == NULL || ch->sample == NULL || ch->sample_position < 0) {
        if(ch->frame_count < XM_SAMPLE_RAMPING_POINTS) {
            return XM_LERP(ch->end_of_previous_sample[ch->frame_count], .0f,
                    (float)ch->frame_count / (float)XM_SAMPLE_RAMPING_POINTS);
        }
        return .0f;
    }
    if(ch->sample->length == 0) {
        return .0f;
    }

    float u, v, t;
    uint32_t a, b;
    a = (uint32_t)ch->sample_position;
    if(XM_LINEAR_INTERPOLATION) {
        b = a + 1;
        t = ch->sample_position - a;
    }
    u = xm_sample_at(ch->sample, a);

    switch(ch->sample->loop_type) {

        case XM_NO_LOOP:
            if(XM_LINEAR_INTERPOLATION) {
                v = (b < ch->sample->length) ? xm_sample_at(ch->sample, b) : .0f;
            }
            ch->sample_position += ch->step;
            if(ch->sample_position >= ch->sample->length) {
                ch->sample_position = -1;
            }
            break;

        case XM_FORWARD_LOOP:
            if(XM_LINEAR_INTERPOLATION) {
                v = xm_sample_at(
                        ch->sample,
                        (b == ch->sample->loop_end) ? ch->sample->loop_start : b
                        );
            }
            ch->sample_position += ch->step;
            while(ch->sample_position >= ch->sample->loop_end) {
                ch->sample_position -= ch->sample->loop_length;
            }
            break;

        case XM_PING_PONG_LOOP:
            if(ch->ping) {
                ch->sample_position += ch->step;
            } else {
                ch->sample_position -= ch->step;
            }
            if(ch->ping) {
                if(XM_LINEAR_INTERPOLATION) {
                    v = xm_sample_at(ch->sample, (b >= ch->sample->loop_end) ? a : b);
                }
                if(ch->sample_position >= ch->sample->loop_end) {
                    ch->ping = false;
                    ch->sample_position = (ch->sample->loop_end << 1) - ch->sample_position;
                }
                if(ch->sample_position >= ch->sample->length) {
                    ch->ping = false;
                    ch->sample_position -= ch->sample->length - 1;
                }
            } else {
                if(XM_LINEAR_INTERPOLATION) {
                    v = u;
                    u = xm_sample_at(
                            ch->sample,
                            (b == 1 || b - 2 <= ch->sample->loop_start) ? a : (b - 2)
                            );
                }
                if(ch->sample_position <= ch->sample->loop_start) {
                    ch->ping = true;
                    ch->sample_position = (ch->sample->loop_start << 1) - ch->sample_position;
                }
                if(ch->sample_position <= .0f) {
                    ch->ping = true;
                    ch->sample_position = .0f;
                }
            }
            break;

        default:
            v = .0f;
            break;
    }

    float endval = (XM_LINEAR_INTERPOLATION ? XM_LERP(u, v, t) : u);

    if(ch->frame_count < XM_SAMPLE_RAMPING_POINTS) {
        return XM_LERP(ch->end_of_previous_sample[ch->frame_count], endval,
                (float)ch->frame_count / (float)XM_SAMPLE_RAMPING_POINTS);
    }

    return endval;
}

void xm_sample(xm_context_t* ctx, float* left, float* right) {
    if(ctx->remaining_samples_in_tick <= 0) {
        xm_tick(ctx);
    }
    ctx->remaining_samples_in_tick--;

    *left = 0.f;
    *right = 0.f;

    if(ctx->max_loop_count > 0 && ctx->loop_count >= ctx->max_loop_count) {
        return;
    }

    for(uint8_t i = 0; i < ctx->module.num_channels; ++i) {
        xm_channel_context_t* ch = ctx->channels + i;

        if(ch->instrument == NULL || ch->sample == NULL || ch->sample_position < 0) {
            continue;
        }

        const float fval = xm_next_of_sample(ch);

        if(!ch->muted && !ch->instrument->muted) {
            *left += fval * ch->actual_volume * (1.f - ch->actual_panning);
            *right += fval * ch->actual_volume * ch->actual_panning;
        }

        ch->frame_count++;
        XM_SLIDE_TOWARDS(ch->actual_volume, ch->target_volume, ctx->volume_ramp);
        XM_SLIDE_TOWARDS(ch->actual_panning, ch->target_panning, ctx->panning_ramp);
    }

    const float fgvol = ctx->global_volume * ctx->amplification;
    *left *= fgvol;
    *right *= fgvol;

}

void xm_generate_samples(xm_context_t* ctx, float* output, size_t numsamples) {
    ctx->generated_samples += numsamples;

    for(size_t i = 0; i < numsamples; i++) {
        xm_sample(ctx, output + (2 * i), output + (2 * i + 1));
    }
}

