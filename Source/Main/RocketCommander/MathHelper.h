#pragma once

namespace RocketCommander
{
	class MathHelper
	{
	public:
		static double Round(double r)
		{
			return (r > 0.0) ? std::floor(r + 0.5) : std::ceil(r - 0.5);
		}
	};
}