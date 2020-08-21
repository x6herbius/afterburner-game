#pragma once

#include <cstdint>
#include "Types.h"

namespace NFMDL
{
#pragma pack(push, 1)
	struct HitBox
	{
		int32_t parentBoneIndex;
		int32_t hitGroupIndex;
		BoundingBox bbox;
	};
#pragma pack(pop)
}
