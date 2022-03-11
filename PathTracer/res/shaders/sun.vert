#version 330 core
layout (location = 0) in vec4 aPos;
layout (location = 1) in vec3 aNorm;
layout (location = 2) in vec2 aTexCoord;

out vec2 v_TexCoord;
out vec3 v_Normal;
out vec3 v_FragPos; 

uniform mat4 u_MVP;
uniform mat4 u_Model;

void main()
{
	gl_Position = u_MVP * aPos;
	v_FragPos = vec3(u_Model * aPos);
	v_TexCoord = aTexCoord;
	v_Normal = normalize(aNorm);
}