#pragma once

#include <cstdint>
#include "Types.h"
#include "ElementTraits.h"

namespace NFMDL
{
#pragma pack(push, 1)
	struct TextureCoOrdinateV14
	{
		Vec2F coOrds;
	};
#pragma pack(pop)

	template<>
	struct ElementTraits<TextureCoOrdinateV14>
	{
		static constexpr const char* ELEMENT_NAME = "TextureCoOrdinateV14";
	};
}
