#include "stdafx.h"
#include <iostream>
#include "FractalCreator.h"
#include "Mandelbrot.h"

using namespace std;

namespace fractal
{
	fractal::FractalCreator::FractalCreator(
		int width, 
		int height, 
		int iterations) :
		m_width(width),
		m_height(height),
		m_iterations(iterations),
		m_histogram(new int[Mandelbrot::MAX_ITERATIONS - 1]{}),
		m_itCountFor(new int[width * height]{}),
		m_bitmap(width, height),
		m_zoomList(width, height)
	{
	// constructor	
		// center is (0,0) scale is 1.0
		m_notMaxed = 0;
		strcpy_s( m_prefix, "yymmdd-hhmmss-nn-Mandelbrot.bmp");
	}

	FractalCreator::~FractalCreator()
	{
		// destructor
	}





	void FractalCreator::addZoom(const Zoom & zoom)
	{
		m_zoomList.add(zoom);
	}





	int FractalCreator::calculateNextIteration()
	{
		double	xFractal;
		double	yFractal;
		int		it;
		int		Maxed = 0;
		//
		//		- prepare histogram
		//		- compute iterations 
		//
		if(!m_zoomList.getNext()) return 0;	// get next coordinates
		for (int y = 0; y<m_height; y++)
		{
			for (int x = 0; x < m_width; x++)
			{
				xFractal = (x - m_width  / 2)* m_zoomList.m_scale + m_zoomList.m_xCenter;
				yFractal = (y - m_height / 2)* m_zoomList.m_scale + m_zoomList.m_yCenter;
				it = Mandelbrot::getIterations(xFractal, yFractal,	m_iterations);
				m_itCountFor[y*m_width + x] = it;
				if (it >= m_iterations) Maxed += 1;	// this one did not converge
				m_histogram[it] += 1;
			}
		};
		m_notMaxed = m_height * m_width - Maxed;
		return 1;
	}





	void FractalCreator::clearData()
	{
		for (int i = 0; i <= m_iterations; i++)	m_histogram[i] = 0;
		return;
	}




	void FractalCreator::createSolidBitmap
	(
		string filename,
		int red,
		int green,
		int blue
	)
	{
		m_bitmap.fillBitmap(red, green, blue);
		m_bitmap.write(filename);
		return;
	}





	int FractalCreator::drawFractal()
	{
		char filename[40];

		uint8_t red;
		uint8_t green;
		uint8_t blue;

		RGB		startColor(0, 0, 20);
		RGB		endColor(255, 128, 74);
		RGB		colorDiff = endColor - startColor;

		//
		//	it is possible that not a single point converged
		//	so there is nothing to draw. After a deep zoom
		//	data points tends to max out. in this case we
		//  just set a black display and return. Just for 
		//  make it clear, generate a histogram file with
		//  the data, all zeroes..
		//

		if (m_notMaxed == 0)
		{
			m_bitmap.fillBitmap(0, 0, 0);
			strcpy_s(filename, m_prefix);
			strcat_s(filename, ".txt");
			dumpHistogram(filename);
			return 0;
		}
		//	normal flow... there is some data
		for (int y = 0; y < m_height; y++)
		{
			for (int x = 0; x < m_width; x++)
			{
				int iterations = m_itCountFor[y*m_width + x];
				if (iterations != m_iterations)
				{
					double hue = 0;
					for (int i = 0; i <= iterations; i++)
					{
						hue += (double)(m_histogram[i]) / m_notMaxed;
					}
					red =	(uint8_t)(startColor.r + colorDiff.r * hue);
					green = (uint8_t)(startColor.g + colorDiff.g * hue);
					blue =	(uint8_t)(startColor.b + colorDiff.b * hue);
				}
				else
				{
					red = 0;
					green = 0;
					blue = 0;
				}
				m_bitmap.setPixel(x, y, red, green, blue);
			}
		}
		//
		// now dump the histogram to file
		//
		// yymmdd-hhmmss-nn-Mandelbrot.bmp
		// 0123456789012345678901234567890
		strcpy_s(filename, m_prefix);
		strcat_s(filename, ".txt");
		dumpHistogram(filename);
		return 1;
	}	//	end 






	void FractalCreator::dumpHistogram(string const filename)
	{
		ofstream	f;
		long	sum = 0;

		//cout << "dumpHistogram():\n\t\tdumping histogram for " <<
		//	m_iterations << " of " <<
		//	Mandelbrot::MAX_ITERATIONS << " MAX iterations\n\t\tat " <<
		//	filename <<
		//	endl;

		f.open(filename, ios::out);
		if (!f)
		{
			cout << "could not create " << filename << endl;
			return;
		}
		f << "Histogram for a run of " <<
			m_iterations << "/" <<
			Mandelbrot::MAX_ITERATIONS << " MAX iterations" << endl;
		
		f << "    filename is " << filename << "\n";
		f << "    Size is " << m_width << "*" << m_height << endl;
		f << "    Center is (" <<
			m_zoomList.m_xCenter <<
			", " <<
			m_zoomList.m_yCenter <<
			") scale is " <<
			m_zoomList.m_scale <<
			endl;

		// now the data: 
		for (int i = 0; i <= m_iterations; i++)
		{
			f << i << "\t\t" << m_histogram[i] << "\n";
			sum += m_histogram[i];
		}
		f << "\ntotal of " << sum << " values\n";
		f.close();
		return;

	};





	int FractalCreator::getFileNamePrefix
	(
		char * prefix,
		const int gen
	)
	{
		char timebuf[30];
		struct tm newtime;
		__time64_t long_time;
		errno_t err;
		_time64(&long_time);
		err = _localtime64_s(&newtime, &long_time);
		strcpy_s(timebuf, "yymmdd-hhmmss-nn-Mandelbrot");
		if (!err)
		{	// Convert to a valid prefix an in yymmdd-hhmmss-NN-
			sprintf_s(
				timebuf, 32, "%02d%02d%02d-%02d%02d%02d-%02d-Mandelbrot",
				(newtime.tm_year - 100), (newtime.tm_mon + 1), newtime.tm_mday, 
				newtime.tm_hour, newtime.tm_min,	newtime.tm_sec,	gen
			);
		}
		memcpy_s(prefix, 32, timebuf, 32);
		return 0;
	}





	string FractalCreator::getPrefix()
	{
		return m_prefix;
	}

		
		
		
		
	int FractalCreator::run()
	{
		int generation = 0;
		cout << "Generating fractals. This can take a while. Please wait" << endl;
		do
		{
			getFileNamePrefix(m_prefix, ++generation);
			if (!calculateNextIteration()) break;
			if (drawFractal())
			{
				writeBitmap();
				clearData();
			}
			else
			{
				writeBitmap();
				break;
			};
			cout << getPrefix() << " generated. Looking for more zoom points. Please wait" << endl;
		} while (true);
		cout << "No more points. Ended" << endl;
		return 0;
	}





	void FractalCreator::testCoordinates()
	{
		int x;
		int y;
		double xF;
		double yF;

		// test some coordinates
		// 0,0
		std::cout << "\ntesting scaling...\n" << endl;
		x = 0;
		y = 0;
		m_bitmap.scaleToFractal(x, y, xF, yF);
		cout <<
			"\t(" << x << ", " << y << ") ==> " <<
			"(" << xF << ", " << yF << ")" <<
			endl;

		// 0,HEIGHT-1
		x = 0;
		y = m_height - 1;
		m_bitmap.scaleToFractal(x, y, xF, yF);

		cout <<
			"\t(" << x << ", " << y << ") ==> " <<
			"(" << xF << ", " << yF << ")" <<
			endl;

		// WIDTH-1,0
		x = m_width - 1;
		y = 0;
		m_bitmap.scaleToFractal(x, y, xF, yF);
		cout <<
			"\t(" << x << ", " << y << ") ==> " <<
			"(" << xF << ", " << yF << ")" <<
			endl;

		// WIDTH-1, height-1
		x = m_width - 1;
		y = m_height - 1;
		m_bitmap.scaleToFractal(x, y, xF, yF);
		cout <<
			"\t(" << x << ", " << y << ") ==> " <<
			"(" << xF << ", " << yF << ")" <<
			endl;
		cout << "\nEnd of scaling tests...\n" << endl;
		return;
	}





	void FractalCreator::writeBitmap()
	{
		string f = m_prefix;
		f.append(".bmp");
		m_bitmap.write(f);
	}
}