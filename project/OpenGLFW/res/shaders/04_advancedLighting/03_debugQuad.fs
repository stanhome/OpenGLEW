#version 330 core
layout (location = 0) out vec4 fragColor;

in vec2 texCoords;

uniform sampler2D depthMap;
uniform float nearPlane;
uniform float farPlane;

// required when using a perspective projection matrix
float linearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0; // Back to NDC 
    return (2.0 * nearPlane * farPlane) / (farPlane + nearPlane - z * (farPlane - nearPlane));	
}

void main()
{
	float depthVal = texture(depthMap, texCoords).r;
	// fragColor = vec4(vec3(linearizeDepth(depthVal) / farPlane), 1.0); // perspective
	fragColor = vec4(vec3(depthVal), 1.0); // orthographic
}