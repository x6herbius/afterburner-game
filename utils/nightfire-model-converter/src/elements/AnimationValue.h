#pragma once

#include <cstdint>
#include "ElementTraits.h"
#include "Types.h"

namespace NFMDL
{
#pragma pack(push, 1)
	struct AnimationValue
	{
		int16_t value;
	};
#pragma pack(pop)

	template<>
	struct ElementTraits<AnimationValue>
	{
		static constexpr const char* ELEMENT_NAME = "AnimationValue";
	};
}
