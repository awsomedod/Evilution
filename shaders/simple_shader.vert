#version 450

layout(location = 0) in vec2 position;

layout(push_constant) uniform Push {
    float u_time;
    vec2 u_mouse;
    float u_aspect_ratio; // aspect ratio of the swap chain
} push;

layout(location = 0) out vec2 fragCoord;  // Pass to fragment shader

void main() {
    gl_Position = vec4(position, 0.0, 1.0);
    
    fragCoord = position;
}
