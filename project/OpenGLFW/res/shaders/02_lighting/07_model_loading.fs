#version 330 core
// in vec3 fragPos;
// in vec3 normal;
in vec2 texCoords;

out vec4 FragColor;

uniform sampler2D tex_diffuse0;

void main()
{
    FragColor = texture(tex_diffuse0, texCoords);
}