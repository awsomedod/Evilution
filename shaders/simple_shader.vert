#version 450

layout(location = 0) in vec2 position;
layout(location = 1) in vec3 color;

layout(location = 0) out vec3 fragColor;

layout(push_constant) uniform Push {
    mat2 transform;
    vec2 offset;
    vec3 color;
} push;

vec3 getColorForPosition(float x) {
    // Define your color stops (you can adjust these colors)
    vec3 color1 = vec3(0.65, 0.89, 0.18);  // Monokai green
    vec3 color2 = vec3(0.0, 1.0, 0.0);     // Green screen
    vec3 color3 = vec3(0.1, 0.1, 1.0);     // Sky blue

    // Normalize x position to 0-1 range
    float normalizedX = (x + 1.0) * 0.5;  // Convert from [-1,1] to [0,1]
    
    // Determine which color segment we're in
    if (normalizedX < 0.5) {
        return mix(color1, color2, normalizedX * 2.0);
    } else {
        return mix(color2, color3, (normalizedX - 0.5) * 3.0);
    }
}

void main() {
    vec4 transformedPos = vec4(push.transform * position + push.offset, 0.0, 1.0);
    gl_Position = transformedPos;
    
    // Calculate color based on the transformed x position
    fragColor = getColorForPosition(transformedPos.x);
}