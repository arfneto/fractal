#pragma once
#include	<vector>
#include	<utility>
#include	"Zoom.h"

namespace fractal
{
	class ZoomList
	{
	private:

		int m_width{ 0 };
		int m_height{ 0 };
		std::vector<Zoom> zooms;
		double m_xCenter{ 0 };
		double m_yCenter{ 0 };
		double m_scale{ 1.0 };

	public:

		ZoomList
		(
			int width,
			int height
		);

		void add(const Zoom & zoom);

		std::pair<double, double> doZoom(int x, int y);
	};
}
