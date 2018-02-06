#pragma once
#include <iostream>
#include <cstdint>
#include <string>
#include <memory>
#include "BitmapFileHeader.h"

using namespace std;

namespace fractal
{
	class Bitmap
	{
	private:
		int m_width		{0};
		int m_height	{0};
		unique_ptr<uint8_t[]> m_pPixels{ nullptr };

	public:
		Bitmap
		(
			int width,
			int heigth
		);

		virtual ~Bitmap();

		void setPixel(
			int x,
			int y,
			uint8_t red,
			uint8_t green,
			uint8_t blue
			);

		bool write(string filename);

	};	// end class Bitmap
}	//	end namespace fractal