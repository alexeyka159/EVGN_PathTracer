#version 460 core

layout (location = 0) out vec4 FragColor;
layout (location = 2) out vec4 FragDepth;

in vec3 v_WorldPos;
 
uniform samplerCube u_EnvironmentMap;
 
void main()
{		
    vec3 envColor = texture(u_EnvironmentMap, v_WorldPos).rgb;
    
    envColor = envColor / (envColor + vec3(1.0));
    envColor = pow(envColor, vec3(1.0/2.2));
  
    FragColor = vec4(envColor, 1.0);

    FragDepth = vec4(vec3(pow(gl_FragCoord.z, 35)), 0.0);
}