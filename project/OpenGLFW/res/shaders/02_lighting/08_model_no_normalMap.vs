#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;

out vec3 fragPos;
out vec2 texCoords;
out vec3 normal;

uniform mat4 MVP;
uniform mat4 M;

void main()
{
	fragPos = vec3(M * vec4(aPos, 1.0));
    texCoords = aTexCoords;

    mat3 matrixModelToWorld = mat3(transpose(inverse(M)));
    normal =  normalize(matrixModelToWorld * aNormal);

    gl_Position = MVP * vec4(aPos, 1.0);
}