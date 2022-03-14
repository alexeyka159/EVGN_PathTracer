#version 330 core
layout (points) in;
layout (triangle_strip, max_vertices = 4) out;


in vec4 v_Color[];
out vec4 g_Color;
out vec2 g_UV;

uniform mat4 u_VP;
uniform float u_DotSize;

void main()
{
	vec4 pos = gl_in[0].gl_Position;

	g_Color = v_Color[0];
	
	vec4 center = u_VP * pos;
	vec2 dir = vec2(u_DotSize * .015f);

	g_UV = vec2(0, 0) * 2 - 1;
	gl_Position = vec4( center.x+dir.x, center.y+dir.y, center.z, center.w );
	EmitVertex();

	
	g_UV = vec2(1, 0) * 2 - 1;
	gl_Position = vec4( center.x-dir.x, center.y+dir.y, center.z, center.w );
	EmitVertex();

	
	g_UV = vec2(0, 1) * 2 - 1;
	gl_Position = vec4( center.x+dir.x, center.y-dir.y, center.z, center.w );
	EmitVertex();

	
	g_UV = vec2(1, 1) * 2 - 1;
	gl_Position = vec4( center.x-dir.x, center.y-dir.y, center.z, center.w );
	EmitVertex();

	EndPrimitive();
}