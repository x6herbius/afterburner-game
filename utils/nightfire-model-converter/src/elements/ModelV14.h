#pragma once

#include <cstdint>
#include "HeaderV14FormatTraits.h"
#include "ElementTraits.h"

namespace NFMDL
{
#pragma pack(push, 1)
	struct ModelV14
	{
		char name[32];
		uint32_t index;
		uint32_t modelInfoOffset[24];
	};
#pragma pack(pop)

	template<>
	struct ElementTraits<ModelV14>
	{
		static constexpr const char* ELEMENT_NAME = "ModelV14";
	};
}
