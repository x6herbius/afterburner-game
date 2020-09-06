#pragma once

#include <cstdint>
#include "ElementTraits.h"

namespace NFMDL
{
#pragma pack(push, 1)
	struct VertexParentBoneV10Xash
	{
		uint8_t index;
	};
#pragma pack(pop)

	template<>
	struct ElementTraits<VertexParentBoneV10Xash>
	{
		static constexpr const char* ELEMENT_NAME = "VertexParentBoneV10Xash";
	};
}
