#version 450

layout(location = 0) in vec2 position;

layout(push_constant) uniform Push {
    float u_time;
    vec2 u_mouse;  // Normalized mouse position (0-1)
    float u_aspect_ratio; // aspect ratio of the swap chain
} push;

layout(location = 0) out vec2 fragCoord;  // Pass to fragment shader

void main() {
    gl_Position = vec4(position, 0.0, 1.0);
    
    // Convert from NDC (-1 to 1) to normalized coords (0 to 1)
    fragCoord = vec2(position.x * push.u_aspect_ratio, -position.y);
}
