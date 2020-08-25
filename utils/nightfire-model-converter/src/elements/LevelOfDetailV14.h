#pragma once

#include <cstdint>
#include <cstddef>
#include "Types.h"

namespace NFMDL
{
#pragma pack(push, 1)
	struct LevelOfDetailV14
	{
		uint32_t levels;
		int32_t distance[4];
	};
#pragma pack(pop)

	static inline size_t LevelOfDetailFlagsToCount(uint32_t flags)
	{
		// Unsure whether these are supposed to be separate if-statements, but this
		// is how Ulti/Ford's code worked, so I'm keeping it just in case.

		size_t initial = 0;

		if ( flags & LOD_TimesOne )
		{
			initial = 1;
		}

		if ( flags & LOD_TimesTwo )
		{
			initial = 2;
		}

		size_t output = 0;

		if ( flags & LOD_PlusOne )
		{
			output += 1 * initial;
		}

		if ( flags & LOD_PlusTwo )
		{
			output += 2 * initial;
		}

		if ( flags & LOD_PlusFour )
		{
			output += 4 * initial;
		}

		if ( flags & LOD_TimesFive )
		{
			output *= 5;
		}

		return output;
	}
}
