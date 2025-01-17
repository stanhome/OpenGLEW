#version 330 core
in vec2 texCoords;

out vec4 FragColor;

uniform sampler2D screenTexture;

const float offset = 1.0 / 300.0;

void main()
{
	vec2 offset[9] = vec2[] (
		vec2(-offset,  offset), // top-left
		vec2( 0.0f,    offset), // top-center
		vec2( offset,  offset), // top-right
		vec2(-offset,  0.0f),   // center-left
		vec2( 0.0f,    0.0f),   // center-center
		vec2( offset,  0.0f),   // center-right
		vec2(-offset, -offset), // bottom-left
		vec2( 0.0f,   -offset), // bottom-center
		vec2( offset, -offset)  // bottom-right  
	);

	// particular sharpen
	// float kernel[9] = float[](
    //     -1, -1, -1,
    //     -1,  9, -1,
    //     -1, -1, -1
	// );

	// Blur
	// float kernel[9] = float[](
	// 	1.0 / 16, 2.0 / 16, 1.0 / 16,
	// 	2.0 / 16, 4.0 / 16, 2.0 / 16,
	// 	1.0 / 16, 2.0 / 16, 1.0 / 16  
	// );

	float kernel[9] = float[](
		1, 1, 1,
		1, -8, 1,
		1, 1, 1  
	);

	vec3 sampleTex[9];
	for(int i = 0; i < 9; i++) {
		sampleTex[i] = vec3(texture(screenTexture, texCoords.st + offset[i]));
	}

	vec3 col = vec3(0.0);
	for(int i = 0; i < 9; i++)
		col += sampleTex[i] * kernel[i];

	FragColor = vec4(col, 1.0);
}
