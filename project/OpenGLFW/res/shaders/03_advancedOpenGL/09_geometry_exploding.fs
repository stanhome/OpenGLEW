#version 330 core
out vec4 FragColor;

in vec2 texCoords;

uniform sampler2D tex_diffuse0;

void main()
{
    FragColor = texture(tex_diffuse0, texCoords);
}