const int ITR=60;
const float EPS=0.00001;
const float MAX_T=100.;

int material = 0;

float xz_plane(vec3 p) {
    return p.y;
}

float sphere(vec3 p, float r) {
    return length(p) - r;
}

vec2 middle_sphere(vec3 p) {
    return vec2(sphere(p - vec3(sin(u_time/200.)*4., 0., 4.), 1.), 0.);
}

vec2 floor_plane(vec3 p) {
    return vec2(xz_plane(p - vec3(0., -1., 0.)), 1.);
}

vec2 sdf(vec3 p) {
    vec2 ms = middle_sphere(p);
    vec2 fp = floor_plane(p);
    if (ms.x < fp.x) {
        return ms;
    } else {
        return fp;
    }
}

vec3 march(vec3 cam, vec3 ray) {
    vec2 dist;
    float t=0.;
    for (int i=0; i<ITR; i++) {
        dist = sdf(cam + ray * t);
        t += dist.x;
        if (dist.x < EPS) {
            material = int(dist.y);
            break;
        }
        if (t > MAX_T) {
            material = -1;
            break;
        }
    }
    return cam + ray * t;
}

vec3 grad(vec3 p) {
    vec2 e = vec2(EPS, 0.);
    return (vec3(sdf(p+e.xyy).x, sdf(p+e.yxy).x, sdf(p+e.yyx).x) - sdf(p).x) / e.x;
}

vec3 checkerboard(vec3 p, float f) {
    float pattern = clamp(sin(p.x*f)*sin(p.z*f)*(1./EPS), 0., 1.);
    return vec3(pattern);
}

vec3 shade(vec3 p) {
    vec3 light = normalize(vec3(-0.6, -0.6, 1.));
    vec3 normal = normalize(grad(p));

    vec3 color = vec3(1.);
    float emit = 0.;
    if (material == -1) {
        color = vec3(0.);
    } else if (material == 1) {
        color = checkerboard(p, 20.);
    }
    return vec3(clamp(dot(-normal, light) * (color + emit), 0., 1.));
}

void main() {
    vec3 ray = normalize(vec3((v_texpos*2.-1.), 1.));
    vec3 p = march(vec3(0.), ray);
    gl_FragColor = vec4(shade(p), 1.);
}

