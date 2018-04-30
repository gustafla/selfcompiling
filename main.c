#include <stdio.h>

void play(void *d, uint8_t *stream, int len) {
    xm_generate_samples((xm_context_t*)d, (float*)stream, (len/4)/2);
}

void music() {
    xm_context_t *xm;
    xm_create_context_safe(&xm, UNREEEAL_SUPERHERO_3_XM, UNREEEAL_SUPERHERO_3_XM_LEN, 48000);

    A want, have;
    want.freq = 48000;
    want.format = 0x8120; // SDL_AUDIO_F32
    want.channels = 2;
    want.samples = 2048;
    want.callback = play;
    want.userdata = (void*)xm;

    uint32_t dev = SDL_OpenAudioDevice(NULL, 0, &want, &have, 0);
    SDL_PauseAudioDevice(dev, 0);
}

unsigned compile_shader(unsigned type, const char *src) {
    unsigned s = glCreateShader(type);

    // add correct header to shader
    const char *full_src[2];
    switch (type) {
        case 0x8B31:
            full_src[0] = "#version 330\n"
                "layout (location = 0) in vec3 a_pos;\n"
                "layout (location = 1) in vec2 a_texturePos;\n"
                "out vec3 v_pos;\n"
                "out vec2 v_texturePos;\n";
            break;
        case 0x8B30:
            full_src[0] = "#version 330\n"
                "out vec4 fragColor;\n"
                "in vec2 v_texturePos;\n";
            break;
        default:
            full_src[0] = "#version 330\n";
    }
    full_src[1] = src;

    glShaderSource(s, 2, full_src, NULL);
    glCompileShader(s);
    int success; char info[512];
    glGetShaderiv(s, 0x8B81, &success); // GL_COMPILE_STATUS
    if (!success) {
        glGetShaderInfoLog(s, 512, NULL, info);
        printf("%s", info);
    }
    return s;
}

unsigned link_program(const char *vertex_src, const char *fragment_src) {
    unsigned p = glCreateProgram();
    glAttachShader(p, compile_shader(0x8B31, vertex_src));
    glAttachShader(p, compile_shader(0x8B30, fragment_src));
    glLinkProgram(p);
    int success; char info[512];
    glGetProgramiv(p, 0x8B82, &success);
    if (!success) {
        glGetProgramInfoLog(p, 512, NULL, info);
        printf("%s", info);
        return 0;
    }
    return p;
}

unsigned create_buf(unsigned target, size_t size, const void *data) {
    unsigned buf;
    glGenBuffers(1, &buf);
    glBindBuffer(target, buf);
    glBufferData(target, size, data, 0x88E4); // GL_STATIC_DRAW
    glBindBuffer(target, 0);
    return buf;
}

void render(unsigned program, unsigned vertex_array) {
    glUseProgram(program);
    glBindVertexArray(vertex_array);
    glDrawArrays(0x0004, 0, 6); // GL_TRIANGLES
    glBindVertexArray(0);
}

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
    SDL_GL_CreateContext(w); // SDL_GL_LoadLibrary, SDL_GL_MakeCurrent

    // Load OpenGL functions using SDL_GetProcAddress
    gl();

    // Compile and link shaders
    unsigned s = link_program(
            SHADERS_TRIVIAL_GLSL_MIN, SHADERS_FRAGMENT_GLSL_MIN);
    if (!s) {
        return EXIT_FAILURE;
    }

    // Generate a VBO
    float const vertices[] = {
        -1.f, -1.f, 0.f, 0.f, 0.f,
        1.f, -1.f, 0.f, 1.f, 0.f,
        1.f, 1.f, 0.f, 1.f, 1.f,
        1.f, 1.f, 0.f, 1.f, 1.f,
        -1.f, 1.f, 0.f, 0.f, 1.f,
        -1.f, -1.f, 0.f, 0.f, 0.f
    };
    //GL_ARRAY_BUFFER
    unsigned buf = create_buf(0x8892, sizeof(vertices), vertices);

    // Generate the VAO for the shader quad
    unsigned vertex_array;
    glGenVertexArrays(1, &vertex_array);
    glBindVertexArray(vertex_array);
    glBindBuffer(0x8892, buf); // GL_ARRAY_BUFFER
    size_t sf = sizeof(float);
    glVertexAttribPointer(0, 3, 0x1406, 0, 5 * sf, (void*)0); // GL_FLOAT
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, 0x1406, 0, 5 * sf, (void*)(3*sf));
    glEnableVertexAttribArray(1);

    // Start playing music
    music();

    // Event struct
    E e;
    while(1) {
        // GL_COLOR_BUFFER_BIT = 0x4000
        //glClear(0x4000);

        render(s, vertex_array);

        SDL_GL_SwapWindow(w);

        SDL_PollEvent(&e);
        // SDL_QUIT = 0x100, SDL_KEYDOWN = 0x300
        if (e.type == 0x100 || e.type == 0x300) break;
    }

    return 0;
}

