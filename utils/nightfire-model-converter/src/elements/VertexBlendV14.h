#pragma once

#include <cstdint>
#include "ElementTraits.h"

namespace NFMDL
{
#pragma pack(push, 1)
	struct VertexBlendV14
	{
		int8_t boneIndex[4];
	};
#pragma pack(pop)

	template<>
	struct ElementTraits<VertexBlendV14>
	{
		static constexpr const char* ELEMENT_NAME = "VertexBlendV14";
	};
}
