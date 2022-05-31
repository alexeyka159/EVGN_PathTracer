#version 330 core
out vec4 FragColor;

in vec2 v_TexCoord;

uniform sampler2D u_ScreenTexture;

void main()
{
	//FragColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);
	FragColor = vec4(vec3(texture(u_ScreenTexture, v_TexCoord)), 1.0f);
	//FragColor = vec4(vec3(gl_FragCoord.z), 1.0);
}