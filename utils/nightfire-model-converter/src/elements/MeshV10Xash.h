#pragma once

#include <cstdint>
#include "Types.h"
#include "ElementTraits.h"

namespace NFMDL
{
#pragma pack(push, 1)
	struct MeshV10Xash
	{
		CountOffsetPair triangles;
		uint32_t skinref;
		CountOffsetPair normals;
	};
#pragma pack(pop)

	template<>
	struct ElementTraits<MeshV10Xash>
	{
		static constexpr const char* ELEMENT_NAME = "MeshV10Xash";
	};
}
