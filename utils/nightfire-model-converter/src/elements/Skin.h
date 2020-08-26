#pragma once

#include <cstdint>
#include "ElementTraits.h"

namespace NFMDL
{
#pragma pack(push, 1)
	struct Skin
	{
		int16_t value;
	};
#pragma pack(pop)

	template<>
	struct ElementTraits<Skin>
	{
		static constexpr const char* ELEMENT_NAME = "Skin";
	};
}
