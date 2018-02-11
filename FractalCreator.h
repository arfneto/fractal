#pragma once
#pragma pack(2)

#include <iostream>
#include <string>
#include <fstream>
#include <memory>
#include <cstdint>
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
		unique_ptr<int[]>		m_histogram;
		unique_ptr<int[]>		m_itCountFor;
		Bitmap					m_bitmap;
		ZoomList				m_zoomList;
		int						m_notMaxed;
		int						m_iterations;
		char					m_prefix[40];

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

		string getPrefix();

		int run();

		void testCoordinates();
	};
}
