#version 330 core
out vec4 fragColor;

in VS_OUT {
    vec3 worldPos;
} fs_in;

uniform samplerCube environmentMap;

void main() {
	vec3 envColor = texture(environmentMap, fs_in.worldPos).rgb;

	// HDR tonemap and gamma correct
	envColor = envColor / (envColor + vec3(1.0));
	envColor = pow(envColor, vec3(1.0 / 2.2));

	fragColor = vec4(envColor, 1.0);
}
