#version 330 core
out vec4 FragColor;

struct Material {
	sampler2D texture_diffuse1;
	sampler2D texture_roughness1;
};

uniform vec4 u_Color;
uniform sampler2D u_Texture;
uniform vec3 u_LightPos;
uniform Material u_material;

in vec2 v_TexCoord;
in vec3 v_Normal;
in vec3 v_FragPos;

void main()
{
	vec4 texColor = texture(u_material.texture_diffuse1, v_TexCoord);

	FragColor = texColor;
}