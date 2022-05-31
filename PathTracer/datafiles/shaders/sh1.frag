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

	vec3 diffuseColor;
	bool isDiffuseUsing;
	float diffuseValue;
	float diffuseContrast;

	float roughnessColor;
	bool isRoughnessUsing;
	bool isRoughnessInvert;
	float roughnessValue;
	float roughnessContrast;

	bool isNormalUsing;
	float normalStrength;
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

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, vec3 diffCol, float roughCol);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDirCol, vec3 diffCol, float rough);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDirCol, vec3 diffCol, float rough);

void main()
{
	vec3 diffuseColor = u_material.diffuseColor;
	float roughnessColor = u_material.roughnessColor;
	vec3 normalColor = v_Normal;

	if(u_material.isDiffuseUsing)
	{
		diffuseColor = texture(u_material.texture_diffuse1, fs_in.TexCoords).rgb;
		diffuseColor = ((diffuseColor - 0.5f) * max(u_material.diffuseContrast, 0)) + 0.5f;
		diffuseColor += u_material.diffuseValue;
	}
	if(u_material.isRoughnessUsing)
	{
		roughnessColor = texture(u_material.texture_roughness1, fs_in.TexCoords).r;
		roughnessColor = ((roughnessColor - 0.5f) * max(u_material.roughnessContrast, 0)) + 0.5f;
		if(u_material.isRoughnessInvert)
			roughnessColor = 1 - roughnessColor;
		roughnessColor += u_material.roughnessValue;
	}
	if(u_material.isNormalUsing)
	{
		normalColor = texture(u_material.texture_normal1, fs_in.TexCoords).rgb;
  	normalColor = normalColor * 2.0f - 1.0f;
  	normalColor.xy *= u_material.normalStrength;
  	normalColor = normalize(normalColor);
  	normalColor = normalize(fs_in.TBN * normalColor);
	}



	//Эмбиент
	//float ambientStrength = 0.3;
  vec3 ambient = u_AmbientColor * diffuseColor;

  vec3 viewDir = normalize(u_ViewPos - fs_in.FragPos);
  vec3 result = vec3(0);
 
  //Направленное освещение
  for(int i = 0; i < NR_DIR_LIGHTS; i++)
  	if(u_DirLightsInUse[i])
  		result += CalcDirLight(u_DirLights[i], normalColor, viewDir, diffuseColor, roughnessColor);
  
  //Точечные источники света
  for(int i = 0; i < NR_POINT_LIGHTS; i++)
  	if(u_PointLightsInUse[i])
      	result += CalcPointLight(u_PointLights[i], normalColor, fs_in.FragPos, viewDir, diffuseColor, roughnessColor); 
 
  //Прожектор
  for(int i = 0; i < NR_SPOT_LIGHTS; i++)
  	if(u_SpotLightsInUse[i])
  		result += CalcSpotLight(u_SpotLights[i], normalColor, fs_in.FragPos, viewDir, diffuseColor, roughnessColor);

  result += ambient;


  float gamma = 2.2f;
	FragColor = vec4(pow(result, vec3(1.0/gamma)), 1.0);

	entityID = u_EntityID;

 	FragDepth = vec4(vec3(pow(gl_FragCoord.z, 35)), 1.0);
}

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, vec3 diffCol, float roughCol)
{
    vec3 lightDir = normalize(-light.direction);
 
    // Рассеянное затенение
    float diff = max(dot(normal, lightDir), 0.0);
 
    // Отраженное затенение
		vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 32); //TODO: Добавить в марериал спекулар
 
    // Комбинируем результаты
    //vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));
    vec3 diffuse = light.color * diff * diffCol;
    float roughness = 1 - roughCol;
    vec3 specular = light.color * spec * roughness;
    return (diffuse + specular);
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 diffCol, float roughCol)
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
    vec3 diffuse = light.color * diff * diffCol;
    float roughness = 1 - roughCol;
    vec3 specular = light.color * spec * roughness;
    //ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    return vec3(diffuse + specular);
}

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 diffCol, float roughCol)
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
    vec3 diffuse = light.color * diff * diffCol;
    float roughness = 1 - roughCol;
    vec3 specular = light.color * spec * roughness;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;
    return vec3(diffuse + specular);
}