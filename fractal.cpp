// fractal.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <cstdint>
#include <memory>
#include <fstream>
#include <math.h>
#include "Bitmap.h"
#include "Mandelbrot.h"
#include "ZoomList.h"

using namespace std;
using namespace fractal;

void testCoordinates(Bitmap &bitmap)
{
	int const WIDTH = 800;
	int const HEIGHT = 800;
	int x;
	int y;
	double xF;
	double yF;

	// test some coordinates
	// 0,0
	cout << "\ntesting scaling...\n" << endl;
	x = 0;
	y = 0;
	bitmap.scaleToFractal(x, y, xF, yF);
	cout <<
		"\t(" << x << ", " << y << ") ==> " <<
		"(" << xF << ", " << yF << ")" <<
		endl;

	// 0,HEIGHT-1
	x = 0;
	y = HEIGHT - 1;
	bitmap.scaleToFractal(x, y, xF, yF);
	cout <<
		"\t(" << x << ", " << y << ") ==> " <<
		"(" << xF << ", " << yF << ")" <<
		endl;

	// WIDTH-1,0
	x = WIDTH - 1;
	y = 0;
	bitmap.scaleToFractal(x, y, xF, yF);
	cout <<
		"\t(" << x << ", " << y << ") ==> " <<
		"(" << xF << ", " << yF << ")" <<
		endl;

	// WIDTH-1, height-1
	x = WIDTH - 1;
	y = HEIGHT - 1;
	bitmap.scaleToFractal(x, y, xF, yF);
	cout <<
		"\t(" << x << ", " << y << ") ==> " <<
		"(" << xF << ", " << yF << ")" <<
		endl;
	cout << "\nEnd of scaling tests...\n" << endl;
	return;
}





void dumpHistogram(
	unique_ptr<int[]> &h,	int it,	int lim,	string filename)
{
	//
	ofstream	f;
	long	sum = 0;

	cout << "dumping histogram for " <<
		it << " of " <<
		lim << " MAX iterations at " <<
		filename <<
		endl;

	f.open(filename, ios::out);
	if (!f)
	{
		cout << "could not create " << filename << endl;
		return;
	}
	f << "dumping histogram for a run of " <<
		it << "/" <<
		lim << " MAX iterations at " <<
		filename << "\n";

	// now the data: 
	for (int i = 0; i <= it; i++)
	{
		f << i << "\t\t" << h[i] << "\n";
		sum += h[i];
	}
	f << "\ntotal of " << sum << " values\n";
	f.close();
	return;
};





void testIterate(Bitmap & b, int it)
{
	double xF;
	double yF;
	int H = b.get_height();
	int W = b.get_width();
	uint8_t		color = 0;
	unique_ptr<int[]>		histogram(new int[Mandelbrot::MAX_ITERATIONS - 1]{});
	unique_ptr<int[]>		itCountFor(new int[H*W]{});
	int	notMaxed = 0;
	int Maxed = 0;

	//
	//	1st loop: prepare histogram and iteration's count
	//
	for (int y = 0; y<H; y++)
	{
		for (int x = 0; x < W; x++)
		{
			b.scaleToFractal(x, y, xF, yF);
			int iterations = Mandelbrot::getIterations(xF, yF, it);
			itCountFor[y*W + x] = iterations;
			if (iterations >= it) Maxed += 1;	// this one did not converged
			histogram[iterations] += 1;
		}
	};

	notMaxed = H * W - Maxed;

	for (int y = 0; y<H; y++)
	{
		for (int x = 0; x < W; x++)
		{
			uint8_t red = 0;
			uint8_t green = 0;
			uint8_t blue = 0;

			int iterations = itCountFor[y*W + x];
			if (iterations != it)
			{
				double hue = 0;

				for (int i = 0; i <= iterations; i++)
				{
					hue += (double)(histogram[i]) / notMaxed;
				}
				green = pow(255, hue);
			}
			b.setPixel(x, y, red, green, blue);
		}
	};

	//
	// now dump the histogram to file
	//
	dumpHistogram(histogram, it, Mandelbrot::MAX_ITERATIONS, "histogram.txt");
}	//	end 





int main(
	int argc, 
	char ** argv)
{
	int const WIDTH = 800;
	int const HEIGHT = 800;
	int x;
	int y;
	double xF;
	double yF;

	Bitmap bitmap(WIDTH, HEIGHT);
	ZoomList	zoomList(WIDTH, HEIGHT);
	zoomList.add(Zoom(WIDTH / 2, HEIGHT / 2, 1));

	//bitmap.fillBitmap(255,0,0);
	//bitmap.write("red.bmp");

	//bitmap.fillBitmap(0,255,0);
	//bitmap.write("green.bmp");

	//bitmap.fillBitmap(0,0,255);
	//bitmap.write("blue.bmp");

	//bitmap.fillBitmap(0, 0, 0);
	//bitmap.write("test.bmp");

	//testCoordinates(bitmap);

	testIterate(bitmap, 30);
	bitmap.write("Mandel.bmp");

	return 0;
}

