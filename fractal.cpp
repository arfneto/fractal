// fractal.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <time.h>
#include <stdio.h>
#include <iostream>
#include <cstdint>
#include <memory>
#include <fstream>
#include <math.h>
#include <string>
#include <iterator>
#include <queue>
#include "Bitmap.h"
#include "Mandelbrot.h"
#include "Zoom.h"
#include "ZoomList.h"
#include "FractalCreator.h"
#include "fractal.h"
#include "RGB.h"

using namespace std;
using namespace fractal;





// display a few help lines
void usage(char * a)
{
	/*
	Usage:

fractal width height iterations [ (x y scale) (x y scale) ... ]
	
where (x, y, scale) are sets of center points and scale for additional zoom bitmaps

Ex: "fractal 800 600 1000 321 377 0.1 150 150 0.1"

	- will generate 3 sets of bitmap/histogram files
	- The first set
	- A second set with zoom at (321,377) with scale 0.1
 	- A third set with zoom at (150,150) with scale 0.1
 	- bitmaps will be named yymmdd-hhmmss-01-Mandelbrot, yymmdd-hhmmss-02-Mandelbrot, and so on
 	- histogram files will have the same names, but extension txt
	
	*/
	cout <<	"\n\nUsage: \n" << endl;
	cout << "fractal Width Height Iterations [x y scale] [x y scale ]...\n" << endl;
	cout <<
		"where (x, y, scale)... are sets of center points and scale for additional zoom bitmaps\n"
		<< endl;
	cout <<
		"Ex: \"fractal 800 600 1000 321 377 0.1 150 150 0.1\"\n" 
		<< endl;
	cout <<	"\t- will generate 2 sets of bitmap/histogram files" << endl;
	cout << "\t- A first set with zoom at (321, 377) with scale 0.1" << endl;
	cout << "\t- A second set with zoom at (150, 150) with scale 0.1" << endl;
	cout << "\t- bitmaps will be named yymmdd-hhmmss-01-Mandelbrot, yymmdd-hhmmss-02-Mandelbrot, and so on" << endl;
	cout << "\t- histogram files will have the same names, but extension txt\n" << endl;
	return;
}




void abortErrArgs(int err)
{
	cout << "error is " << err << endl;
	exit(1);
}





int main(
	int argc,
	char ** argv)
{

	char filename[40] = "yymmdd-hhmmss-nn.Mandelbrot.bmp";
	//         prefix = "0123456789012345678901234567890";
	char * prefix = filename;
	int generation = 0;
	Zoom		z(0, 0, 1);
	int			err = 0;

	//
	// check for a feasible number of arguments: 7, 10, 13...
	//
	if (!((argc % 3 == 1) && (argc > 4)))
	{
		usage(argv[0]);
		return -1;
	}
	//
	// 200 <= WIDTH <= 4096
	//
	int const WIDTH = atoi(argv[1]);
	if ((WIDTH < 200) || (WIDTH > 4096)) abortErrArgs(++err);
	//
	// 200 <= HEIGHT <= 4096
	//
	int const HEIGHT = atoi(argv[2]);
	if ((HEIGHT < 200) || (HEIGHT > 4096))	abortErrArgs(++err);
	//
	// 50 <= iterations <= Mandelbrot::MAX_ITERATIONS
	//
	int const iterations = atoi(argv[3]);
	if ((iterations<50)||(iterations>Mandelbrot::MAX_ITERATIONS)) 	abortErrArgs(++err);

	//
	// now create the class and the starting default point
	//
	FractalCreator fractalCreator(WIDTH, HEIGHT, iterations);
	// 
	// now the rest of arguments are triples (x,y,scale)
	// can be nothing
	//
	for (int a = 4; a < argc; )
	{
		int x;
		int y;
		double scale;
		// x
		x = atoi(argv[a]);
		if ((x < 0) || (x >= WIDTH))	abortErrArgs(++err);
		// y
		a += 1;
		y = atoi(argv[a]);
		if ((y < 0) || (y >= HEIGHT))	abortErrArgs(++err);
		// scale
		a += 1;
		scale = atof(argv[a]);
		if ((scale <= 0) || (scale > 1))	abortErrArgs(++err);
		a += 1;
		//cout << "Point is " << x << ", " << y << " scale is " << scale << endl;
		fractalCreator.addZoom(Zoom(x, y, scale));
	}
	fractalCreator.run();
	return 0;
}