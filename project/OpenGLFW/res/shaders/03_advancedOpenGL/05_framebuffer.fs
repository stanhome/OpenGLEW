#version 330 core
in vec2 texCoords;

out vec4 FragColor;

uniform sampler2D screenTexture;

void main()
{
	vec3 texColor = texture(screenTexture, texCoords).rgb;
	// FragColor = vec4(vec3(texColor), 1.0f);
	FragColor = vec4(vec3(1.0 - texColor), 1.0f);
}
