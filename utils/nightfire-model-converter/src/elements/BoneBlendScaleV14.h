#pragma once

#include <cstdint>
#include "ElementTraits.h"

namespace NFMDL
{
#pragma pack(push, 1)
	struct BoneBlendScaleV14
	{
		float scale[4];
	};
#pragma pack(pop)

	template<>
	struct ElementTraits<BoneBlendScaleV14>
	{
		static constexpr const char* ELEMENT_NAME = "BoneBlendScaleV14";
	};
}
