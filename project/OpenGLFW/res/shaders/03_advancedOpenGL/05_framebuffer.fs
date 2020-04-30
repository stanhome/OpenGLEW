#version 330 core
in vec2 texCoords;

out vec4 FragColor;

uniform sampler2D screenTexture;

void main()
{
	vec3 texColor = texture(screenTexture, texCoords).rgb;
	// FragColor = vec4(vec3(texColor), 1.0f);

	// PostProcess:
	//-------------------
	// 1. inversion
	// FragColor = vec4(vec3(1.0 - texColor), 1.0f);

	// 2. Grayscale
	const vec3 grayFactor = vec3(0.2126, 0.7152, 0.0722);
	float average = dot(texColor, grayFactor);
	FragColor = vec4(average, average, average, 1.0f);
}
