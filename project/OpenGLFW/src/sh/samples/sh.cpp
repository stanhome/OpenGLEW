#pragma once

#include "sh.h"

#include <glm/glm.hpp>

namespace sh {

const float PI = 3.14159265359;

double P(int l, int m, double x) {
	//First generate the value of P(m, m) at x
	double pmm = 1.0;

	// use rule 2
	if (m > 0)
	{
		double sqrtOneMinusX2 = sqrt(1.0 - x*x);

		double fact = 1.0;

		for (int i = 1; i <= m; ++i)
		{
			pmm *= (-fact)*sqrtOneMinusX2;
			fact += 2.0;
		}
	}

	//If l==m, P(l, m)==P(m, m)
	if (l == m)
		return pmm;

	//Use rule 3 to calculate P(m+1, m) from P(m, m)
	double pmp1m = x*(2.0*m + 1.0)*pmm;

	//If l==m+1, P(l, m)==P(m+1, m)
	if (l == m + 1)
		return pmp1m;

	//Iterate rule 1 to get the result
	//Otherwise, l>m+1.
	double plm = 0.0;

	for (int i = m + 2; i <= l; ++i)
	{
		plm = ((2.0*i - 1.0)*x*pmp1m - (i + m - 1.0)*pmm) / (i - m);
		pmm = pmp1m;
		pmp1m = plm;
	}

	return plm;
}

// calculate the normalisation constant for a SH function
//No need to use |m| since SH always passes positive m
double K(int l, int m) {
	double tmp = ((2.0*l + 1.0)*factorial(l - m)) / ((4.0 * PI)*factorial(l + m));

	return sqrt(tmp);
}

// sample a spherical harmonic basis function y(l, m) at a point on the unit sphere
double SH(int l, int m, double theta, double phi) {
	const double sqrt2 = sqrt(2.0);
	m = abs(m);

	double k = K(l, m);
	double p = P(l, m, cos(theta));

	if (m == 0) return k * p;
	else if (m > 0) return sqrt2 * k * cos(m * phi) * p;
	else return sqrt2 * k * sin(m * phi) * p;
}

// Calculate n!
int factorial(int n) {
	if (n <= 1) return 1;

	int ret = n;

	while(--n > 1) ret *= n;

	return ret;
}

}