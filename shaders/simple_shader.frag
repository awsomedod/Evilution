#version 450

layout (location = 0) in vec3 fragColor;
layout (location = 0) out vec4 outColor;

void main() {
    // Create exponential interpolation
    vec3 expColor = exp(fragColor * 4.0);
    
    // Normalize to maintain proper color intensity
    expColor = expColor / (expColor.r + expColor.g + expColor.b);
    
    outColor = vec4(expColor, 1.0);
}