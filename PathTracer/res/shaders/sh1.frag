#version 460 core

layout (location = 0) out vec4 FragColor;
layout (location = 1) out int entityID;
layout (location = 2) out vec4 FragDepth;

in VS_OUT {
	mat3 TBN;
  vec3 FragPos;
  vec2 TexCoords;
} fs_in;

struct Material {
	sampler2D texture_diffuse1;
	sampler2D texture_roughness1;
	sampler2D texture_normal1;
};

struct DirLight {
	vec3 direction;
	vec3 color;
};

struct PointLight {
	vec3 position;
	vec3 color;

	float constant;
    float linear;
    float quadratic;
};

struct SpotLight {
    vec3 position;
    vec3 color;
    vec3 direction;
    float cutOff;
    float outerCutOff;
  
    float constant;
    float linear;
    float quadratic;
};

#define NR_POINT_LIGHTS 10 
#define NR_SPOT_LIGHTS 10
#define NR_DIR_LIGHTS 5
uniform PointLight u_PointLights[NR_POINT_LIGHTS];
uniform DirLight u_DirLights[NR_DIR_LIGHTS];
uniform SpotLight u_SpotLights[NR_SPOT_LIGHTS];
uniform bool u_DirLightsInUse[NR_DIR_LIGHTS];
uniform bool u_SpotLightsInUse[NR_SPOT_LIGHTS];
uniform bool u_PointLightsInUse[NR_POINT_LIGHTS];


uniform vec4 u_Color;
uniform sampler2D u_Texture;
uniform Material u_material;
uniform int u_EntityID;
uniform vec3 u_ViewPos;
uniform vec3 u_AmbientColor;

in vec3 v_Normal;

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main()
{
	//Эмбиент
	//float ambientStrength = 0.3;
  vec3 ambient = u_AmbientColor * vec3(texture(u_material.texture_diffuse1, fs_in.TexCoords));

  // Свойства
  //vec3 norm = normalize(v_Normal);
  vec3 norm = texture(u_material.texture_normal1, fs_in.TexCoords).rgb;
  norm = norm * 2.0f - 1.0f;
  norm = normalize(fs_in.TBN * norm); 

  vec3 viewDir = normalize(u_ViewPos - fs_in.FragPos);
  vec3 result = vec3(0);
 
  //Направленное освещение
  for(int i = 0; i < NR_DIR_LIGHTS; i++)
  	if(u_DirLightsInUse[i])
  		result += CalcDirLight(u_DirLights[i], norm, viewDir);
  
  //Точечные источники света
  for(int i = 0; i < NR_POINT_LIGHTS; i++)
  	if(u_PointLightsInUse[i])
      	result += CalcPointLight(u_PointLights[i], norm, fs_in.FragPos, viewDir); 
 
  //Прожектор
  for(int i = 0; i < NR_SPOT_LIGHTS; i++)
  	if(u_SpotLightsInUse[i])
  		result += CalcSpotLight(u_SpotLights[i], norm, fs_in.FragPos, viewDir);

  result += ambient;


  float gamma = 2.2f;
	FragColor = vec4(pow(result, vec3(1.0/gamma)), 1.0);

	entityID = u_EntityID;

 	FragDepth = vec4(vec3(pow(gl_FragCoord.z, 35)), 1.0);
}

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
 
    // Рассеянное затенение
    float diff = max(dot(normal, lightDir), 0.0);
 
    // Отраженное затенение
		vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 32); //TODO: Добавить в марериал спекулар
 
    // Комбинируем результаты
    //vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));
    vec3 diffuse = light.color * diff * vec3(texture(u_material.texture_diffuse1, fs_in.TexCoords));
    float roughness = 1 - texture(u_material.texture_roughness1, fs_in.TexCoords).r;
    vec3 specular = light.color * spec * roughness;
    return (diffuse + specular);
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
 
    // Рассеянное затенение
    float diff = max(dot(normal, lightDir), 0.0);
 

    // Отраженное затенение
		vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 32); //TODO: Добавить в марериал спекулар
 
    // Затухание
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (/* light.constant + light.linear * distance + light.quadratic *  */(distance * distance));    
 
    // Комбинируем результаты
    //vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));
    vec3 diffuse = light.color * diff * vec3(texture(u_material.texture_diffuse1, fs_in.TexCoords));
    float roughness = 1 - texture(u_material.texture_roughness1, fs_in.TexCoords).r;
    vec3 specular = light.color * spec * roughness;
    //ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    return vec3(diffuse + specular);
}

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
	
    // Диффузное затенение
    float diff = max(dot(normal, lightDir), 0.0);
	
    // Отраженное затенение
		vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 32); //TODO: Добавить в марериал спекулар
	
    // Затухание
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (/* light.constant + light.linear * distance + light.quadratic *  */(distance * distance)); 
	
    // Интенсивность прожектора
    float theta = dot(lightDir, normalize(-light.direction)); 
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
	
    // Совмещаем результаты
    //vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));
    vec3 diffuse = light.color * diff * vec3(texture(u_material.texture_diffuse1, fs_in.TexCoords));
    float roughness = 1 - texture(u_material.texture_roughness1, fs_in.TexCoords).r;
    vec3 specular = light.color * spec * roughness;
    //ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;
    return vec3(diffuse + specular);
}