#pragma once

#include <cstdint>
#include "ElementTraits.h"

namespace NFMDL
{
#pragma pack(push, 1)
	struct BoneController
	{
		// A value of -1 indicates bone 0
		int32_t boneIndex;

		uint32_t motionFlags;
		float start;
		float end;

		// "Byte index value at rest"
		// What this actually means I don't know,
		// but it doesn't seem to be used in Xash.
		int32_t rest;

		// 0-3 user set controller, 4 mouth
		int32_t controllerIndex;
	};
#pragma pack(pop)

	template<>
	struct ElementTraits<BoneController>
	{
		static constexpr const char* ELEMENT_NAME = "BoneController";
	};
}
