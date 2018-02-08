#include "stdafx.h"
#include <iostream>
#include "FractalCreator.h"
#include "Mandelbrot.h"

using namespace std;

namespace fractal
{
	fractal::FractalCreator::FractalCreator(int width, int height) :
		m_width(width),
		m_height(height),
		m_histogram(new int[Mandelbrot::MAX_ITERATIONS - 1]{}),
		m_itCountFor(new int[width * height]{}),
		m_bitmap(width, height),
		m_zoomList(width, height)
	{
	// constructor	
		// center is (0,0) scale is 1.0
		m_zoomList.add(Zoom(width / 2, height / 2, 4.0 / width));
		m_notMaxed = 0;
		m_iterations = 100;
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





	void FractalCreator::calculateIteration()
	{
		//
		//	1st loop: prepare histogram and iteration's count
		//
		int Maxed = 0;
		for (int y = 0; y<m_height; y++)
		{
			for (int x = 0; x < m_width; x++)
			{
				pair<double, double> coords = m_zoomList.doZoom(x, y);
				int iterations = Mandelbrot::getIterations(
					coords.first,
					coords.second,
					m_iterations);
				m_itCountFor[y*m_width + x] = iterations;
				if (iterations >= m_iterations) Maxed += 1;	// this one did not converged
				m_histogram[iterations] += 1;
			}
		};

		m_notMaxed = m_height * m_width - Maxed;
	}





	void FractalCreator::clearData()
	{
		for (int i = 0; i <= m_iterations; i++)	m_histogram[i] = 0;
		for (int i = 0; i < (m_width * m_height); i++)			m_itCountFor[i] = 0;
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





	void FractalCreator::drawFractal()
	{
		char filename[40];
		//
		//	it is possible that not a single point converged
		//	so there is nothing to draw. After a deep zoom
		//	data points tends to max out
		//
		if (m_notMaxed != 0)
		{
			for (int y = 0; y < m_height; y++)
			{
				for (int x = 0; x < m_width; x++)
				{
					uint8_t red = 0;
					uint8_t green = 0;
					uint8_t blue = 0;

					int iterations = m_itCountFor[y*m_width + x];
					if (iterations != m_iterations)
					{
						double hue = 0;
						for (int i = 0; i <= iterations; i++)
						{
							hue += (double)(m_histogram[i]) / m_notMaxed;
						}
						green = (uint8_t)(255 * hue);
					}
					m_bitmap.setPixel(x, y, red, green, blue);
				}
			}
		}
		else
		{
			// sets a black image. no need to calculate
			m_bitmap.fillBitmap(0, 0, 0);
		};
		//
		// now dump the histogram to file
		//
		strcpy_s(filename, m_prefix);
		memcpy_s(filename+27, 4, ".txt", 4);
		// yymmdd-hhmmss-nn-Mandelbrot.bmp
		// 0123456789012345678901234567890
		dumpHistogram(filename);
	}	//	end 






	void FractalCreator::dumpHistogram(string filename)
	{
		ofstream	f;
		long	sum = 0;

		cout << "dumping histogram for " <<
			m_iterations << " of " <<
			Mandelbrot::MAX_ITERATIONS << " MAX iterations at " <<
			filename <<
			endl;

		f.open(filename, ios::out);
		if (!f)
		{
			cout << "could not create " << filename << endl;
			return;
		}
		f << "dumping histogram for a run of " <<
			m_iterations << "/" <<
			Mandelbrot::MAX_ITERATIONS << " MAX iterations at " <<
			filename << "\n";

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





	string FractalCreator::getPrefix()
	{
		return m_prefix;
	}
	
	
	
	
	
	void FractalCreator::setFileNamePrefix(char * p)
	{
		strcpy_s(m_prefix, p);
		return;
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
		m_bitmap.write(f);
	}
}