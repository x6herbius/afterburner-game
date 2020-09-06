#pragma once

#include <cstdint>
#include "ElementTraits.h"

namespace NFMDL
{
#pragma pack(push, 1)
	struct BodyGroup
	{
		char name[64];
		uint32_t modelCount;
		uint32_t bodyCount;
		uint32_t modelOffset;
	};
#pragma pack(pop)

	template<>
	struct ElementTraits<BodyGroup>
	{
		static constexpr const char* ELEMENT_NAME = "BodyGroup";
	};
}
