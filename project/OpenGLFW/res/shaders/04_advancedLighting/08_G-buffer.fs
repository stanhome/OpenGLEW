#version 330 core
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;

in VS_OUT {
    vec3 fragPos;
    vec3 normal;
    vec2 texCoords;
} fs_in;

uniform sampler2D tex_diffuse0;
uniform sampler2D tex_specular0;


void main()
{
	// store the fragment position vector in the first G-buffer texture
	gPosition = fs_in.fragPos;
	// also store the per-fragment normals into the G-buffer
	gNormal = normalize(fs_in.normal);
	// and the diffuse per-fragment color
	gAlbedoSpec.rgb = texture(tex_diffuse0, fs_in.texCoords).rgb;
	// store specular intensity in gAlbedoSpec's alpha component
	gAlbedoSpec.a = texture(tex_specular0, fs_in.texCoords).r;
}