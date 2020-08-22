#pragma once

#include <cstdint>
#include "Types.h"

namespace NFMDL
{
#pragma pack(push, 1)
	struct Pivot
	{
		Vec3F position;
		int32_t start;
		int32_t end;
	};
#pragma pack(pop)
}
