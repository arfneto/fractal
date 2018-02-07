#include "stdafx.h"
#include "Mandelbrot.h"

using namespace std;
namespace fractal
{

	Mandelbrot::Mandelbrot()
	{
	}


	Mandelbrot::~Mandelbrot()
	{
	}

	int Mandelbrot::getIterations(
		double x,
		double y,
		int it
	)
	{
		complex<double> z = 0;
		complex<double> c(x,y);

		if (it > MAX_ITERATIONS) it = MAX_ITERATIONS;

		int iterations = 0;
		while (iterations < it)
		{
			z = z * z + c;
			if (abs(z) > 2)
			{
				break;
			}
			iterations += 1;
		}
		return iterations;
	}
}