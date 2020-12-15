#pragma once

#include <glm/glm.hpp>

namespace sh {

// Class for a sample on the surface of the unit sphere
class Sample {
public:
	// Spherical polar coords
	double theta;
	double phi;

	// Cartesian direction
	glm::vec3 direction;

	// Values of each SH function at this point
	double *shValues = nullptr;

	Sample() {}
	~Sample() {
		if (shValues) delete[] shValues;

		shValues = nullptr;
	}
};

//fill an array of samples using jittered stratified sampling
bool generateSamples(int sqrtNumSamples, int numBands, Sample *samples);

}