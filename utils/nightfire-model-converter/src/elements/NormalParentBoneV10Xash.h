#pragma once

#include <cstdint>
#include "ElementTraits.h"

namespace NFMDL
{
#pragma pack(push, 1)
	struct NormalParentBoneV10Xash
	{
		uint8_t index;
	};
#pragma pack(pop)

	template<>
	struct ElementTraits<NormalParentBoneV10Xash>
	{
		static constexpr const char* ELEMENT_NAME = "NormalParentBoneV10Xash";
	};
}
