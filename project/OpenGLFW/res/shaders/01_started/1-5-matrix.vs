#version 330 core
layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 texCoord;

// out vec3 oColor;
out vec2 oTexCoord;

uniform mat4 MVP;

void main()
{
    gl_Position = MVP * vec4(pos, 1.0);
    // oColor = color;
    oTexCoord = texCoord;
}