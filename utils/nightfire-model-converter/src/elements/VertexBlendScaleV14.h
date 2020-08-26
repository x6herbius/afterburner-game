#pragma once

#include <cstdint>
#include "ElementTraits.h"

namespace NFMDL
{
#pragma pack(push, 1)
	struct VertexBlendScaleV14
	{
		float scale[4];
	};
#pragma pack(pop)

	template<>
	struct ElementTraits<VertexBlendScaleV14>
	{
		static constexpr const char* ELEMENT_NAME = "VertexBlendScaleV14";
	};
}
