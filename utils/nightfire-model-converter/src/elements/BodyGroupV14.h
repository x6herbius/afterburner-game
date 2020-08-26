#pragma once

#include <cstdint>
#include "ElementTraits.h"

namespace NFMDL
{
#pragma pack(push, 1)
	struct BodyGroupV14
	{
		char name[64];
		uint32_t modelCount;
		uint32_t bodyCount;
		uint32_t modelOffset;
	};
#pragma pack(pop)

	template<>
	struct ElementTraits<BodyGroupV14>
	{
		static constexpr const char* ELEMENT_NAME = "BodyGroupV14";
	};
}
