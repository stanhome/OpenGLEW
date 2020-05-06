#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out vec3 normalInWorld;
out vec3 worldPos;

uniform mat4 M;
uniform mat4 MVP;

void main()
{
	normalInWorld = mat3(transpose(inverse(M))) * aNormal;
	worldPos = vec3(M * vec4(aPos, 1.0));
	gl_Position = MVP * vec4(aPos, 1.0);
}