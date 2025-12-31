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
    vec2 d = abs(p);
    d = sin(d *8.)/8.;
    return length(max(d, 0.0)) + min(max(d.x,d.y),0.0);
}


void main() { 

    float d = sdBox(fragCoord, vec2(0.5));
    // d = sin(d *8.)/8.;
    d = abs(d);

    d = smoothstep(0.0, 0.1, d);

    outColor = vec4(d, d, d, 1.0);
}
