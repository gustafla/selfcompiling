const int ITR=20;
const float EPS=0.000001;
const float MAX_T=100.;

float scene(vec3 point) {
    return distance(vec3(0., 0., 4.), point) -1.;
}

vec3 march(vec3 cam, vec3 ray) {
    float dist;
    float t=0.;
    for (int i=0; i<ITR; i++) {
        dist = scene(cam + ray * t);
        t += dist;
        if (dist < EPS || t > MAX_T) {
            break;
        }
    }
    return cam + ray * t;
}

void main() {
    vec3 ray = normalize(vec3((v_texturePos*2.-1.), 1.));
    vec3 p = march(vec3(0.), ray);
    fragColor = vec4(vec3(p.z/8.), 1.);
}
