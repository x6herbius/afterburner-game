#pragma once

#include <cstddef>
#include <cstdint>
#include "Types.h"

namespace NFMDL
{
	struct HeaderV10Xash;

	template<>
	struct FormatTraits<HeaderV10Xash>
	{
		static constexpr size_t MAX_NAME_LENGTH = 64;
		static constexpr const char* const TARGET_IDENTIFIER = MDL_IDENTIFIER_HALFLIFE;
		static constexpr uint32_t TARGET_VERSION = 10;
		static constexpr size_t MAX_VERTEX_BONE_BLENDS = 4;
	};
}
