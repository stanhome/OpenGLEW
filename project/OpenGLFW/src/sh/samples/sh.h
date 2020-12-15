#pragma once

namespace sh {

// evaluate an Associated Legendre Polynomial(APL) P(l, m) at x
double P(int l, int m, double x);

// calculate the normalization constant for a SH function
double K(int l, int m);

// sample a spherical harmonic basis function y(l, m) at a point on the unit sphere
double SH(int l, int m, double theta, double phi);

// Calculate n!
int factorial(int n);

}