#include <cstring>
#include <stdexcept>
#include "Conversions.h"
#include "HeaderV10XashFormatTraits.h"

namespace NFMDL
{
	void Convert(const HeaderV14& in, HeaderV10Xash& out)
	{
		memset(&out, 0, sizeof(out));

		// TODO: Sort out how we're going to do textures.
		// Leave them zeroed for the moment.
		// Sounds and transitions don't seem to be referenced at all
		// in the Xash engine, so they're not included here.

		out.ident = HeaderIdentifier(FormatTraits<HeaderV10Xash>::TARGET_IDENTIFIER);
		out.version = FormatTraits<HeaderV10Xash>::TARGET_VERSION;
		strncpy(out.name, in.name, sizeof(out.name));
		out.name[sizeof(out.name) - 1] = '\0';
		out.eyePosition = in.eyePosition;
		out.movementHull = in.movementHull;
		out.clippingHull = in.clippingHull;
		out.flags = in.flags;
		out.bones.count = in.bones.count;
		out.boneControllers.count = in.boneControllers.count;
		out.hitBoxes.count = in.hitBoxes.count;
		out.sequences.count = in.sequences.count;
		out.sequenceGroups.count = in.sequenceGroups.count;
		out.skinReferenceCount = in.skinReferenceCount;
		out.skinFamilyCount = in.skinFamilyCount;
		out.bodyGroups.count = in.bodyGroups.count;
		out.attachments.count = in.attachments.count;
	}

	void Convert(const SequenceV14& in, SequenceV10Xash& out)
	{
		if ( sizeof(in.name) != sizeof(out.name) )
		{
			throw std::runtime_error("V14 and V10 sequence name lengths did not match.");
		}

		memset(&out, 0, sizeof(out));

		memcpy(out.name, in.name, sizeof(out.name));
		out.fps = in.fps;
		out.sequenceFlags = in.sequenceFlags;
		out.activityIndex = in.activityIndex;
		out.activityWeight = in.activityWeight;
		out.events.count = in.events.count;
		out.frameCount = in.frameCount;
		out.footPivots.count = in.footPivots.count;
		out.motionFlags = in.motionFlags;
		out.motionBoneIndex = in.motionBoneIndex;
		out.linearMovement = in.linearMovement;
		out.autoMovePositionIndex = in.autoMovePositionIndex;
		out.autoMoveAngleIndex = in.autoMoveAngleIndex;
		out.bbox = in.bbox;
		out.blendCount = in.blendCount;
		out.animationDataOffset = in.animationDataOffset;
		out.blendMotionFlags[0] = in.blendMotionFlags[0];
		out.blendMotionFlags[1] = in.blendMotionFlags[1];
		out.blendStart[0] = in.blendStart[0];
		out.blendStart[1] = in.blendStart[1];
		out.blendEnd[0] = in.blendEnd[0];
		out.blendEnd[1] = in.blendEnd[1];
		out.blendparent = in.blendParent;
		out.sequenceGroupIndex = in.sequenceGroupIndex;
		out.transitionNodeEntry = in.transitionNodeEntry;
		out.transitionNodeExit = in.transitionNodeExit;
		out.transitionNodeFlags = in.transitionNodeFlags;
	}

	void Convert(const VertexV14& in, VertexV10Xash& out)
	{
		// For now, we ignore scale values - we probably don't need them.
		out.position = in.position;
	}

	void Convert(const NormalV14& in, NormalV10Xash& out)
	{
		// For now, we ignore scale values - we probably don't need them.
		out.position = in.position;
	}
}
