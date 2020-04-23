#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec3 fragPos;
out vec3 normal;
out vec2 texCoords;

uniform mat4 MVP;
uniform mat4 M;

void main()
{
	fragPos = vec3(M * vec4(aPos, 1.0));
    normal = mat3(transpose(inverse(M))) * aNormal;
    texCoords = aTexCoords;

    gl_Position = MVP * vec4(aPos, 1.0);
}