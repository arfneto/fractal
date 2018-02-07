#pragma once

#include <complex>

using namespace std;

namespace fractal
{
	class Mandelbrot
	{
	public:

		static const int MAX_ITERATIONS = 10000;

	public:
		Mandelbrot();
		~Mandelbrot();

		static int getIterations(
			double x,
			double y, 
			int it
		);
	};
}