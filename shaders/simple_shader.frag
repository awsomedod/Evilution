#version 450

layout(location = 0) in vec2 fragCoord;  // Normalized 0-1 coords from vertex shader
layout(location = 0) out vec4 outColor;

layout(push_constant) uniform Push {
    float u_time;
    vec2 u_mouse;  // Normalized mouse position (0-1)
} push;

void main() {    
    outColor = vec4(abs(sin(push.u_time / 2.0)),abs(sin(push.u_time / 3.0)),abs(sin(push.u_time / 7.0)),1.0);
}
