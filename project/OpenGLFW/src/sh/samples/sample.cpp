#include "sample.h"
#include "common/log.h"
#include "sh.h"

namespace sh {

const float PI = 3.14159265359;

bool generateSamples(int sqrtNumSamples, int numBands, Sample *samples) {
	int numSamples = sqrtNumSamples * sqrtNumSamples;
	int numFunctions = numBands * numBands;

	// create space for the SH values in each sample
	for (int i = 0; i< numSamples; ++i)
	{
		samples[i].shValues = new double[numFunctions];
		if (!samples[i].shValues) {
			Log::instance()->i("Unable to allocate space for SH values in samples");
			return false;
		}
	}

	int index = 0;
	for (int i = 0; i < sqrtNumSamples; ++i)
	{
		for (int j = 0; j < sqrtNumSamples; ++j)
		{
			Sample &s = samples[index];

			// Generate the position of this sample in [0, 1) x [0, 1)
			double x = (i + ((double)rand() / RAND_MAX)) / sqrtNumSamples;
			double y = (j + ((double)rand() / RAND_MAX)) / sqrtNumSamples;

			//Convert to spherical polars
			double theta = 2.0 * acos(sqrt(1.0 - x));
			double phi = 2.0 * PI * y;

			s.theta = theta;
			s.phi = phi;

			//Convert to cartesians
			s.direction.x = sin(theta) * cos(phi);
			s.direction.y = sin(theta) * sin(phi);
			s.direction.z = cos(theta);

			// compute SH coefficients for this sample
			for (int l = 0; l < numBands; ++l)
			{
				for (int m = -l; m <= l; ++m)
				{
					int shIdx = l * (l + 1) + m;
					s.shValues[shIdx] = SH(l, m, theta, phi);
				}
			}
		}
	}
}

}