#pragma once

#include <cstdint>
#include "HeaderV14FormatTraits.h"

namespace NFMDL
{
#pragma pack(push, 1)
	struct SoundV14
	{
		uint32_t unknown;
		char names[FormatTraits<HeaderV14>::MAX_NAMES_PER_SOUND][64];
	};
#pragma pack(pop)
}
