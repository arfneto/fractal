#include "stdafx.h"
#include <iostream>
#include "FractalCreator.h"

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
		m_histogram(new int[Mandelbrot::MAX_ITERATIONS + 1][3]{}),
		m_histogramAvg(new double[Mandelbrot::MAX_ITERATIONS + 1]{}),
		m_itCountFor(new int[width * height]{}),
		m_bitmap(width, height),
		m_zoomList(width, height)
	{
	// constructor	
		// center is (0,0) scale is 1.0
		m_notMaxed = 0;
		strcpy_s( m_prefix, "yymmdd-hhmmss-nn-Mandelbrot.bmp");
		rangesInUse = 0;

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
		char filename[40];
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
				m_itCountFor[y*m_width + x] = it;		// maxed out points return 0
				m_histogram[it][0] += 1;	// in column zero we have the MAXed out items
			}
		};
		m_notMaxed = m_height * m_width - (int) m_histogram[0][0];
		// now fill in the histogam and write it down
		strcpy_s(filename, m_prefix);
		strcat_s(filename, ".txt");
		dumpHistogram(filename);
		return 1;
	}





	void FractalCreator::clearData()
	{
		for (int i = 0; i <= m_iterations; i++)	m_histogram[i][0] = 0;
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
		//  make it clear to the user, we still generate
		//  a histogram file with the data, all zeroes..
		//

		if (m_notMaxed == 0)
		{
			m_bitmap.fillBitmap(0, 0, 0);
			return 0;
		}
		//	normal flow... there is some data
		for (int y = 0; y < m_height; y++)
		{
			for (int x = 0; x < m_width; x++)
			{
				int iterations = m_itCountFor[y*m_width + x];
				// maxed out?
				if (iterations != m_iterations)
				{
					// draw this one. its position in the range of iterations is at
					// column 1 of the histogram
					RGB c;
					double hue = 0;
					getColorForPixel(iterations, red, green, blue);
				}
				else
				{
					red = 0; green = 0;	blue = 0;
				}
				m_bitmap.setPixel(x, y, red, green, blue);
			}
		}
		return 1;
	}	//	end 






	void FractalCreator::dumpHistogram(string const filename)
	{
		ofstream	f;
		double	sum;
		double average;
		double deviation;
		int i;
		int j;
		int t;
		int le, ri, ple, pri;
		char line[80];

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
		//
		// now get the values into the histogram
		//
		// average:
		sum = 0;
		for (i = 1; i <= m_iterations; i++)
		{
				sum += m_histogram[i][0];
		}
		average = sum / m_iterations;
		//// now the standard deviation: 
		for (i = 1; i <= m_iterations; i++)
		{
			m_histogramAvg[i] = m_histogram[i][0] - average;
			m_histogramAvg[i] *= m_histogramAvg[i]; // square it
		};
		// standard deviation:
		deviation = 0;
		for (i = 1; i <= m_iterations; i++)
		{
			deviation += m_histogramAvg[i];
		}
		deviation /= (double)m_iterations;
		deviation = sqrt(deviation);
		// now put the ratio of the (iterations - average)/deviation on column 1
		for (i = 1; i <= m_iterations; i++)
		{
			m_histogramAvg[i] = (m_histogram[i][0] - average) / deviation;
		}
		// now set column 1 with h[i] = i
		for (i = m_iterations; i >=0; i--)
		{
			m_histogram[i][1] = i;
		};
		// now sort pointers on column 1 based on values at column 0
		for (i = m_iterations; i>1; i--)
		{
			for (j = 1; j<i; j++)
			{
				ple = m_histogram[j][1];
				pri = m_histogram[j+1][1];
				le = m_histogram[ple][0];
				ri = m_histogram[pri][0];
				if (le > ri)
				{
					t = m_histogram[j][1];
					m_histogram[j][1] = m_histogram[j + 1][1];
					m_histogram[j + 1][1] = t;
				}
			};
		};
		// now points column 2 to ordered range using the pointers at column 1
		for (i = 1; i <= m_iterations; i++)
		{
			m_histogram[i][2] = m_histogram[m_histogram[i][1]][0];
		};
		//
		// now dump the data: 
		sum = 0;
		for (i = 0; i <= m_iterations; i++)
		{
			sprintf_s(
				line, 78, "\t%05d\t%10d\t%+8.4f%10d\t%10d\t%+8.4f\n",
				i,
				m_histogram[i][0],
				m_histogramAvg[i],
				m_histogram[i][1],
				m_histogram[i][2],
				m_histogramAvg[m_histogram[i][1]]
			);
			f << line;
			sum += m_histogram[i][0];
		}
		sprintf_s(line, 70, "\n\ttotal of %8.0f values\n", sum);
		f << line;
		sprintf_s(line, 70, "\tAverage is %8.2f\n", average); f << line;
		sprintf_s(line, 70, "\tStandard Deviation is %+8.2f\n", deviation); f << line;
		f.close();
		return;

	};




	void	FractalCreator::getColorForPixel(
		const int it, 
		uint8_t & red,
		uint8_t & green,
		uint8_t & blue)
	{
		RGB		c = RGB(0, 0, 0);
		RGB		colorDiff(0, 0, 0);

		int rank;
		double d = it - 1;
		rank = (int)(d / m_iterations * rangesInUse);
		//cout << "iteration:" << it << " of " << m_iterations << " rank is " << rank << endl;
		double mult = (double)m_histogram[it][0] / rangeTotal[rank];
		if (rank == 0)
		{
			//cout << "starting color for this rank is (" << c.r << "," << c.g << "," << c.b << ")" << endl;
			colorDiff = colorRange[0];
			red =	(uint8_t) (colorDiff.r * mult);
			green = (uint8_t) (colorDiff.g * mult);
			blue =  (uint8_t) (colorDiff.b * mult);
		}
		else
		{
			c = colorRange[rank - 1];
			//cout << "starting color for this rank is (" << c.r << "," << c.g << "," << c.b << ")" << endl;
			colorDiff = colorRange[rank] - colorRange[rank - 1];
			red =	(uint8_t) (colorRange[rank - 1].r + colorDiff.r * mult);
			green = (uint8_t) (colorRange[rank - 1].g + colorDiff.g * mult);
			blue =	(uint8_t) (colorRange[rank - 1].b + colorDiff.b * mult);
		}
		//cout << "color for pixel is (" << (int)red << "," << (int)green << "," << (int)blue << ")" << endl;
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
			setColorRanges();
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





	void FractalCreator::setColorRanges()
	{
		int rank;
		rangesInUse = 4;

		//colorRange[0].r = 128;
		//colorRange[0].g = 128;
		//colorRange[0].b = 128;	// gray

		colorRange[0].r = 255;
		colorRange[0].g = 255;
		colorRange[0].b = 128;	// yellow

		colorRange[1].r = 128;
		colorRange[1].g = 255;
		colorRange[1].b = 128;	// somewhat green

		colorRange[2].r = 128;
		colorRange[2].g = 128;
		colorRange[2].b = 255;	// somewhat blue

		colorRange[3].r = 255;
		colorRange[3].g = 0;
		colorRange[3].b = 0;	// red

		// now set the running totals for each range
		// in int rangeTotal[]

		for (int i = 0; i < rangesInUse; i++)
		{
			rangeTotal[i] = 0;
		};

		for (int i = 1; i <= m_iterations; i++)
		{
			double d = i - 1;
			rank = (int)(d / m_iterations * rangesInUse);
			int count = m_histogram[m_histogram[i][1]][0];
			rangeTotal[rank] += count;
			//cout <<	i << ": rank is " << rank << " value is " << count << endl;
		}
		int sum = 0;
		for (int i = 0; i < rangesInUse; i++)
		{
			cout << i << ": range total is " << rangeTotal[i] << endl;
			sum += rangeTotal[i];
		}
		cout << "Grand Total is: " << sum << " + " <<
			m_histogram[0][0] << " = " << sum + m_histogram[0][0] << endl;
	};





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