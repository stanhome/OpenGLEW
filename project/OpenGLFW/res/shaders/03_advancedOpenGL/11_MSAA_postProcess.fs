#version 330 core
out vec4 FragColor;

in VS_OUT {
    vec2 texCoords;
} vs_out;

uniform sampler2D screenTexture;

void main()
{
    vec3 col = texture(screenTexture, vs_out.texCoords).rgb;
    float grayscale = 0.2126 * col.r + 0.7152 * col.g + 0.0722 * col.b;
    FragColor = vec4(vec3(grayscale), 1.0);
}