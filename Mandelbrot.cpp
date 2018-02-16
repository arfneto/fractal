#include "stdafx.h"
#include "Mandelbrot.h"

using namespace std;
namespace fractal
{

	Mandelbrot::Mandelbrot(){}
	Mandelbrot::~Mandelbrot(){}

	int Mandelbrot::getIterations(
		double x, double y,
		int it
	)
	{
		complex<double> z = 0;
		complex<double> c(x,y);

		if (it > MAX_ITERATIONS) it = MAX_ITERATIONS;
		int iterations = 0;
		do
		{
			iterations += 1;
			z = z * z + c;
			if (abs(z) > 2) return iterations; 
		} while (iterations < it);
		return 0;	// MAXED OUT
	}
}