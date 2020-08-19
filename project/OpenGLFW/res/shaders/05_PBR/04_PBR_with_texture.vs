#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;

out VS_OUT {
    vec3 worldPos;
    vec2 texCoords;
    mat3 TBN;
} vs_out;

uniform mat4 M;
uniform mat4 VP;

void main()
{
    vs_out.worldPos = vec3(M * vec4(aPos, 1.0));

    mat3 normalMatrixModelToWorld = transpose(inverse(mat3(M)));
    vec3 N = normalize(normalMatrixModelToWorld * aNormal);
    vec3 T = normalize(normalMatrixModelToWorld * aTangent);
    vec3 B = cross(N, T);
    vs_out.TBN = mat3(T, B, N);
    vs_out.texCoords = aTexCoords;

    gl_Position = VP * vec4(vs_out.worldPos, 1.0);
}