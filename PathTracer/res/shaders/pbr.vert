#version 460 core
layout (location = 0) in vec4 aPos;
layout (location = 1) in vec3 aNorm;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec3 aTangent;

out VS_OUT {
	mat3 TBN;
    vec3 FragPos;
    vec2 TexCoords;
} vs_out;

out vec2 v_TexCoord;
out vec3 v_Normal;
out vec3 v_FragPos;
out vec3 v_LightDir;
out vec3 v_LightColor;

uniform mat4 u_MVP;
uniform mat4 u_Model;

// uniform vec3 u_LightPos = vec3(0);
// uniform vec3 u_LightColor = vec3(0);
// uniform float u_LightIntensity = 0;

uniform vec3 u_ViewPos;

void main()
{
	vs_out.FragPos = vec3(u_Model * aPos);
	vs_out.TexCoords = aTexCoord;

	mat3 normalMatrix = transpose(inverse(mat3(u_Model)));

	vec3 T = normalize(vec3(u_Model * vec4(aTangent, 0.0)));
   	vec3 N = normalize(vec3(u_Model * vec4(aNorm, 0.0)));
   	vec3 B = normalize(cross(N, T));
   	//mat3 TBN = mat3(T, B, N);


    /* vec3 T = normalize(normalMatrix * aTangent);
    vec3 N = normalize(normalMatrix * aNorm);
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T); */

    vs_out.TBN = mat3(T, B, N);
	v_Normal = normalMatrix * aNorm;

	gl_Position = u_MVP * aPos;
}