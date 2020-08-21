#pragma once

#include <cstdint>

namespace NFMDL
{
#pragma pack(push, 1)
	struct TextureV10Xash
	{
		char name[64];
		uint32_t flags;
		uint32_t width;
		uint32_t height;
		uint32_t index;
	};
#pragma pack(pop)
}
