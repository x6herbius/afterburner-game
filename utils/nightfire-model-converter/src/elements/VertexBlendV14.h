#pragma once

#include <cstdint>
#include "ElementTraits.h"
#include "HeaderV14FormatTraits.h"

namespace NFMDL
{
#pragma pack(push, 1)
	struct VertexBlendV14
	{
		int8_t boneIndex[FormatTraits<HeaderV14>::MAX_VERTEX_BONE_BLENDS];
	};
#pragma pack(pop)

	template<>
	struct ElementTraits<VertexBlendV14>
	{
		static constexpr const char* ELEMENT_NAME = "VertexBlendV14";
	};
}
