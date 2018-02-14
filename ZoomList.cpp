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
		zooms.push(zoom);
		return;
	}






	int  ZoomList::getNext()
	{
		Zoom zoom(0, 0, 1);
		if (zooms.empty())
		{
			return 0;
		}
		zoom = zooms.front();
		m_xCenter += (zoom.x - m_width / 2) * m_scale;
		m_yCenter += (zoom.y - m_height / 2) * m_scale;
		m_scale *= zoom.scale;
		std::cout << "\tZoom at (" <<
			zoom.x << ", " <<
			zoom.y << "), scale set to " <<
			zoom.scale << ". Calculating..." <<
			std::endl;
		zooms.pop();	// done with this one
		return 1;
	}





	std::queue<Zoom> ZoomList::getZooms()
	{
		return zooms;
	}
	;
}
