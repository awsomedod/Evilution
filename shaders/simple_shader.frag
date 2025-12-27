#version 450
#define PI 3.14159265359

layout(location = 0) in vec2 fragCoord;  // Normalized 0-1 coords from vertex shader
layout(location = 0) out vec4 outColor;

layout(push_constant) uniform Push {
    float u_time;
    vec2 u_mouse;  // Normalized mouse position (0-1)
} push;

vec3 colorA = vec3(0.912,0.141,0.141);
vec3 colorB = vec3(0.141,0.912,0.912);

float plot (vec2 st, float pct){
  return  smoothstep( pct-0.01, pct, st.y) -
          smoothstep( pct, pct+0.01, st.y);
}
                
float ploty (vec2 st, float pct){
  return  smoothstep( pct-0.01, pct, st.x) -
          smoothstep( pct, pct+0.01, st.x);
}

void main() {    
    vec3 color = vec3(0.0);

    vec3 pct = vec3(0);

    pct.b = pow((fragCoord.y * 3.0) - 1.5, 2.0);
    pct.r = pow((fragCoord.x *3.0) - 2.0, 2.0);
    pct.r = smoothstep(pct.r,1.0, fragCoord.y);
    pct.b = smoothstep(pct.b,1.0, fragCoord.y);
    pct.g = smoothstep(pct.g,1.0, fragCoord.y);
    // pct.g = sin(fragCoord.x*PI);
    // pct.b = pow(fragCoord.x,0.5);

    color = mix(colorA, colorB, pct);

    // Plot transition lines for each channel
    color = mix(color,vec3(0.0,0.0,1.0),ploty(fragCoord,pct.b));
    color = mix(color,vec3(1.0,0.0,0.0),plot(fragCoord,pct.r));
    // color = mix(color,vec3(0.0,0.0,1.0),plot(fragCoord,pct.b));

    outColor = vec4(color,1.0);
}
