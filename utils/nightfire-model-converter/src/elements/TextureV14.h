#pragma once

#include <cstdint>
#include "ElementTraits.h"

namespace NFMDL
{
#pragma pack(push, 1)
	struct TextureV14
	{
		char materialName[64];
		char textureName[64];
		uint32_t referenceCount;
		uint32_t unknown;
	};
#pragma pack(pop)

	template<>
	struct ElementTraits<TextureV14>
	{
		static constexpr const char* ELEMENT_NAME = "TextureV14";
	};
}
