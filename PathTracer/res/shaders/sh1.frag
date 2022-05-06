#version 460 core

layout (location = 0) out vec4 FragColor;
layout (location = 1) out int entityID;
layout (location = 2) out vec4 FragDepth;

struct Material {
	sampler2D texture_diffuse1;
	sampler2D texture_roughness1;
};

uniform vec4 u_Color;
uniform sampler2D u_Texture;
uniform Material u_material;
uniform int u_EntityID;
uniform vec3 u_ViewPos;

in vec2 v_TexCoord;
in vec3 v_Normal;
in vec3 v_FragPos;
in vec3 v_LightDir;

void main()
{
	vec3 lightColor = vec3(1.0f, 1.0f, 1.0f);

	//Эмбиент
	float ambientStrength = 0.3;
    vec3 ambient = ambientStrength * lightColor;

    //Диффуз
	float diff = max(dot(v_Normal, v_LightDir), 0.0);
	vec3 diffuse = diff * vec3(1.5f, 1.5f, 1.5f);

	//Спекулар
	float specularStrength = 1;
	vec3 viewDir = normalize(u_ViewPos - v_FragPos);
	vec3 reflectDir = reflect(-v_LightDir, v_Normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
	vec3 specular = specularStrength * spec * lightColor;  

	//Обджект колор
	vec4 texColor = texture(u_material.texture_diffuse1, v_TexCoord);

	//Рузультат
	vec3 result = (ambient + diffuse + specular) * vec3(texColor);
	FragColor = vec4(result, 1.0);

	entityID = u_EntityID;

    FragDepth = vec4(vec3(gl_FragCoord.z), 1.0);
}