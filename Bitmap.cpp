#include "stdafx.h"
#include "Bitmap.h"
#include "BitmapFileHeader.h"
#include <string>
#include <fstream>

using namespace std;

namespace fractal
{
	Bitmap::Bitmap(
		int width,
		int height
	):	m_width(width),
		m_height(height),
		m_pPixels(new uint8_t[width*height * 3]{}) // zero filled
	{
		//
	}

	Bitmap::~Bitmap()
	{
	}





	void Bitmap::fillBitmap(
		uint8_t red, 
		uint8_t green, 
		uint8_t blue)
	{
		uint8_t *pPixel = nullptr;

		for	(int y=0;y<m_height;y++)
		{
			for(int x=0;x<m_width;x++)
			{
				pPixel = m_pPixels.get() + (y * 3) * m_width + (x * 3);
				pPixel[2] = red;
				pPixel[1] = green;
				pPixel[0] = blue;
 			}
		}
	}	//	end fillBitmap()






	int Bitmap::get_height()
	{
		return m_height;
	}





	int Bitmap::get_width()
	{
		return m_width;
	}





	void Bitmap::scaleToFractal(
		int x,
		int y,
		double &xFractal,
		double &yFractal)
	{
		xFractal = (x - m_width / 2 - 200) * 2.0 / m_height;
		yFractal = (y - m_height / 2) * 2.0 / m_height;
	}





	void Bitmap::setPixel(int x, int y, uint8_t red, uint8_t green, uint8_t blue)
	{
		uint8_t *pPixel = m_pPixels.get();
		// get address of coordinates of x,y
		pPixel += (y * 3) * m_width + (x * 3);
		pPixel[2] = red;
		pPixel[1] = green;
		pPixel[0] = blue;
	}





	void Bitmap::testScaling(int n)
	{
		double xF, yF;
		int count = 1;
		for (int y = 0; y<m_height; y++)
		{
			for (int x = 0; x<m_width; x++)
			{
				scaleToFractal(x, y, xF, yF);
				// ex: (0,0) ==> (0.0,0.0)
				cout <<
					"(" << x << ", " << y << ") ==> " <<
					"(" << xF << ", " << yF << ")\t" <<
					flush;
				if (count == n)
				{
					cout << endl;
					count = 1;
				}
				else
				{
					count += 1;
				}
			}
			cout << endl;
		}
	}	//	end fillBitmap()





	bool Bitmap::write(string filename)
	{
		ofstream				file;
		BitmapFileHeader		fileHeader;
		BitmapInfoHeader		infoHeader;

		fileHeader.fileSize = 
			sizeof(BitmapFileHeader) +
			sizeof(BitmapInfoHeader) +
			m_width * m_height * 3;

		fileHeader.dataOffset =
			sizeof(BitmapFileHeader) +
			sizeof(BitmapInfoHeader);

		infoHeader.width = m_width;
		infoHeader.height = m_height;

		file.open(filename, ios::out | ios::binary);
		if (!file)
		{
			return false;
		}
		file.write((char *)&fileHeader, sizeof(fileHeader));
		file.write((char *)&infoHeader, sizeof(infoHeader));
		file.write((char *)m_pPixels.get(), m_width*m_height*3);

		file.close();
		if (!file)
		{
			return false;
		}
		return true;
	}
};	//	end namespace