// fractal.cpp : Defines the entry point for the console application.
//
#pragma pack(2)

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

using namespace std;
using namespace fractal;





int getFileNamePrefix
(
	char * prefix,
	int gen
)
{
	char timebuf[20];
	struct tm newtime;
	__time64_t long_time;
	errno_t err;
	_time64(&long_time);
	err = _localtime64_s(&newtime, &long_time);
	strcpy_s(timebuf, "yymmdd-hhmmss-nn-");
	if (!err)
	{
		// Convert to a valid prefix an in yymmdd-hhmmss-NN-
		sprintf_s(
			timebuf,
			18,
			"%02d%02d%02d-%02d%02d%02d-%02d-",
			(newtime.tm_year - 100),
			(newtime.tm_mon + 1),
			newtime.tm_mday,
			newtime.tm_hour,
			newtime.tm_min,
			newtime.tm_sec,
			gen
		);
	}
	memcpy_s(prefix, 17, timebuf, 17);
	return 0;
}





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
	cout <<	"\t- will generate 3 sets of bitmap/histogram files" << endl;
	cout << "\t- the first set" << endl;
	cout << "\t- A second set with zoom at (321, 377) with scale 0.1" << endl;
	cout << "\t- A third set with zoom at (150, 150) with scale 0.1" << endl;
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
	// check for a feasible number of arguments: 4, 7, 10, 13...
	//
	if (!((argc % 3 == 1) && (argc > 1)))
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
	fractalCreator.addZoom(Zoom(WIDTH / 2, HEIGHT / 2,.005));

	// 
	// now the rest of arguments are triples (x,y,scale)
	// or nothing
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

	do
	{
		getFileNamePrefix(prefix, ++generation);
		fractalCreator.setFileNamePrefix(prefix);
		if (!fractalCreator.calculateNextIteration()) break;
		if (fractalCreator.drawFractal())
		{
			fractalCreator.writeBitmap();
			fractalCreator.clearData();
		}
		else
		{
			fractalCreator.writeBitmap();
			break;
		};
	} while (true);
	return 0;
}