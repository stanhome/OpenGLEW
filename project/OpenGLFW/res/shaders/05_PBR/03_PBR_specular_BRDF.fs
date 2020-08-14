#version 330 core
out vec2 fragColor;
in vec2 texCoords;

const float PI = 3.14159265359;
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
	float cosTheat = sqrt(1.0 - xi.y) / (1.0 + (a2 - 1.0) * xi.y);
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

float geometrySchlickGGX(float NdotV, float roughness) {
	// note that we use a different k for IBL
	float r = roughness * roughness;
	float k = (r * r) / 2.0;

	float numerator = NdotV;
	float denominator = NdotV * (1 - k) + k;

	return numerator / denominator;
}

float geometrySmith(vec3 n, vec3 v, vec3 l, float roughness)
{
	float NdotV = max(dot(n, v), 0.0);
	float NdotL = max(dot(n, l), 0.0);
	float ggx1 = geometrySchlickGGX(NdotV, roughness);
	float ggx2 = geometrySchlickGGX(NdotL, roughness);

	return ggx1 * ggx2;
}

vec2 integrateBRDF(float NdotV, float roughness) {
	vec3 v;
	v.x = sqrt(1.0 - NdotV * NdotV);
	v.y = 0.0;
	v.z = NdotV;

	float A = 0.0;
	float B = 0.0;

	vec3 n = vec3(0.0, 0.0, 1.0);

	const uint SAMPLE_COUNT = 1024u;
	for	(uint i = 0u; i < SAMPLE_COUNT; ++i) {
		//generates a sample vector that's biased towards the preferred alignment direction(importance sampling).
		vec2 xi = hammersley(i, SAMPLE_COUNT);
		vec3 h = importanceSampleGGX(xi, n, roughness);
		vec3 l = normalize(2.0 * dot(v, h) * h - v);

		float NdotL = max(l.z, 0.0);
		float NdotH = max(h.z, 0.0);
		float VdotH = max(dot(v, h), 0.0);

		if (NdotL > 0.0) {
			float G = geometrySmith(n, v, l, roughness);
			float G_Vis = (G * VdotH) / (NdotH * NdotV);
			float Fc = pow(1.0 - VdotH, 5.0);

			A += (1.0 - Fc) * G_Vis;
			B += Fc * G_Vis;
		}
	}

	A /= float(SAMPLE_COUNT);
	B /= float(SAMPLE_COUNT);

	return vec2(A, B);
}

void main() {
	vec2 integratedVal = integrateBRDF(texCoords.x, texCoords.y);
	fragColor = integratedVal;
}
