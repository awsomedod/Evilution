#version 450
#define PI 3.14159265359

layout(location = 0) in vec2 fragCoord;  // Normalized 0-1 coords from vertex shader
layout(location = 0) out vec4 outColor;

layout(push_constant) uniform Push {
    float u_time;
    vec2 u_mouse;  // Normalized mouse position (0-1)
    float u_aspect_ratio; // aspect ratio of the swap chain
} push;

void main() {    
    float d = length(fragCoord);

    d = sin(d * 8.0 + push.u_time)/8.0;
    d = abs(d);

    d = smoothstep(0.0, 0.1, d);

    outColor = vec4(vec3(d), 1.0);
}
