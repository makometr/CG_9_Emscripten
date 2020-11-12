#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;

out vec3 Normal;
out vec3 FragPos;

uniform mat4 transform;
uniform mat4 model;

void main() {
   gl_Position = transform * vec4(position, 1.0f);
   FragPos = (model * vec4(position, 1.0f)).xyz;
   Normal = mat3(transpose(inverse(model))) * normal;
}