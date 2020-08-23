#pragma once

#include <cstdint>
#include <cstddef>
#include "Types.h"
#include "HeaderV14FormatTraits.h"
#include "ElementTraits.h"

namespace NFMDL
{
#pragma pack(push, 1)
	struct HeaderV14
	{
		HeaderIdentifier ident;
		uint32_t version;

		char name[FormatTraits<HeaderV14>::MAX_NAME_LENGTH];
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
		uint32_t skinDataOffset;
		CountOffsetPair bodyGroups;
		CountOffsetPair attachments;
		CountOffsetPair soundGroups;
		uint32_t soundsOffset;
		uint32_t transitionCount;
		uint32_t transitionFlagsOffset;
		uint32_t transitionsOffset;
		uint32_t lodFlags;
		uint32_t modelCount;
		uint32_t vertexCount;
		uint32_t triangleCount;
		uint32_t triangleMapOffset;
		uint32_t vertexOffset;
		uint32_t normalOffset;
		uint32_t textureCoOrdOffset;
		uint32_t blendingScaleOffset;
		uint32_t boneBlendOffset;
		uint32_t boneFixUpOffset;
		uint32_t modelOffset[FormatTraits<HeaderV14>::MAX_MODELS];
	};
#pragma pack(pop)

	template<>
	struct ElementTraits<HeaderV14>
	{
		static constexpr const char* ELEMENT_NAME = "HeaderV14";
	};
}
