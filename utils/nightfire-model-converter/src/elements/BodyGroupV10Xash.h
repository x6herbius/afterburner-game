#pragma once

#include <cstdint>

namespace NFMDL
{
#pragma pack(push, 1)
	struct BodyGroupV10Xash
	{
		char name[64];
		uint32_t modelCount;
		int32_t base;

		// Index into models array
		uint32_t modelIndex;
	};
#pragma pack(pop)
}
