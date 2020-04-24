#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;

out vec3 fragPos;
out vec2 texCoords;
out mat3 TBN; // the matrix which convert TBN space to world space

uniform mat4 MVP;
uniform mat4 M;

void main()
{
	fragPos = vec3(M * vec4(aPos, 1.0));
    texCoords = aTexCoords;

    mat3 matrixModelToWorld = mat3(transpose(inverse(M)));
    vec3 N =  normalize(matrixModelToWorld * aNormal);
    vec3 T = normalize(matrixModelToWorld * aTangent);
    // vec3 B = cross(N, T) * aTangent.w; // w 决定副法线的方向，我们这里没有，就直接使用副法线
    vec3 B = normalize(matrixModelToWorld * aBitangent);

    TBN = mat3(T, B, N);

    gl_Position = MVP * vec4(aPos, 1.0);
}