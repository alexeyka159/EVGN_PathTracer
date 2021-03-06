#version 460 core

layout (location = 0) out vec4 FragColor;

in vec3 v_LocalPos;
 
uniform sampler2D u_EnvironmentMap;
 
const vec2 invAtan = vec2(0.1591, 0.3183);
vec2 SampleSphericalMap(vec3 v)
{
    vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
    uv *= invAtan;
    uv += 0.5;
    return uv;
}
 
void main()
{
    vec2 uv = SampleSphericalMap(normalize(v_LocalPos));
    vec3 color = textureLod(u_EnvironmentMap, uv, 0).rgb;
    
    FragColor = vec4(color, 1.0);
}