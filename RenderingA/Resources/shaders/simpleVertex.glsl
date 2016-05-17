#version 330
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;
out vec4 vColor;

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;

 
void main()
{
  gl_Position = P * V * M * vec4(position, 1.0);
  vColor = vec4(color, 1.0);
}