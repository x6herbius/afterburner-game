#pragma once

#include <cstdint>
#include "ElementTraits.h"

namespace NFMDL
{
#pragma pack(push, 1)
	struct TextureCoOrdinateV14
	{
		float u;
		float v;
	};
#pragma pack(pop)

	template<>
	struct ElementTraits<TextureCoOrdinateV14>
	{
		static constexpr const char* ELEMENT_NAME = "TextureCoOrdinateV14";
	};
}
