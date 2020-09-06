#pragma once

#include <cstdint>
#include "Types.h"
#include "ElementTraits.h"
#include "HeaderV10XashFormatTraits.h"

namespace NFMDL
{
#pragma pack(push, 1)
	struct BoneWeightV10Xash
	{
		uint8_t weight[FormatTraits<HeaderV10Xash>::MAX_VERTEX_BONE_BLENDS];
		int8_t boneIndex[FormatTraits<HeaderV10Xash>::MAX_VERTEX_BONE_BLENDS];
	};
#pragma pack(pop)

	template<>
	struct ElementTraits<BoneWeightV10Xash>
	{
		static constexpr const char* ELEMENT_NAME = "BoneWeightV10Xash";
	};
}
