#pragma once

namespace RocketCommander
{
	template <typename T, unsigned int i> inline unsigned int ArraySize(T (&a)[i])
	{
		return sizeof a / sizeof a[0];
	}
}