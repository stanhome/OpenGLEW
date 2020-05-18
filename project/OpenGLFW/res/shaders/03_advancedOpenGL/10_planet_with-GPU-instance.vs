#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in mat4 instanceMatrixM;

out vec2 texCoords;

uniform mat4 VP;

void main()
{
	texCoords = aTexCoords;
	gl_Position = VP * instanceMatrixM * vec4(aPos, 1.0);
}