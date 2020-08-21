#pragma once

#include <cstdint>

namespace NFMDL
{
#pragma pack(push, 1)
	struct BodyGroupV14
	{
		char name[64];
		uint32_t modelCount;
		uint32_t bodyCount;
		uint32_t modelOffset;
	};
#pragma pack(pop)
}
