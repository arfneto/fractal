#pragma once
#include	<iostream>
#include	<queue>
#include	<utility>
#include	"Zoom.h"

namespace fractal
{
	class ZoomList
	{
	private:

		int m_width{ 0 };
		int m_height{ 0 };

	public:

		double m_xCenter{ 0 };
		double m_yCenter{ 0 };
		double m_scale{ 1.0 };
		std::queue<Zoom> zooms;

		ZoomList
		(
			int width,
			int height
		);

		void add(const Zoom & zoom);

		int getNext();

		std::queue<Zoom> getZooms();
	};
}
