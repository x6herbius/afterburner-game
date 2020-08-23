#pragma once

#include <cstdint>
#include "ElementTraits.h"

namespace NFMDL
{
#pragma pack(push, 1)
	struct Animation
	{
		uint16_t offset[6];
	};
#pragma pack(pop)

	template<>
	struct ElementTraits<Animation>
	{
		static constexpr const char* ELEMENT_NAME = "Animation";
	};
}
