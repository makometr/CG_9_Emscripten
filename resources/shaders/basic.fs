#version 330 core

in vec3 fColor;

out vec4 FragColor;

uniform vec3 lightColor;

void main()
{
   FragColor = vec4(lightColor * fColor, 1.0f);
}