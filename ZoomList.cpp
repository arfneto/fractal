#include "stdafx.h"
#include "ZoomList.h"

namespace fractal
{
	ZoomList::ZoomList
	(
		int width,
		int height
	):	m_width(width),
		m_height(height)
	{
		std::queue<Zoom> zooms;
	};





	void ZoomList::add(const Zoom & zoom)
	{
		//
		zooms.push(zoom);
		return;
	}

	

	std::pair<double, double> ZoomList::doZoom
	(
		int x,
		int y
	)
	{
		double xFractal = (x - m_width / 2)*m_scale + m_xCenter;
		double yFractal = (y - m_height / 2)*m_scale + m_yCenter;

		return std::pair<double, double>(xFractal, yFractal);
	}





	int  ZoomList::getNext()
	{
		Zoom zoom(0, 0, 1);
		if (zooms.empty())
		{
			return 0;
		}
		zoom = zooms.front();

		/*
		{
			std::cout << 
				"new coordinates being computed for " <<
				" center: (" <<
				zoom.x << ", " <<
				zoom.y << ") scale " <<
				zoom.scale <<
				std::endl;
		}
		*/
		m_xCenter += (zoom.x - m_width / 2) * m_scale;
		m_yCenter += (zoom.y - m_height / 2) * m_scale;
		m_scale *= zoom.scale;
		zooms.pop();	// done with this one
		return 1;
	}





	std::queue<Zoom> ZoomList::getZooms()
	{
		return zooms;
	}
	;
}
