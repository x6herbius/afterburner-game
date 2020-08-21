#pragma once

#include <cstdint>

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
}
