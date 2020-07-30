#version 330 core
out vec4 fragColor;

in VS_OUT {
    vec3 worldPos;
} fs_in;

uniform sampler2D equirectangularMap0;

const vec2 invAtan = vec2(0.1591, 0.3183);

vec2 sampleSphericalMap(vec3 v) {
	vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
	uv *= invAtan;
	uv += 0.5;
	return uv;
}

void main() {
	vec2 uv = sampleSphericalMap(normalize(fs_in.worldPos));
	vec3 color = texture(equirectangularMap0, uv).rgb;

	fragColor = vec4(color, 1.0);
}
