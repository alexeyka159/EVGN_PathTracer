#version 330 core
layout (location = 0) in vec4 aPos;

out vec2 v_TexCoord;

uniform float u_Size;
uniform vec3 u_Color;

out vec4 v_Color;

void main()
{
	v_Color = vec4(u_Color, aPos.a / u_Size);
	gl_Position = vec4(aPos.xyz, 1.0f);
}