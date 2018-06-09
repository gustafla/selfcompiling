const int ITR=50;
const float EPS=0.001;
const float MAX_T=120.;

int material = 0;

float xz_plane(vec3 p) {
    return p.y;
}

float sphere(vec3 p, float r) {
    return length(p) - r;
}

vec2 middle_sphere(vec3 p) {
    //return vec2(sphere(p - vec3(2.8, 2.5, 5.), 1.), 0.);
    return vec2(sphere(p - vec3(4., 2.5, 4.), 0.8), 0.);
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
        t += dist.x*1.6;
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

float random (vec2 st) {
    return fract(sin(dot(st.xy,vec2(12.9898,78.233)))*43758.5453123);
}

// Based on Morgan McGuire @morgan3d
// https://www.shadertoy.com/view/4dS3Wd
float noise (vec2 st) {
    vec2 i = floor(st);
    vec2 f = fract(st);
    float a = random(i);
    float b = random(i + vec2(1.0, 0.0));
    float c = random(i + vec2(0.0, 1.0));
    float d = random(i + vec2(1.0, 1.0));
    vec2 u = f * f * (3.0 - 2.0 * f);
    return mix(a, b, u.x) + (c - a)* u.y * (1.0 - u.x) + (d - b) * u.x * u.y;
}

const int OCTAVES=5;
float fbm (vec2 st) {
    float value = 0.0;
    float amplitude = 0.5;
    float frequency = 0.;
    for (int i = 0; i < OCTAVES; i++) {
        value += amplitude * noise(st);
        st *= 2.;
        amplitude *= .5;
    }
    return value;
}

vec3 sky(vec3 p) {
    return mix(vec3(0.2, 0.3, 0.7), vec3(0.3, 0.5, 1.1), p.y/20.);
}

vec3 shade(vec3 p) {
    if (p.z > 6.) {
        return sky(p);
    }
    vec3 light = normalize(vec3(0., 0., 1.));
    vec3 light2 = normalize(vec3(-0.2, -1., 0.2));
    vec3 normal = normalize(grad(p));

    vec3 color = vec3(1., 1., 0.);
    float emit = 0.2;
    if (material == -1) {
        color = vec3(0.);
    } else if (material == 1) {
        color = checkerboard(p, 10.);
    }
    float l = clamp(dot(-normal, light), 0., 1.);
    l += clamp(dot(-normal, light2), 0., 1.);
    return vec3(l * (color + emit));
}

void main() {
    vec3 ray = normalize(vec3((v_texpos*vec2(2.*u_aspect_ratio,2.)-vec2(u_aspect_ratio,1.)), 1.));
    vec3 p = march(vec3(sin(u_time*0.3)*0.1, sin(u_time*0.2)*0.1, sin(u_time*0.1)*0.3), ray);
    vec2 cloudpos = v_texpos*10.+vec2(u_time*0.6, 0.);
    cloudpos.y += sin(cloudpos.x-u_time)*0.2;
    cloudpos.y += sin(cloudpos.x*4.-u_time*0.3)*0.07;
    gl_FragColor = vec4(shade(p)+fbm(cloudpos*(sin(cloudpos.x+u_time)*.001+1.))*0.4*cloudpos.y/5., 1.);
}

