#pragma once

#include <cstdint>
#include "Types.h"

namespace NFMDL
{
#pragma pack(push, 1)
	struct ModelInfoV14
	{
		uint32_t skinReference;
		CountOffsetPair mesh;
	};
#pragma pack(pop)
}
