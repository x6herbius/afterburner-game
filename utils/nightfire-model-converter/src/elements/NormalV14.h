#pragma once

#include <cstdint>
#include "Types.h"
#include "ElementTraits.h"

namespace NFMDL
{
#pragma pack(push, 1)
	struct NormalV14
	{
		Vec3F position;
		float scale;
	};
#pragma pack(pop)

	template<>
	struct ElementTraits<NormalV14>
	{
		static constexpr const char* ELEMENT_NAME = "NormalV14";
	};
}
