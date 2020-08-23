#pragma once

#include <cstdint>
#include "Types.h"
#include "ElementTraits.h"

namespace NFMDL
{
#pragma pack(push, 1)
	struct SequenceV14
	{
		char name[32];
		float fps;
		uint32_t sequenceFlags;

		int32_t activityIndex;
		int32_t activityWeight;
		CountOffsetPair events;
		uint32_t frameCount;
		CountOffsetPair footPivots;

		uint32_t motionFlags;
		int32_t motionBoneIndex;
		Vec3F linearMovement;
		int32_t autoMovePositionIndex;
		int32_t autoMoveAngleIndex;

		BoundingBox bbox;

		uint32_t blendCount;

		// Relative to start of sequence group data
		uint32_t animationDataOffset;

		uint32_t blendMotionFlags[2];
		float blendStart[2];
		float blendEnd[2];
		int32_t blendParent;
		int32_t	sequenceGroupIndex;

		int32_t transitionNodeEntry;
		int32_t transitionNodeExit;
		uint32_t transitionNodeFlags;

		int32_t unknown[4];
	};
#pragma pack(pop)

	template<>
	struct ElementTraits<SequenceV14>
	{
		static constexpr const char* ELEMENT_NAME = "SequenceV14";
	};
}
