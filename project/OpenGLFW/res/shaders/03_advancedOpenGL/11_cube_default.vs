#version 330 core
layout (location = 0) in vec3 aPos;
// (location = 1) is normal
// layout (location = 2) in vec2 aTexCoords;

// out VS_OUT {
//     vec2 texCoords;
// } vs_out;

uniform mat4 MVP;

void main()
{
	// vs_out.texCoords = aTexCoords;
	gl_Position = MVP * vec4(aPos, 1.0);
}