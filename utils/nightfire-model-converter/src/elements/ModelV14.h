#pragma once

#include <cstdint>
#include "HeaderV14FormatTraits.h"

namespace NFMDL
{
#pragma pack(push, 1)
	struct ModelV14
	{
		char name[32];
		uint32_t index;
		uint32_t modelInfoOffset[FormatTraits<HeaderV14>::MAX_MODELS];
	};
#pragma pack(pop)
}
