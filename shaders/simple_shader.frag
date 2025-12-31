#version 450
#define PI 3.14159265359

layout(location = 0) in vec2 fragCoord;  // Normalized 0-1 coords from vertex shader
layout(location = 0) out vec4 outColor;

layout(push_constant) uniform Push {
    float u_time;
    vec2 u_mouse;  // Normalized mouse position (0-1)
    float u_aspect_ratio; // aspect ratio of the swap chain
} push;

vec3 palette(float t) {
    vec3 a = vec3(0.5, 0.5, 0.5);
    vec3 b = vec3(0.5, 0.5, 0.5);
    vec3 c = vec3(1.0, 1.0, 1.0);
    vec3 d = vec3(0.263, 0.416, 0.557);
    
    return a + b*cos(2*PI*(c*t+d));
}

float normLinf(vec2 v)
{
    return max(abs(v.x), abs(v.y));
}

float sdBox( in vec2 p, in vec2 b )
{
    vec2 d = abs(p) - b;
    d = sin(d *8 + push.u_time)/8.;
    return normLinf(max(d, 0.0)) + min(max(d.x,d.y),0.0);
}

float properSdBox( in vec2 p, in vec2 b )
{
    vec2 d = abs(p) - b;
    return normLinf(max(d, 0.0)) + min(max(d.x,d.y),0.0);
}



void main() { 

    vec2 uv = fragCoord;
    vec2 uv0 = uv;
    vec3 finalColor = vec3(0.0);

    for (float i = 0.0; i < 4.0; i++) {
        uv = fract(uv * 1.2) - .5;

        float d = sdBox(uv, vec2(1)) * exp(-sdBox(uv, vec2(1)));

        vec3 color = palette(properSdBox(uv0, vec2(1)) + i*.4 + push.u_time);

        d = sin(d *8)/8.;
        d = abs(d);

        d = .01/d;
        d = pow(d, 2.4);

        finalColor += color * d;
    }

    outColor = vec4(finalColor, 1.0);
}
