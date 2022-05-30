#version 460 core

layout (location = 0) out vec4 FragColor;
layout (location = 2) out vec4 FragDepth;

in vec3 v_WorldPos;
 
uniform samplerCube u_EnvironmentMap;
uniform bool u_IsMapUsing;
uniform vec3 u_EnvironmentColor;
uniform float u_Intisity;
uniform mat4 u_Rotation;
 
void main()
{		
	if(u_IsMapUsing)
	{
		//vec3 envColor = texture(u_EnvironmentMap, v_WorldPos).rgb * u_Intisity;
		vec3 envColor = textureLod(u_EnvironmentMap, vec3(u_Rotation * vec4(v_WorldPos, 1.0f)), 0).rgb * u_Intisity;

		envColor = envColor / (envColor + vec3(1.0));
		envColor = pow(envColor, vec3(1.0/2.2));
	
		FragColor = vec4(envColor, 1.0);
	}
	else
		FragColor = vec4(u_EnvironmentColor, 1.0f);


	FragDepth = vec4(vec3(pow(gl_FragCoord.z, 35)), 0.0);
}