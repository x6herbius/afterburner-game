#pragma once

#include <cstdint>
#include "Types.h"
#include "ElementTraits.h"

namespace NFMDL
{
#pragma pack(push, 1)
	struct HitBox
	{
		int32_t parentBoneIndex;
		int32_t hitGroupIndex;
		BoundingBox bbox;
	};
#pragma pack(pop)

	template<>
	struct ElementTraits<HitBox>
	{
		static constexpr const char* ELEMENT_NAME = "HitBox";
	};
}
