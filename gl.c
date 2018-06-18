#ifdef DEBUG
#include "includes.c"
#include "read_file.c"
#endif

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
                "uniform float u_aspect_ratio;\n"
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
