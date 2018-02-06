// fractal.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include "Bitmap.h"
using namespace std;
using namespace fractal;


int main(
	int argc, 
	char ** argv)
{
	int const WIDTH = 800;
	int const HEIGHT = 600;

	Bitmap bitmap(WIDTH, HEIGHT);

	bitmap.setPixel(
		WIDTH / 2, HEIGHT / 2,
		255, 255, 255
	);

	bitmap.write("test.bmp");

	return 0;
}

