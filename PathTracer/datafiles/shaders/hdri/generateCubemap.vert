#version 460 core
layout (location = 0) in vec3 aPos;
 
out vec3 v_LocalPos;
 
uniform mat4 u_Projection;
uniform mat4 u_View;
 
void main()
{
    v_LocalPos = aPos;
    gl_Position =  u_Projection * u_View * vec4(v_LocalPos, 1.0);
}