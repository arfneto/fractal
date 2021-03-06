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





int main(
	int argc, 
	char ** argv)
{
	int const WIDTH = 800;
	int const HEIGHT = 800;
	int const W = 800;
	int const H = 800;
	char filename[40] = "yymmdd-hhmmss-nn.Mandelbrot.bmp";
	//         prefix = "0123456789012345678901234567890";
	char * prefix = filename;

	int generation = 0;

	FractalCreator fractalCreator(WIDTH, HEIGHT);
	//
	//	set filename prefix to be used for
	//	bitmap and histogram on disk
	getFileNamePrefix(prefix, ++generation);
	fractalCreator.setFileNamePrefix(prefix);
	fractalCreator.calculateIteration();
	fractalCreator.drawFractal();
	fractalCreator.writeBitmap();
	fractalCreator.clearData();

	// =====================================================
	fractalCreator.addZoom(Zoom(321, 277, 0.1));	// zoom in
	getFileNamePrefix(prefix, ++generation);
	fractalCreator.setFileNamePrefix(prefix);
	fractalCreator.calculateIteration();
	fractalCreator.drawFractal();
	fractalCreator.writeBitmap();
	fractalCreator.clearData();

	// =====================================================
	fractalCreator.addZoom(Zoom(126, 405, 0.1));	// zoom in
	getFileNamePrefix(prefix, ++generation);
	fractalCreator.setFileNamePrefix(prefix);
	fractalCreator.calculateIteration();
	fractalCreator.drawFractal();
	fractalCreator.writeBitmap();
	fractalCreator.clearData();

	// =====================================================
	fractalCreator.addZoom(Zoom(333, 433, 0.1));	// zoom in
	getFileNamePrefix(prefix, ++generation);
	fractalCreator.setFileNamePrefix(prefix);
	fractalCreator.calculateIteration();
	fractalCreator.drawFractal();
	fractalCreator.writeBitmap();
	fractalCreator.clearData();

	// =====================================================
	fractalCreator.addZoom(Zoom(100, 100, 0.1));	// zoom in
	getFileNamePrefix(prefix, ++generation);
	fractalCreator.setFileNamePrefix(prefix);
	fractalCreator.calculateIteration();
	fractalCreator.drawFractal();
	fractalCreator.writeBitmap();
	fractalCreator.clearData();

	return 0;
}

//fractalCreator.testCoordinates();
//fractalCreator.createSolidBitmap("red.bmp", 255, 0, 0);
//fractalCreator.createSolidBitmap("green.bmp", 0, 255, 0);
//fractalCreator.createSolidBitmap("blue.bmp", 0, 0, 255);
//fractalCreator.createSolidBitmap("black.bmp", 0, 0, 0);
//
