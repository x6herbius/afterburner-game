#pragma once

#include <cstdint>
#include "ElementTraits.h"

namespace NFMDL
{
#pragma pack(push, 1)
	struct BoneBlendV14
	{
		uint8_t boneIndex[4];
	};
#pragma pack(pop)

	template<>
	struct ElementTraits<BoneBlendV14>
	{
		static constexpr const char* ELEMENT_NAME = "BoneBlendV14";
	};
}
