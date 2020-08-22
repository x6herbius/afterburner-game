#pragma once

#include <cstdint>

namespace NFMDL
{
#pragma pack(push, 1)
	struct LevelOfDetailV14
	{
		uint32_t levels;
		int32_t distance[4];
	};
#pragma pack(pop)
}
