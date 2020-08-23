#pragma once

#include <cstdint>
#include "Types.h"
#include "ElementTraits.h"

namespace NFMDL
{
#pragma pack(push, 1)
	struct Bone
	{
		char name[32];
		int32_t parentBoneIndex;
		uint32_t flags;

		// -1 = no controller
		int32_t boneControllerIndices[6];

		Vec3F position;
		Vec3F rotation;
		Vec3F scalePosition;
		Vec3F scaleRotation;
	};
#pragma pack(pop)

	template<>
	struct ElementTraits<Bone>
	{
		static constexpr const char* ELEMENT_NAME = "Bone";
	};
}
