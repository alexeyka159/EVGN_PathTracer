#version 330 core
out vec4 FragColor;

in vec4 g_Color;
in vec2 g_UV;

void main()
{
  vec4 color = g_Color;
  vec2 uv = g_UV;
  float distance = 1 - length(uv);
  distance = step(0, distance);
  color.a *= distance;

  FragColor = color;
}