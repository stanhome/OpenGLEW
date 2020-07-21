#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;
layout (location = 2) in vec3 aNormal;

out VS_OUT {
    vec3 worldPos;
    vec3 normal;
    vec2 texCoords;
} vs_out;

uniform mat4 M;
uniform mat4 VP;

void main()
{
    vs_out.worldPos = vec3(M * vec4(aPos, 1.0));

    mat3 normalMatrixM = transpose(inverse(mat3(M)));
    vs_out.normal = normalMatrixM * aNormal;
    vs_out.texCoords = aTexCoords;

    gl_Position = VP * vec4(vs_out.worldPos, 1.0);
}