#pragma once

#include <cstdint>
#include "ElementTraits.h"

namespace NFMDL
{
#pragma pack(push, 1)
	struct SequenceGroup
	{
		char name[32];
		char fileName[64];

		// "cache index pointer"
		// Not sure what this means.
		uint32_t cache;

		// "hack for group 0"
		// Not sure what this means.
		int32_t data;
	};
#pragma pack(pop)

	template<>
	struct ElementTraits<SequenceGroup>
	{
		static constexpr const char* ELEMENT_NAME = "SequenceGroup";
	};
}