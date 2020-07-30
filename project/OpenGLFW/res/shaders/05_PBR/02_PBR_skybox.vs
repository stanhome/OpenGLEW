#version 330 core
layout (location = 0) in vec3 aPos;

out VS_OUT {
    vec3 worldPos;
} vs_out;

uniform mat4 V;
uniform mat4 P;

void main()
{
    vs_out.worldPos = aPos;

    // remove view translation 
    mat4 rotView = mat4(mat3(V));
    vec4 clipPos = P * rotView * vec4(vs_out.worldPos, 1.0);

    gl_Position = clipPos.xyww;
}