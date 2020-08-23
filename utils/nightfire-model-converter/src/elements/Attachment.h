#pragma once

#include <cstdint>
#include "Types.h"
#include "ElementTraits.h"

namespace NFMDL
{
#pragma pack(push, 1)
	struct Attachment
	{
		char name[32];
		uint32_t type;
		int32_t boneIndex;
		Vec3F origin;

		// Not 100% sure what these are for
		Vec3F vectors[3];
	};
#pragma pack(pop)

	template<>
	struct ElementTraits<Attachment>
	{
		static constexpr const char* ELEMENT_NAME = "Attachment";
	};
}
