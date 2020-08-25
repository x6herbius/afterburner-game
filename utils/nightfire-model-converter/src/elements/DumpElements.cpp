#include <sstream>
#include <iomanip>
#include <cstdint>
#include "DumpElements.h"
#include "Utils.h"

using namespace NFMDL;

#define AS_HEX(value) "0x" << std::hex << std::setfill('0') << std::setw(8) << (value) << std::dec

static const char* const MotionFlagNames[16] =
{
	"Motion_X",
	"Motion_Y",
	"Motion_Z",
	"Motion_XR",
	"Motion_YR",
	"Motion_ZR",
	"Motion_LX",
	"Motion_LY",
	"Motion_LZ",
	"Motion_AX",
	"Motion_AY",
	"Motion_AZ",
	"Motion_AXR",
	"Motion_AYR",
	"Motion_AZR",
	"Motion_Loop"
};

static const char* const SequenceFlagNames[1] =
{
	"Sequence_Looping"
};

static const char* const LODFlagNames[] =
{
	"LOD_TimesOne",
	"LOD_TimesTwo",
	"LOD_TimesFive",
	"<unknown>",
	"<unknown>",
	"<unknown>",
	"<unknown>",
	"<unknown>",
	"LOD_PlusOne",
	"LOD_PlusTwo",
	"LOD_PlusFour",
};

template<size_t N>
static inline std::string FlagNames(uint32_t flags, const char* const(& flagNames)[N])
{
	std::stringstream stream;
	bool flagAdded = false;

	if ( flags == 0 )
	{
		stream << "None";
	}
	else
	{
		for ( uint32_t index = 0; index < N; ++index )
		{
			if ( !(flags & (1 << index)) )
			{
				continue;
			}

			if ( flagAdded )
			{
				stream << " | ";
			}

			stream << flagNames[index];
			flagAdded = true;
		}
	}

	stream << " " << AS_HEX(flags);
	return stream.str();
}

static inline std::ostream& operator <<(std::ostream& stream, const CountOffsetPair& pair)
{
	stream << "[" << pair.count << (pair.count == 1 ? " element" : " elements") << " @ offset " << pair.offset << "]";
	return stream;
}

static inline std::ostream& operator <<(std::ostream& stream, const Vec3F& vec)
{
	stream << "Vec3F(" << vec[0] << ", " << vec[1] << ", " << vec[2] << ")";
	return stream;
}

static inline std::ostream& operator <<(std::ostream& stream, const BoundingBox& bbox)
{
	stream
		<< "BoundingBox(("
		<< bbox.min[0] << ", " << bbox.min[1] << ", " << bbox.min[2]
		<< ") - ("
		<< bbox.max[0] << ", " << bbox.max[1] << ", " << bbox.max[2]
		<< "))";

	return stream;
}

std::ostream& operator <<(std::ostream& stream, const HeaderV14& header)
{
	const HeaderIdentifier& ident = header.ident;

	stream
		<< ElementTraits<HeaderV14>::ELEMENT_NAME << "\n"
		<< "[\n"
		<< "    Ident: " << ident.ident[0] << ident.ident[1] << ident.ident[2] << ident.ident[3] << "\n"
		<< "    Version: " << header.version << "\n"
		<< "    Name: " << header.name << "\n"
		<< "    Length: " << header.length << "\n"
		<< "    Eye position: " << header.eyePosition << "\n"
		<< "    Movement hull: " << header.movementHull << "\n"
		<< "    Clipping hull: " << header.clippingHull << "\n"
		<< "    Flags: " << AS_HEX(header.flags) << "\n"
		<< "    Bones: " << header.bones << "\n"
		<< "    Bone controllers: " << header.boneControllers << "\n"
		<< "    Hitboxes: " << header.hitBoxes << "\n"
		<< "    Sequences: " << header.sequences << "\n"
		<< "    Sequence groups: " << header.sequenceGroups << "\n"
		<< "    Textures: " << header.textures << "\n"
		<< "    Texture data offset: " << header.textureDataOffset << "\n"
		<< "    Skin reference count: " << header.skinReferenceCount << "\n"
		<< "    Skin family count: " << header.skinFamilyCount << "\n"
		<< "    Skin data offset: " << header.skinDataOffset << "\n"
		<< "    Body groups: " << header.bodyGroups << "\n"
		<< "    Attachments: " << header.attachments << "\n"
		<< "    Sound groups: " << header.soundGroups << "\n"
		<< "    Sounds offset: " << header.soundsOffset << "\n"
		<< "    Transitions: " << header.transitionCount << "\n"
		<< "    Transition flags offset: " << header.transitionFlagsOffset << "\n"
		<< "    Transitions offset: " << header.transitionsOffset << "\n"
		<< "    LOD flags: " << FlagNames(header.lodFlags, LODFlagNames) << "\n"
		<< "    Model count: " << header.modelCount << "\n"
		<< "    Vertex count: " << header.vertexCount << "\n"
		<< "    Triangle count: " << header.triangleCount << "\n"
		<< "    Triangle maps offset: " << header.triangleMapOffset << "\n"
		<< "    Vertices offset: " << header.vertexOffset << "\n"
		<< "    Normals offset: " << header.normalOffset << "\n"
		<< "    Texture co-ords offset: " << header.textureCoOrdOffset << "\n"
		<< "    Blending scales offset: " << header.textureCoOrdOffset << "\n"
		<< "    Bone blends offset: " << header.boneBlendOffset << "\n"
		<< "    Bone fix-ups offset: " << header.boneFixUpOffset << "\n";

		for ( uint32_t index = 0; index < ArraySize(header.modelOffset); ++index )
		{
			if ( header.modelOffset[index] == 0 )
			{
				break;
			}

			stream << "    Model offset [" << index << "]: " << header.modelOffset[index] << "\n";
		}

		stream << "]";

	return stream;
}

std::ostream& operator <<(std::ostream& stream, const NFMDL::Bone& bone)
{
	stream
		<< ElementTraits<Bone>::ELEMENT_NAME << "\n"
		<< "[\n"
		<< "    Name: " << bone.name << "\n"
		<< "    Parent index: " << bone.parentBoneIndex << "\n"
		<< "    Flags: " << AS_HEX(bone.flags) << "\n";

	for ( uint32_t index = 0; index < ArraySize(bone.boneControllerIndices); ++index )
	{
		stream << "    Bone controller index [" << index << "]: " << bone.boneControllerIndices[index] << "\n";
	}

	stream
		<< "    Position: " << bone.position << "\n"
		<< "    Rotation: " << bone.rotation << "\n"
		<< "    Scale position: " << bone.scalePosition << "\n"
		<< "    Scale rotation: " << bone.scaleRotation << "\n"
		<< "]";

	return stream;
}

std::ostream& operator <<(std::ostream& stream, const NFMDL::BoneController& boneController)
{
	stream
		<< ElementTraits<BoneController>::ELEMENT_NAME << "\n"
		<< "[\n"
		<< "    Bone index: " << boneController.boneIndex << "\n"
		<< "    Motion flags: " << FlagNames(boneController.motionFlags, MotionFlagNames) << "\n"
		<< "    Start: " << boneController.start << "\n"
		<< "    End: " << boneController.end << "\n"
		<< "    Rest value: " << boneController.rest << "\n"
		<< "    Controller index: " << boneController.controllerIndex << "\n"
		<< "]";

	return stream;
}

std::ostream& operator <<(std::ostream& stream, const NFMDL::SequenceV14& sequence)
{
	stream
		<< ElementTraits<BoneController>::ELEMENT_NAME << "\n"
		<< "[\n"
		<< "    Name: " << sequence.name << "\n"
		<< "    FPS: " << sequence.fps << "\n"
		<< "    Sequnce flags: " << FlagNames(sequence.sequenceFlags, SequenceFlagNames) << "\n"
		<< "    Activity index: " << sequence.activityIndex << "\n"
		<< "    Activity weight: " << sequence.activityWeight << "\n"
		<< "    Events: " << sequence.events << "\n"
		<< "    Frame count: " << sequence.frameCount << "\n"
		<< "    Foot pivots: " << sequence.footPivots << "\n"
		<< "    Motion flags: " << FlagNames(sequence.motionFlags, MotionFlagNames) << "\n"
		<< "    Motion bone index: " << sequence.motionBoneIndex << "\n"
		<< "    Linear movement: " << sequence.linearMovement << "\n"
		<< "    Auto-move position index: " << sequence.autoMovePositionIndex << "\n"
		<< "    Auto-move angle index: " << sequence.autoMoveAngleIndex << "\n"
		<< "    Bounding box: " << sequence.bbox << "\n"
		<< "    Blend count: " << sequence.blendCount << "\n"
		<< "    Animation data offset: " << sequence.animationDataOffset << "\n";

	for ( uint32_t index = 0; index < ArraySize(sequence.blendMotionFlags); ++index )
	{
		stream << "    Blend motion flags [" << index << "]: " << FlagNames(sequence.blendMotionFlags[index], MotionFlagNames) << "\n";
	}

	for ( uint32_t index = 0; index < ArraySize(sequence.blendStart); ++index )
	{
		stream << "    Blend start [" << index << "]: " << sequence.blendStart[index] << "\n";
	}

	for ( uint32_t index = 0; index < ArraySize(sequence.blendEnd); ++index )
	{
		stream << "    Blend end [" << index << "]: " << sequence.blendEnd[index] << "\n";
	}

	stream
		<< "    Blend parent: " << sequence.blendParent << "\n"
		<< "    Sequence group index: " << sequence.sequenceGroupIndex << "\n"
		<< "    Entry transition node: " << sequence.transitionNodeEntry << "\n"
		<< "    Exit transition node: " << sequence.transitionNodeExit << "\n"
		<< "    Transition node flags: " << AS_HEX(sequence.transitionNodeFlags) << "\n";

	for ( uint32_t index = 0; index < ArraySize(sequence.unknown); ++index )
	{
		stream << "    Unknown [" << index << "]: " << sequence.unknown[index] << "\n";
	}

	stream
		<< "]";

	return stream;
}
