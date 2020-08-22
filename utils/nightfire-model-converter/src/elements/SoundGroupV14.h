#pragma once

#include <cstdint>

namespace NFMDL
{
#pragma pack(push, 1)
	struct SoundGroupV14
	{
		char name[32];
		uint32_t offset;
	};
#pragma pack(pop)
}
