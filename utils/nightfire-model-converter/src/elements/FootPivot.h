#pragma once

#include <cstdint>
#include "Types.h"
#include "ElementTraits.h"

namespace NFMDL
{
#pragma pack(push, 1)
	struct FootPivot
	{
		Vec3F position;
		int32_t start;
		int32_t end;
	};
#pragma pack(pop)

	template<>
	struct ElementTraits<FootPivot>
	{
		static constexpr const char* ELEMENT_NAME = "FootPivot";
	};
}
