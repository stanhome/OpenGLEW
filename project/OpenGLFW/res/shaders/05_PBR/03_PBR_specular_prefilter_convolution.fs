#version 330 core
out vec4 fragColor;

in VS_OUT {
    vec3 worldPos;
} fs_in;

uniform samplerCube environmentMap;
uniform float roughness;

const float PI = 3.14159265359;

//------------------------------------------------
// normal distribution
float distributionGGX(vec3 n, vec3 h, float roughness)
{
	float a = roughness * roughness;
	float a2 = a * a;
	float NdotH = max(dot(n, h), 0.0);
	float NdotH2 = NdotH * NdotH;

	float numerator = a2;
	float denominator = (NdotH2 * (a2 - 1.0) + 1.0);
	denominator = PI * denominator * denominator;

	return numerator / denominator;
}

//-----------------------------------------------
//http://holger.dammertz.org/stuff/notes_HammersleyOnHemisphere.html
// efficient VanDerCorpus calculation
float radicalInverse_VdC(uint bits) {
     bits = (bits << 16u) | (bits >> 16u);
     bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
     bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
     bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
     bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
     return float(bits) * 2.3283064365386963e-10; // / 0x100000000
}

vec2 hammersley(uint i, uint n) {
	return vec2(float(i)/float(n), radicalInverse_VdC(i));
}

vec3 importanceSampleGGX(vec2 xi, vec3 n, float roughness) {
	float a = roughness * roughness;
	float a2 = a * a;
	float phi = 2.0 * PI * xi.x;
	float cosTheat = sqrt((1.0 - xi.y) / (1.0 + (a2 - 1.0) * xi.y));
	float sinTheat = sqrt(1.0 - cosTheat * cosTheat);

	// from spherical coordinates to cartesian coordinates - halfway vector
	vec3 h;
	h.x = cos(phi) * sinTheat;
	h.y = sin(phi) * sinTheat;
	h.z = cosTheat;

	// from tangent-space h vector to world-space sample vector
	vec3 up = abs(n.z) < 0.999 ? vec3(0.0, 0.0, 1.0) : vec3(1.0, 0.0, 0.0);
	vec3 tangent = normalize(cross(up, n));
	vec3 bitangent = cross(n, tangent);

	vec3 sampleVec = tangent * h.x + bitangent * h.y + n * h.z;
	return normalize(sampleVec);
}

void main() {
	vec3 n = normalize(fs_in.worldPos);

	// make the simplyfying assumption that V equals R equals the normal
	vec3 r = n;
	vec3 v = r;

	const uint SAMPLE_COUNT = 1024u;
	vec3 prefilteredColor = vec3(0.0);
	float totalWeight = 0.0;

	for(uint i = 0u; i < SAMPLE_COUNT; ++i) {
		// generates a smaple vector that's biased towards the preferred alignment direction(import sampling).
		vec2 xi = hammersley(i, SAMPLE_COUNT);
		vec3 h = importanceSampleGGX(xi, n, roughness);
		vec3 l = normalize(2.0 * dot(v, h) * h - v);

		float NdotL = max(dot(n, l), 0.0);
		if (NdotL > 0.0) {
			// sample from the environment's mip level based on roughness / PDF
			float D = distributionGGX(n, h, roughness);
			float NdotH = max(dot(n, h), 0.0);
			float VdotH = max(dot(v, h), 0.0);
			float pdf = (D * NdotH / (4.0 * VdotH)) + 0.0001;

			float resolution = 512.0f; // resolution of source cubemap(per face)
			float saTexel = 4.0 * PI / (6.0 * resolution * resolution);
			float saSample = 1.0 / (float(SAMPLE_COUNT) * pdf + 0.0001);

			float mipLevel = roughness == 0.0 ? 0.0 : 0.5 * log2(saSample / saTexel);

			prefilteredColor += textureLod(environmentMap, l, mipLevel).rgb * NdotL;
			totalWeight += NdotL;
		}
	}

	prefilteredColor = prefilteredColor / totalWeight;
	fragColor = vec4(prefilteredColor, 1.0);
}
