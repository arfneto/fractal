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





	void Bitmap::setPixel(int x, int y, uint8_t red, uint8_t green, uint8_t blue)
	{
		uint8_t *pPixel = m_pPixels.get();

		// get address of coordinates of x,y
		pPixel += (y * 3) * m_width + (x * 3);

		pPixel[2] = red;
		pPixel[1] = green;
		pPixel[0] = blue;

	}





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
		infoHeader.heigth = m_height;

		file.open(filename, ios::out | ios::binary);
		if (!file)
		{
			return false;
		}
		cout <<
			"Bitmap file " <<
			filename <<
			" is open" <<
			endl;
		file.write((char *)&fileHeader, sizeof(fileHeader));
		file.write((char *)&infoHeader, sizeof(infoHeader));
		file.write((char *)m_pPixels.get(), m_width*m_height*3);

		file.close();
		if (!file)
		{
			return false;
		}
		cout <<
			"Bitmap file " <<
			filename <<
			" is closed" <<
			endl;

		return true;
	}
};	//	end namespace