#pragma once

#include <cstdint>
#include "ElementTraits.h"

namespace NFMDL
{
#pragma pack(push, 1)
	struct SoundGroupV14
	{
		char name[32];
		uint32_t offset;
	};
#pragma pack(pop)

	template<>
	struct ElementTraits<SoundGroupV14>
	{
		static constexpr const char* ELEMENT_NAME = "SoundGroupV14";
	};
}
