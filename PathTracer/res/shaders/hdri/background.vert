#version 460 core
layout (location = 0) in vec3 aPos;
 
out vec3 v_WorldPos;
 
uniform mat4 u_Projection;
uniform mat4 u_View;
 
void main()
{
    v_WorldPos = aPos;

    mat4 rotView = mat4(mat3(u_View));
    vec4 clipPos = u_Projection * rotView * vec4(v_WorldPos, 1.0);

    gl_Position = clipPos.xyww;
}