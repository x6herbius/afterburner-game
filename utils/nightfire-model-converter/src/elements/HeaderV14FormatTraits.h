#pragma once

#include <cstddef>
#include <cstdint>
#include "Types.h"

namespace NFMDL
{
	struct HeaderV14;

	template<>
	struct FormatTraits<HeaderV14>
	{
		static constexpr size_t MAX_NAME_LENGTH = 64;
		static constexpr size_t MAX_MODELS = 48;
		static constexpr size_t MAX_NAMES_PER_SOUND = 10;
		static constexpr const char* const TARGET_IDENTIFIER = MDL_IDENTIFIER_NIGHTFIRE;
		static constexpr uint32_t TARGET_VERSION = 14;
		static constexpr size_t NUM_ANIMATION_COMPONENTS = 6;
		static constexpr size_t MAX_VERTEX_BONE_BLENDS = 4;
	};
}
