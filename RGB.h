#pragma once
namespace fractal
{
	struct RGB
	{
		double r;
		double g;
		double b;

		RGB	(
			double r,
			double g,
			double b
		);
	};

	RGB operator -	(
		const RGB & first,
		const RGB & second
			);

	RGB operator +	(
		const RGB & first,
		const RGB & second
		);
}
