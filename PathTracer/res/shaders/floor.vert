#version 460 core
layout (location = 0) in vec4 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec3 v_NearPoint;
out vec3 v_FarPoint;

out mat4 v_FragView;
out mat4 v_FragProj;

out vec4 v_Pos;

uniform mat4 u_View;
uniform mat4 u_Proj;
uniform mat4 u_Model;



void main() {
    vec4 p = u_Proj * u_View * u_Model * aPos;

    gl_Position = vec4(p);
    v_Pos = aPos;
    v_FragView = u_View;
    v_FragProj = u_Proj;

}