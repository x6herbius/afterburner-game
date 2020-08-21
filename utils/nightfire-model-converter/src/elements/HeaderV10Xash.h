#pragma once

#include <cstdint>
#include <cstddef>
#include "Types.h"

namespace NFMDL
{
	struct HeaderV10Xash;

	template<>
	struct FormatTraits<HeaderV10Xash>
	{
		static constexpr size_t MAX_NAME_LENGTH = 64;
		static constexpr HeaderIdentifier TARGET_IDENTIFIER = MDL_IDENTIFIER;
		static constexpr uint32_t TARGET_VERSION = 10;
	};

#pragma pack(push, 1)
	struct HeaderV10Xash
	{
		HeaderIdentifier ident;
		uint32_t version;

		char name[FormatTraits<HeaderV10Xash>::MAX_NAME_LENGTH];
		int32_t length;

		Vec3F eyePosition;
		BoundingBox movementHull;
		BoundingBox clippingHull;

		uint32_t flags;

		CountOffsetPair bones;
		CountOffsetPair boneControllers;
		CountOffsetPair hitBoxes;
		CountOffsetPair sequences;
		CountOffsetPair sequenceGroups;
		CountOffsetPair textures;
		uint32_t textureInfoOffset;
		uint32_t skinReferenceCount;
		uint32_t skinFamilyCount;
		uint32_t skinOffset;
		CountOffsetPair bodyParts;
		CountOffsetPair attachments;
		uint32_t studiohdr2Offset;
		uint32_t soundsOffset;
		CountOffsetPair soundGroups;
		CountOffsetPair transitions;
	};
#pragma pack(pop)
}
