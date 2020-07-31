#version 330 core
out vec4 fragColor;

in VS_OUT {
    vec3 worldPos;
} fs_in;

uniform samplerCube environmentMap;

const float PI = 3.14159265359;

void main() {
	/*
	The world vector acts as the normal of a tangent surface from the origin, aligned to WorldPos.
	Given this normal, calculate all incoming radiance of the environment.
	The result of this radiance is the radiance of light coming from Noraml direction, 
	which is what we use in the PBR shader to sample irradiance.
	*/
	vec3 n = normalize(fs_in.worldPos);
	vec3 irradiance = vec3(0.0);

	// tangent space calculation from origin point.
	vec3 bitangent = vec3(0.0, 1.0, 0.0);
	vec3 tangent = cross(bitangent, n);
	bitangent = cross(n, tangent);

	float sampleDelta = 0.025;
	float nrSamples = 0.0;
	for(float phi = 0.0; phi < 2.0 * PI; phi += sampleDelta)
		for(float theta = 0.0; theta < 0.5 * PI; theta += sampleDelta)
		{
			//spherical to cartesian (in tangent space)
			vec3 tangentSample = vec3(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));
			// tangent sapce to world
			vec3 sampleVec = tangentSample.x * tangent + tangentSample.y * bitangent + tangentSample.z * n;

			irradiance += texture(environmentMap, sampleVec).rgb * cos(theta) * sin(theta);

			nrSamples++;
		}

	irradiance = PI * irradiance * (1.0 / float(nrSamples));
	
	fragColor = vec4(irradiance, 1.0);
}
