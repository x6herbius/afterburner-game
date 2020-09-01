#pragma once

#include <cstdint>
#include "ElementTraits.h"
#include "Types.h"

namespace NFMDL
{
#pragma pack(push, 1)
	struct AnimationValueMeta
	{
		uint8_t valid;
		uint8_t total;
	};
#pragma pack(pop)

	template<>
	struct ElementTraits<AnimationValueMeta>
	{
		static constexpr const char* ELEMENT_NAME = "AnimationValueMeta";
	};
}
