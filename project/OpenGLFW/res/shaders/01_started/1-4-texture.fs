#version 330 core
in vec3 oColor;
in vec2 oTexCoord;

out vec4 FragColor;

uniform sampler2D texture1;

void main()
{
    FragColor = texture(texture1, oTexCoord) * 0.5 + vec4(oColor, 1.0f) * 0.5;
}