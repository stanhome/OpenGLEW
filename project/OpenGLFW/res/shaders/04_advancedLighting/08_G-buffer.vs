#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out VS_OUT {
    vec3 fragPos;
    vec3 normal;
    vec2 texCoords;
} vs_out;

uniform mat4 VP;
uniform mat4 M;

void main()
{
	vec4 worldPos = M * vec4(aPos, 1.0);

    vs_out.fragPos = worldPos.xyz;

    mat3 normalMatrix = transpose(inverse(mat3(M)));
    vs_out.normal = normalMatrix * aNormal;

    vs_out.texCoords = aTexCoords;

    gl_Position = VP * worldPos;
}