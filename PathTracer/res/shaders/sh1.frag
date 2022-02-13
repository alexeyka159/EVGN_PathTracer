#version 330 core
out vec4 FragColor;

uniform vec4 u_Color;
uniform sampler2D u_Texture;
uniform vec3 u_LightPos;

in vec2 v_TexCoord;
in vec3 v_Normal;
in vec3 v_FragPos;

void main()
{
	vec3 lightDir = normalize(u_LightPos - v_FragPos);  
	float diff = max(dot(v_Normal, lightDir), 0.0);
	vec3 diffuse = diff * vec3(1.5f, 1.5f, 1.5f);

	vec4 texColor = texture(u_Texture, v_TexCoord);

	vec3 result = texColor.xyz * (vec3(0.2f, 0.2f, 0.2f) + diffuse) * vec3(1.0f, 1.0f, 1.0f);
	FragColor = vec4(result, 1.0);


	//FragColor = texColor;
	//FragColor = vec4( 1.0f, 1.0f, 1.0f, 1.0f);
	//FragColor = vec4(v_NormVec, 1.0f);
	//FragColor = vec4(dot(lightPos, pos), dot(lightPos, pos), dot(lightPos, pos), 1.0f);
}