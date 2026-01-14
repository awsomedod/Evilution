#version 450

layout(location = 0) in vec2 fragCoord;
layout(location = 0) out vec4 outColor;

layout(push_constant) uniform Push {
    float u_time;
    vec2 u_mouse;
    float u_aspect_ratio;
} push;

mat2 rotate2d(float _angle){
    return mat2(cos(_angle), -sin(_angle),
                sin(_angle), cos(_angle));
}

void main() { 
    // 1. Setup Coordinates
    // Ensure this results in a range roughly -1.0 to 1.0
    vec2 uv = vec2(fragCoord.x * push.u_aspect_ratio, -fragCoord.y);
    
    vec3 finalColor = vec3(0.0);
    
    // 2. Loop
    // The original shader runs the loop and accumulates rotation cumulatively.
    for (float i = 0.0; i < 20.0; i++) {
        
        // --- A. Cumulative Rotation ---
        // The original rotates the coordinate system itself.
        // Angle logic matches the target: (time + i) * speed
        // Note: In the original, 'i' increments inside the rotation, so we use i for rotation
        // but (i+1) for the drawing logic below to match that execution order.
        float angle = (push.u_time + i) * 0.03;
        uv = rotate2d(angle) * uv; 

        // --- B. Squircle Distance Metric ---
        // Squaring the vector components (element-wise) gives the squircle shape
        vec2 uvSq = uv * uv;
        float rho = length(uvSq); // Effectively sqrt(x^4 + y^4)
        
        // Determine the radius for this iteration.
        // We use (i + 1.0) because the original target code increments 'i' 
        // before calculating the color/distance for that step.
        float index = i + 1.0;
        float radius = index * 0.04;
        
        // Calculate distance to the ring surface
        float dist = abs(rho - radius);

        // --- C. Glow / Intensity ---
        // Inverse distance function for "neon" glow
        float intensity = 0.004 / (dist + 0.005);
        intensity = pow(intensity, 1.4);

        // --- D. Animation / Pulse ---
        // Matches your logic and the target exactly
        float pulse = smoothstep(0.35, 0.4, abs(abs(mod(push.u_time, 2.0) - index * 0.1) - 1.0));

        // --- E. Color Accumulation ---
        // Cosine palette based on the index
        vec3 color = cos(index + vec3(0.0, 1.0, 2.0)) + 1.0;
        
        finalColor += color * intensity * pulse;
    }

    outColor = vec4(finalColor, 1.0);
}