#pragma once

#include <iostream>
#include <string>
#include <fstream>
#include <memory>
#include <cstdint>
#include <iomanip>
#include "Bitmap.h"
#include "time.h"
#include "Mandelbrot.h"
#include "Zoomlist.h"
#include "RGB.h"


using namespace std;

namespace fractal
{
	class FractalCreator
	{
	private:
		int						m_width;
		int						m_height;
		unique_ptr<int[][3]>	m_histogram;
		unique_ptr<double[]>	m_histogramAvg;
		unique_ptr<int[]>		m_itCountFor;
		Bitmap					m_bitmap;
		ZoomList				m_zoomList;
		int						m_notMaxed;
		int						m_iterations;
		char					m_prefix[40];
		int						m_rangesInUse;
		RGB						m_colorRange[10];		// colors 
		int						m_rangeTotal[10][3];	// total first last
		double					m_average;
		double					m_deviation;

	int calculateNextIteration();
	
	void clearData();
	
	int drawFractal();

	void dumpHistogram
	(
		string const filename
	);

	int getFileNamePrefix
	(
		char * prefix,
		const int gen
	);
	
	void prepareHistogram();

	void writeBitmap();


	public:

		FractalCreator
		(
			int width, 
			int height,
			int iterations
		);

		~FractalCreator();

		void addZoom(const Zoom & zoom);

		void createSolidBitmap(
			string filename,  
			int red, 
			int green, 
			int blue);

		void	getColorForPixel(
			const int it,
			uint8_t & red,
			uint8_t & green,
			uint8_t & blue
		);

		string getPrefix();

		int run();

		void setColorRanges();

		void testCoordinates();
	};
}
