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
		m_rangesInUse = 0;

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
		prepareHistogram();
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
				if ((iterations != 0) && (iterations <= m_iterations))
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
		char line[80];
		int i;

		f.open(filename, ios::out);
		if (!f)
		{
			cout << "could not create " << filename << endl;
			return;
		}
		f << "Histogram for a run of " <<
			m_iterations << "/" <<
			Mandelbrot::MAX_ITERATIONS << " MAX iterations" << endl;
		
		f << "\tfilename is " << filename << "\n";
		f << "\tSize is " << m_width << "*" << m_height << endl;
		f << "\tCenter is (" <<
			m_zoomList.m_xCenter <<
			", " <<
			m_zoomList.m_yCenter <<
			") scale is " <<
			m_zoomList.m_scale <<
			endl;
		//
		// write ranges data
		//
		sum = 0;
		for (i = 0; i < m_rangesInUse; i++)
		{
			f << 
				"\tRange " << i << " total is " << m_rangeTotal[i][0] << 
				" from " << m_rangeTotal[i][1] << 
				" to " << m_rangeTotal[i][2] << endl;
			sum += m_rangeTotal[i][0];
		}
		f << "\tGrand Total is: " << 
			sum << " + " <<
			m_histogram[0][0] << " = " << sum + m_histogram[0][0] << endl;
		//
		sprintf_s(line, 70, "\n\tAverage is %+8.2f\n", m_average); f << line;
		//
		sprintf_s(line, 70, "\tStandard Deviation is %+8.2f\n\n",m_deviation); f << line;
		//
		// now dump the histogram data: 
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
		int thisPoint = m_histogram[it][0];
		double d = it - 1;
		double mult;
		RGB		startColor = RGB(0, 0, 0);
				//cout << "iteration:" << it << " of " << m_iterations << " rank is " << rank << endl;
		//
		//  logic:  in this rank we have from rangeTotal[rank][1] to rangeTotal[rank][2] values
		//			present value is at m_histogram[it][0]
		//			we will project this present value between the range limits
		//			note that the range can have only one point and then all three are the same...
		//		
		//		except for the 20 highest points. For these we will go from the end of the range
		//		to the final color in a linear way
		//
		rank = (int)(d / m_iterations * m_rangesInUse);
		if (m_rangeTotal[rank][1] == m_rangeTotal[rank][2])
		{
			// single one in this range
			// put it in the middle of the color range
			mult = 0.5;
		}
		else
		{
			mult = (double)thisPoint / (m_rangeTotal[rank][2] - m_rangeTotal[rank][1]);	// 0..1
		}
		// startColor is black. but if the rank is not 0
		// startColor is the final color of the previous range
		if (rank > 0)
		{
			startColor = m_colorRange[rank - 1];
		}
		// ok, we have a multiplier, but is these one of the 20 highest?
		// it it is, then use final special range
		for (int x = m_iterations; x >= m_iterations - 19; --x)
		{
			if (m_histogram[x][2] == thisPoint)
			{
				if (x == m_iterations)
				{
					// special case max iteration count gets a gray color
					red = green = blue = 210;
				}
				else
				{
					RGB fullRed = RGB(255, 0, 0);
					//cout << "\tfound " << thisPoint << " iterations at " << x << "th position" << endl;
					mult = (x - m_iterations + 20) / 20;
					colorDiff = fullRed - startColor;
					red = (uint8_t)(startColor.r + colorDiff.r * mult);
					green = (uint8_t)(startColor.g + colorDiff.g * mult);
					blue = (uint8_t)(startColor.b + colorDiff.b * mult);
				}
				return;
			}
		}
		//
		// not one of the highest
		//
		if (rank == 0)
		{
			//cout << "starting color for this rank is (" << c.r << "," << c.g << "," << c.b << ")" << endl;
			colorDiff = m_colorRange[0];
			red =	(uint8_t) (colorDiff.r * mult);
			green = (uint8_t) (colorDiff.g * mult);
			blue =  (uint8_t) (colorDiff.b * mult);
		}
		else
		{
			//	if this is one of the 20 highest ranks, then use one of 20 colors between
			//	the final color of the last range and full red
			//cout << "starting color for this rank is (" << c.r << "," << c.g << "," << c.b << ")" << endl;
			colorDiff = m_colorRange[rank] - m_colorRange[rank - 1];
			red =	(uint8_t) (m_colorRange[rank - 1].r + colorDiff.r * mult);
			green = (uint8_t) (m_colorRange[rank - 1].g + colorDiff.g * mult);
			blue =	(uint8_t) (m_colorRange[rank - 1].b + colorDiff.b * mult);
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

		
		
		
	void FractalCreator::prepareHistogram()
	{
		// average
		double sum = 0;
		int ple, le, pri, ri;
		int i;
		int j;
		int t;

		for (i = 1; i <= m_iterations; i++)
		{
			sum += m_histogram[i][0];
		}
		m_average = sum / m_iterations;
		//// now the standard deviation: 
		for (i = 1; i <= m_iterations; i++)
		{
			m_histogramAvg[i] = m_histogram[i][0] - m_average;
			m_histogramAvg[i] *= m_histogramAvg[i]; // square it
		};
		// standard deviation:
		m_deviation = 0;
		for (i = 1; i <= m_iterations; i++)
		{
			m_deviation += m_histogramAvg[i];
		}
		m_deviation /= (double)m_iterations;
		m_deviation = sqrt(m_deviation);
		// now put the ratio of the (iterations - average)/deviation on column 1
		for (i = 1; i <= m_iterations; i++)
		{
			m_histogramAvg[i] = (m_histogram[i][0] - m_average) / m_deviation;
		}
		// now set column 1 with h[i] = i
		for (i = m_iterations; i >= 0; i--)
		{
			m_histogram[i][1] = i;
		};
		// now sort pointers on column 1 based on values at column 0
		for (i = m_iterations; i>1; i--)
		{
			for (j = 1; j<i; j++)
			{
				ple = m_histogram[j][1];
				pri = m_histogram[j + 1][1];
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
	}	// end prepareHistogram()
	
	
	
	

	int FractalCreator::run()
	{
		int generation = 0;
		char filename[40];

		cout << "Generating fractals. This can take a while. Please wait" << endl;
		do
		{
			getFileNamePrefix(m_prefix, ++generation);
			if (!calculateNextIteration()) break;
			setColorRanges();
			// now fill in the histogam and write it down
			strcpy_s(filename, m_prefix);
			strcat_s(filename, ".txt");
			dumpHistogram(filename);

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
		m_rangesInUse = 5;

		m_colorRange[0].r = 128;
		m_colorRange[0].g = 128;
		m_colorRange[0].b = 255;	// purple

		m_colorRange[1].r = 64;
		m_colorRange[1].g = 255;
		m_colorRange[1].b = 255;


		m_colorRange[2].r = 0;
		m_colorRange[2].g = 190;
		m_colorRange[2].b = 64;

		m_colorRange[3].r = 64;
		m_colorRange[3].g = 0;
		m_colorRange[3].b = 160;

		m_colorRange[4].r = 255;
		m_colorRange[4].g = 200;
		m_colorRange[4].b = 0;	

		// now set the running totals for each range
		// in int rangeTotal[]
		// in second column the min iterations in this range
		// in third column the last one in this range
		// this will be used in coloring the bitmap

		for (int i = 0; i < m_rangesInUse; i++)
		{
			m_rangeTotal[i][0] = 0;
			m_rangeTotal[i][1] = 0;
			m_rangeTotal[i][2] = 0;
		};
		bool firstInRank;
		int currentRank = -1;	// we need to know rank breaks

		for (int i = 1; i <= m_iterations; i++)
		{
			int thisOne = m_histogram[m_histogram[i][1]][0];

			double d = i - 1;
			rank = (int)(d / m_iterations * m_rangesInUse);
			firstInRank = (currentRank != rank);
			int count = thisOne;
			if (firstInRank)
			{
				m_rangeTotal[rank][1] = count;	// first in range
				currentRank = rank;
				firstInRank = false;
			}
			m_rangeTotal[rank][2] = count;	// may be the last
			m_rangeTotal[rank][0] += count;	// add it up
			//cout <<	i << ": rank " << rank << " value " << thisOne << endl;
		}
		int sum = 0;
		for (int i = 0; i < m_rangesInUse; i++)
		{
			cout << 
				"\tRange " << i << " total is " << m_rangeTotal[i][0] << 
				" from " << m_rangeTotal[i][1] << 
				" to " << m_rangeTotal[i][2] << endl;
			sum += m_rangeTotal[i][0];
		}
		cout << "\tGrand Total is: " << 
			sum << " + " <<
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