#pragma once

#include <cstdint>
#include "Types.h"
#include "ElementTraits.h"

namespace NFMDL
{
#pragma pack(push, 1)
	struct VertexV10Xash
	{
		Vec3F position;
	};
#pragma pack(pop)

	template<>
	struct ElementTraits<VertexV10Xash>
	{
		static constexpr const char* ELEMENT_NAME = "VertexV10Xash";
	};
}
