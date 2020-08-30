#pragma once

#include <cstdint>
#include "HeaderV14FormatTraits.h"
#include "ElementTraits.h"

namespace NFMDL
{
#pragma pack(push, 1)
	struct SoundV14
	{
		uint32_t nameCount;
		char names[FormatTraits<HeaderV14>::MAX_NAMES_PER_SOUND][64];
	};
#pragma pack(pop)

	template<>
	struct ElementTraits<SoundV14>
	{
		static constexpr const char* ELEMENT_NAME = "SoundV14";
	};
}
