#pragma once

#include <cstdint>
#include "ElementTraits.h"

namespace NFMDL
{
#pragma pack(push, 1)
	struct TriangleMapV14
	{
		uint16_t vertexIndex;
	};
#pragma pack(pop)

	template<>
	struct ElementTraits<TriangleMapV14>
	{
		static constexpr const char* ELEMENT_NAME = "TriangleMapV14";
	};
}
