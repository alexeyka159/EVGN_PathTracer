#version 460 core

layout (location = 0) out vec4 FragColor;
layout (location = 1) out int entityID;
layout (location = 2) out vec4 FragDepth;

in vec3 v_NearPoint;
in vec3 v_FarPoint;
in mat4 v_FragView;
in mat4 v_FragProj;
in vec4 v_Pos;
in vec2 v_TexCoord;

const float near = 0.1;
const float far = 1;

uniform sampler2D u_EntityTexture;

vec4 grid(vec3 pos, float scale, bool drawAxis) {

    vec2 coord = pos.xy * scale;
    vec2 derivative = fwidth(coord);

    vec2 grid = abs(fract(coord - 0.5) - 0.5) / derivative;

    float line = min(grid.x, grid.y);
    float minimumz = min(derivative.y, 1);
    float minimumx = min(derivative.x, 1);

    vec4 color = vec4(0.2, 0.2, 0.2, 1.0 - min(line, 1.0));
    
    // z axis
    if(pos.x * (scale/10) > -0.1 * minimumx && pos.x * (scale/10) < 0.1 * minimumx)
        color.z = 1.0;
    // x axis
    if(pos.y * (scale/10) > -0.1 * minimumz && pos.y * (scale/10) < 0.1 * minimumz)
        color.x = 1.0;

    return color;
}


void main() {

    float fading = 1 - pow(length(v_Pos)*0.9, 5);

    FragColor = grid(v_Pos.xyz, 100, true) + grid(v_Pos.xyz, 500, true);
    FragColor.a *= fading;
}