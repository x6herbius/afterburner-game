#pragma once

#include <cstdint>
#include "Types.h"
#include "ElementTraits.h"

namespace NFMDL
{
#pragma pack(push, 1)
	struct ModelInfoV14
	{
		uint32_t skinReference;
		CountOffsetPair meshes;
	};
#pragma pack(pop)

	template<>
	struct ElementTraits<ModelInfoV14>
	{
		static constexpr const char* ELEMENT_NAME = "ModelInfoV14";
	};
}
