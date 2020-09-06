#pragma once

#include <cstdint>
#include "Types.h"

namespace NFMDL
{
#pragma pack(push, 1)
	struct SequenceV10Xash
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
		int32_t blendparent;
		int32_t	sequenceGroupIndex;

		int32_t transitionNodeEntry;
		int32_t transitionNodeExit;
		uint32_t transitionNodeFlags;

		// For auto-advancing sequences
		int32_t nextSequenceIndex;
	};
#pragma pack(pop)
}
