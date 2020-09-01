#pragma once

#include <cstdint>
#include "ElementTraits.h"
#include "HeaderV14.h"
#include "HeaderV14FormatTraits.h"

namespace NFMDL
{
#pragma pack(push, 1)
	struct BoneAnimationDataOffsets
	{
		uint16_t dataOffsetForComponent[FormatTraits<HeaderV14>::NUM_ANIMATION_COMPONENTS];
	};
#pragma pack(pop)

	template<>
	struct ElementTraits<BoneAnimationDataOffsets>
	{
		static constexpr const char* ELEMENT_NAME = "BoneAnimationDataOffsets";
	};
}
