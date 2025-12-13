#version 450

layout(location = 0) in vec2 position;

layout(location = 0) out vec2 fragCoord;  // Pass to fragment shader

void main() {
    gl_Position = vec4(position, 0.0, 1.0);
    
    // Convert from NDC (-1 to 1) to normalized coords (0 to 1)
    fragCoord = (position + 1.0) * 0.5;
}
