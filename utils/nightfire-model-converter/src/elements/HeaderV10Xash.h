#pragma once

#include <cstdint>
#include <cstddef>
#include "Types.h"
#include "HeaderV10XashFormatTraits.h"
#include "elements/ElementTraits.h"

namespace NFMDL
{
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
		uint32_t textureDataOffset;
		uint32_t skinReferenceCount;
		uint32_t skinFamilyCount;
		uint32_t skinOffset;
		CountOffsetPair bodyGroups;
		CountOffsetPair attachments;
		uint32_t studiohdr2Offset;
		uint32_t soundsOffset;
		CountOffsetPair soundGroups;
		CountOffsetPair transitions;
	};
#pragma pack(pop)

	template<>
	struct ElementTraits<HeaderV10Xash>
	{
		static constexpr const char* ELEMENT_NAME = "HeaderV10Xash";
	};
}
