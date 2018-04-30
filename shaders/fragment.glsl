void main() {
    fragColor = vec4(v_texturePos.x+sin(u_time), v_texturePos.y, .5, 1.);
}
