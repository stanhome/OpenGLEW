#version 330 core
out vec4 FragColor;

in VS_OUT {
    vec2 texCoords;
} vs_out;

uniform sampler2D tex_diffuse0;

void main()
{
    FragColor = texture(tex_diffuse0, vs_out.texCoords);
}