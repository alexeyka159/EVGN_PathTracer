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
	sampler2D texture_metallic1;
	sampler2D texture_roughness1;
	sampler2D texture_normal1;
	// sampler2D texture_ao1;

	float ior;

	vec3 diffuseColor;
	bool isDiffuseUsing;
	float diffuseValue;
	float diffuseContrast;

	float metallicColor;
	bool isMetallicUsing;

	float roughnessColor;
	bool isRoughnessUsing;
	bool isRoughnessInvert;
	float roughnessValue;
	float roughnessContrast;

	bool isNormalUsing;
	float normalStrength;
};

struct Environment {
	samplerCube irradianceMap;
	bool isIrradianceMapUsing;
	vec3 color;
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
uniform Material u_Material;
uniform Environment u_Environment;
uniform int u_EntityID;
uniform vec3 u_ViewPos;

in vec3 v_Normal;

const float PI = 3.14159265359;

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, vec3 diffCol, float roughCol, float metalCol, vec3 IOR);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 diffCol, float roughCol, float metalCol, vec3 IOR);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 diffCol, float roughCol, float metalCol, vec3 IOR);

vec3 CalcLo(vec3 lightPosDir, vec3 lightColor, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 diffCol, float roughCol, float metalCol, vec3 IOR, bool calcAttenuation);
float DistributionGGX(vec3 N, vec3 H, float roughness);
float GeometrySchlickGGX(float NdotV, float roughness);
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness);
vec3 fresnelSchlick(float cosTheta, vec3 IOR);
vec3 fresnelSchlickRoughness(float cosTheta, vec3 IOR, float roughness);

vec3 CalcAmbient(vec3 irradianceCol, vec3 normal, vec3 viewDir, vec3 diffCol, float roughCol, float IOR);

void main()
{
	//Выборка цветов из текстур, если необходимо
	vec3 diffuseColor = u_Material.diffuseColor;
	float metallicColor = u_Material.metallicColor;
	float roughnessColor = u_Material.roughnessColor;
	vec3 normalColor = v_Normal;
	vec3 irradianceColor = u_Environment.color;

	if(u_Material.isDiffuseUsing)
	{
		diffuseColor = texture(u_Material.texture_diffuse1, fs_in.TexCoords).rgb;
		diffuseColor = ((diffuseColor - 0.5f) * max(u_Material.diffuseContrast, 0)) + 0.5f;
		diffuseColor *= u_Material.diffuseValue;
	}
	if(u_Material.isMetallicUsing)
	{
		metallicColor = texture(u_Material.texture_metallic1, fs_in.TexCoords).r;
	}
	if(u_Material.isRoughnessUsing)
	{
		roughnessColor = texture(u_Material.texture_roughness1, fs_in.TexCoords).r;
		roughnessColor = ((roughnessColor - 0.5f) * max(u_Material.roughnessContrast, 0)) + 0.5f;
		if(u_Material.isRoughnessInvert)
			roughnessColor = 1 - roughnessColor;
		roughnessColor *= u_Material.roughnessValue;
	}
	if(u_Material.isNormalUsing)
	{
		normalColor = texture(u_Material.texture_normal1, fs_in.TexCoords).rgb;
		normalColor = normalColor * 2.0f - 1.0f;
		normalColor.xy *= u_Material.normalStrength;
		normalColor = normalize(normalColor);
		normalColor = normalize(fs_in.TBN * normalColor);
	}
	if(/* u_Environment.isIrradianceMapUsing */ true)
	{
		irradianceColor = texture(u_Environment.irradianceMap, normalColor).rgb;
	}


	vec3 viewDir = normalize(u_ViewPos - fs_in.FragPos);
	vec3 result = vec3(0);

	vec3 IOR = vec3(u_Material.ior);
	IOR = mix(IOR, diffuseColor, metallicColor);
 
	//Направленное освещение
	for(int i = 0; i < NR_DIR_LIGHTS; i++)
		if(u_DirLightsInUse[i])
			result += CalcDirLight(u_DirLights[i], normalColor, viewDir, diffuseColor, roughnessColor, metallicColor, IOR);
	
	//Точечные источники света
	for(int i = 0; i < NR_POINT_LIGHTS; i++)
		if(u_PointLightsInUse[i])
			result += CalcPointLight(u_PointLights[i], normalColor, fs_in.FragPos, viewDir, diffuseColor, roughnessColor, metallicColor, IOR); 
  
	//Прожектор
	for(int i = 0; i < NR_SPOT_LIGHTS; i++)
		if(u_SpotLightsInUse[i])
			result += CalcSpotLight(u_SpotLights[i], normalColor, fs_in.FragPos, viewDir, diffuseColor, roughnessColor, metallicColor, IOR);
	
	result += CalcAmbient(irradianceColor, normalColor, viewDir, diffuseColor, roughnessColor, IOR.r);

	//Тональная компрессия HDR 
	result = result / (result + vec3(1.0f));
	// Гамма-коррекция
	result = pow(result, vec3(1.0f/2.2f));
	FragColor = vec4(result, 1.0);

	entityID = u_EntityID;

	FragDepth = vec4(vec3(pow(gl_FragCoord.z, 35)), 1.0);
}


//Пресеты источников света
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, vec3 diffCol, float roughCol, float metalCol, vec3 IOR)
{
	return CalcLo(light.direction, light.color, normal, vec3(0), viewDir, diffCol, roughCol, metalCol, IOR, false);
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 diffCol, float roughCol, float metalCol, vec3 IOR)
{
  return CalcLo(light.position, light.color, normal, fragPos, viewDir, diffCol, roughCol, metalCol, IOR, true);
}

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 diffCol, float roughCol, float metalCol, vec3 IOR)
{
	vec3 lightDir = normalize(light.position - fragPos);
	
	// Интенсивность прожектора
	float theta = dot(lightDir, normalize(-light.direction)); 
	float epsilon = light.cutOff - light.outerCutOff;
	float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
	
	vec3 Lo = CalcLo(light.position, light.color, normal, fragPos, viewDir, diffCol, roughCol, metalCol, IOR, true);
	return Lo * intensity;
}


//PBR
vec3 CalcLo(vec3 lightPosDir, vec3 lightColor, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 diffCol, float roughCol, float metalCol, vec3 IOR, bool calcAttenuation)
{
	vec3 Lo = vec3(0);

	vec3 L;
	vec3 H;
	vec3 radiance;

	if(calcAttenuation)
	{
		L = normalize(lightPosDir - fragPos);
	
		// Отраженное затенение
		H = normalize(viewDir + L);
	
		// Затухание
		float distance = length(lightPosDir - fragPos);
  	float attenuation = 1.0 / (distance * distance);
  	radiance = lightColor * attenuation; 
	}
	else
	{
		L = normalize(-lightPosDir);
		H = normalize(viewDir + (-lightPosDir));
		radiance = lightColor;
	}

	// BRDF Кука-Торренса 
	float NDF = DistributionGGX(normal, H, roughCol);   
	float G = GeometrySmith(normal, viewDir, L, roughCol);      
	vec3 F = fresnelSchlick(max(dot(H, viewDir), 0.0), IOR);

	vec3 nominator = NDF * G * F; 
	float denominator = 4 * max(dot(normal, viewDir), 0.0) * max(dot(normal, L), 0.0) + 0.001; // прибавляем 0.001, чтобы избежать деления на ноль
	vec3 specular = nominator / denominator;

	// kS эквивалентно коэффициенту Френеля
	vec3 kS = F;

	// Чтобы выполнялся закон сохранения энергии, сумма энергий диффузной и отраженной составляющих света не может быть больше 1.0 
	// (кроме тех случаев, когда сама поверхность имеет возможность излучать свет); 
	// для выполнения данного соотношения диффузная составляющая (kD) должна равняться значению 1.0 - kS
	vec3 kD = vec3(1.0) - kS;

	// Умножаем kD на значение "1 - металличность", чтобы только неметаллы имели диффузное освещение, 
	// или линейную композицию в случае полуметаллического материала (чисто металлические материалы не имеют диффузного освещения)
  kD *= 1.0 - metalCol;

  // Масштабируем освещенность при помощи NdotL
  float NdotL = max(dot(normal, L), 0.0);        

  // Добавляем к исходящей энергитической яркости Lo
  Lo += (kD * diffCol / PI + specular) * radiance * NdotL;

  return Lo;
}

vec3 CalcAmbient(vec3 irradianceCol, vec3 normal, vec3 viewDir, vec3 diffCol, float roughCol, float IOR)
{
	vec3 kS = fresnelSchlickRoughness(max(dot(normal, viewDir), 0.0), vec3(IOR), roughCol);
	vec3 kD = 1.0 - kS;
	vec3 diffuse = irradianceCol * diffCol;
	vec3 ao = vec3(1); //AO
	return (kD * diffuse) * ao;
}

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
		float a = roughness*roughness;
		float a2 = a*a;
		float NdotH = max(dot(N, H), 0.0);
		float NdotH2 = NdotH*NdotH;

		float nom   = a2;
		float denom = (NdotH2 * (a2 - 1.0) + 1.0);
		denom = PI * denom * denom;

		return nom / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
		float r = (roughness + 1.0);
		float k = (r*r) / 8.0;

		float nom   = NdotV;
		float denom = NdotV * (1.0 - k) + k;

		return nom / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
		float NdotV = max(dot(N, V), 0.0);
		float NdotL = max(dot(N, L), 0.0);
		float ggx2 = GeometrySchlickGGX(NdotV, roughness);
		float ggx1 = GeometrySchlickGGX(NdotL, roughness);

		return ggx1 * ggx2;
}

vec3 fresnelSchlick(float cosTheta, vec3 IOR)
{
		return IOR + (1.0 - IOR) * pow(1.0 - cosTheta, 5.0);
}

vec3 fresnelSchlickRoughness(float cosTheta, vec3 IOR, float roughness)
{
    return IOR + (max(vec3(1.0 - roughness), IOR) - IOR) * pow(1.0 - cosTheta, 5.0);
}