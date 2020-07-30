#version 330 core
layout (location = 0) in vec3 aPos;

out VS_OUT {
    vec3 worldPos;
} vs_out;

uniform mat4 VP;

void main()
{
    vs_out.worldPos = aPos;

    gl_Position = VP * vec4(vs_out.worldPos, 1.0);
}