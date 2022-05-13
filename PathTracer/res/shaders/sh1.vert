#version 460 core
layout (location = 0) in vec4 aPos;
layout (location = 1) in vec3 aNorm;
layout (location = 2) in vec2 aTexCoord;

out vec2 v_TexCoord;
out vec3 v_Normal;
out vec3 v_FragPos;
out vec3 v_LightDir;
out vec3 v_LightColor;

uniform mat4 u_MVP;
uniform mat4 u_Model;

uniform vec3 u_LightPos = vec3(0);
uniform vec3 u_LightColor = vec3(0);
uniform float u_LightIntensity = 0;

void main()
{
	gl_Position = u_MVP * aPos;
	v_FragPos = vec3(u_Model * aPos);
	v_TexCoord = aTexCoord;
	v_Normal = mat3(transpose(inverse(u_Model))) * aNorm;
	/* v_Normal = normalize(vec3(u_Model * vec4(aNorm, 0))); */
	v_LightDir = normalize(u_LightPos - v_FragPos);
	v_LightColor = u_LightColor * u_LightIntensity;
}