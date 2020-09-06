#include <sstream>
#include <iomanip>
#include <cstdint>
#include "DumpElements.h"
#include "Utils.h"
#include "LevelOfDetailV14.h"

using namespace NFMDL;

#define AS_HEX(value) "0x" << std::hex << std::setfill('0') << std::setw(8) << (value) << std::dec
#define CLASS_INFO(className) ElementTraits<className>::ELEMENT_NAME << " (" << sizeof(className) << " bytes)"

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

static const char* const LODFlagNames[11] =
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
	static_assert(N <= 32, "Number of flags must fit into a 32-bit integer.");

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

static inline std::ostream& WriteOwnedItemKey(std::ostream& stream,
											  const OwnedItemKey& key,
											  const char* owner,
											  const char* item)
{
	stream
		<< "[" << owner << " index " << key.ownerIndex
		<< ", " << item << " index " << key.itemIndex << "]";

	return stream;
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
		<< CLASS_INFO(HeaderV14) << "\n"
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
		<< "    LOD flags: " << FlagNames(header.lodFlags, LODFlagNames) << " (" << LevelOfDetailFlagsToCount(header.lodFlags) << " levels)" << "\n"
		<< "    Model count: " << header.modelCount << "\n"
		<< "    Vertex count: " << header.vertexCount << "\n"
		<< "    Triangle count: " << header.triangleCount << "\n"
		<< "    Triangle maps offset: " << header.triangleMapOffset << "\n"
		<< "    Vertices offset: " << header.vertexOffset << "\n"
		<< "    Normals offset: " << header.normalOffset << "\n"
		<< "    Texture co-ords offset: " << header.textureCoOrdOffset << "\n"
		<< "    Blending scales offset: " << header.textureCoOrdOffset << "\n"
		<< "    Vertex blends offset: " << header.vertexBlendOffset << "\n"
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
		<< CLASS_INFO(Bone) << "\n"
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
		<< CLASS_INFO(BoneController) << "\n"
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
		<< CLASS_INFO(SequenceV14) << "\n"
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

std::ostream& operator <<(std::ostream& stream, const NFMDL::HitBox& hitbox)
{
	stream
		<< CLASS_INFO(HitBox) << "\n"
		<< "[\n"
		<< "    Parent bone index: " << hitbox.parentBoneIndex << "\n"
		<< "    Hit group index: " << hitbox.hitGroupIndex << "\n"
		<< "    Bounding box: " << hitbox.bbox << "\n"
		<< "]";

	return stream;
}

std::ostream& operator <<(std::ostream& stream, const NFMDL::SequenceGroup& sequenceGroup)
{
	stream
		<< CLASS_INFO(SequenceGroup) << "\n"
		<< "[\n"
		<< "    Name: " << sequenceGroup.name << "\n"
		<< "    File name: " << sequenceGroup.fileName << "\n"
		<< "    Cache: " << AS_HEX(sequenceGroup.cache) << "\n"
		<< "    Data: " << AS_HEX(sequenceGroup.data) << "\n"
		<< "]";

	return stream;
}

std::ostream& operator <<(std::ostream& stream, const NFMDL::TextureV14& texture)
{
	stream
		<< CLASS_INFO(TextureV14) << "\n"
		<< "[\n"
		<< "    Material name: " << texture.materialName << "\n"
		<< "    Texture name: " << texture.textureName << "\n"
		<< "    Reference count: " << texture.referenceCount << "\n"
		<< "    Unknown: " << texture.unknown << "\n"
		<< "]";

	return stream;
}

std::ostream& operator <<(std::ostream& stream, const NFMDL::Attachment& attachment)
{
	stream
		<< CLASS_INFO(Attachment) << "\n"
		<< "[\n"
		<< "    Name: " << attachment.name << "\n"
		<< "    Type: " << attachment.type << "\n"
		<< "    Parent bone index: " << attachment.type << "\n"
		<< "    Origin: " << attachment.origin << "\n";

	for ( uint32_t index = 0; index < ArraySize(attachment.vectors); ++index )
	{
		stream << "    Vector [" << index << "]: " << attachment.vectors[index] << "\n";
	}

	stream
		<< "]";

	return stream;
}

std::ostream& operator <<(std::ostream& stream, const NFMDL::SoundGroupV14& soundGroup)
{
	stream
		<< CLASS_INFO(SoundGroupV14) << "\n"
		<< "[\n"
		<< "    Name: " << soundGroup.name << "\n"
		<< "    Offset: " << soundGroup.offset << "\n"
		<< "]";

	return stream;
}

std::ostream& operator <<(std::ostream& stream, const NFMDL::SoundV14& sound)
{
	stream
		<< CLASS_INFO(SoundV14) << "\n"
		<< "[\n"
		<< "    Name count: " << sound.nameCount << "\n";

	for ( uint32_t index = 0; index < ArraySize(sound.names); ++index )
	{
		stream << "    Name [" << index << "]: " << sound.names[index] << "\n";
	}

	stream
		<< "]";

	return stream;
}

std::ostream& operator <<(std::ostream& stream, const NFMDL::LevelOfDetailV14& lod)
{
	stream
		<< CLASS_INFO(LevelOfDetailV14) << "\n"
		<< "[\n"
		<< "    Levels: " << lod.levels << "\n";

	for ( uint32_t index = 0; index < ArraySize(lod.distance); ++index )
	{
		stream << "    Distance [" << index << "]: " << lod.distance[index] << "\n";
	}

	stream
		<< "]";

	return stream;
}

std::ostream& operator <<(std::ostream& stream, const NFMDL::BodyGroup& bodyGroup)
{
	stream
		<< CLASS_INFO(BodyGroup) << "\n"
		<< "[\n"
		<< "    Name: " << bodyGroup.name << "\n"
		<< "    Model count: " << bodyGroup.modelCount << "\n"
		<< "    Body count: " << bodyGroup.bodyCount << "\n"
		<< "    Model offset: " << bodyGroup.modelOffset << "\n"
		<< "]";

	return stream;
}

std::ostream& operator <<(std::ostream& stream, const NFMDL::TriangleMapV14& triangleMap)
{
	stream
		<< CLASS_INFO(TriangleMapV14) << "\n"
		<< "[\n"
		<< "    Vertex index: " << triangleMap.vertexIndex << "\n"
		<< "]";

	return stream;
}

std::ostream& operator <<(std::ostream& stream, const NFMDL::VertexV14& vertex)
{
	stream
		<< CLASS_INFO(VertexBlendV14) << "\n"
		<< "[\n"
		<< "    Position: " << vertex.position << "\n"
		<< "    Scale: " << vertex.scale << "\n"
		<< "]";

	return stream;
}

std::ostream& operator <<(std::ostream& stream, const NFMDL::NormalV14& normal)
{
	stream
		<< CLASS_INFO(NormalV14) << "\n"
		<< "[\n"
		<< "    Position: " << normal.position << "\n"
		<< "    Scale: " << normal.scale << "\n"
		<< "]";

	return stream;
}

std::ostream& operator <<(std::ostream& stream, const NFMDL::TextureCoOrdinateV14& texCoOrd)
{
	stream
		<< CLASS_INFO(TextureCoOrdinateV14) << "\n"
		<< "[\n"
		<< "    Co-ord: (" << texCoOrd.coOrds[0] << ", " << texCoOrd.coOrds[1] << ")\n"
		<< "]";

	return stream;
}

std::ostream& operator <<(std::ostream& stream, const NFMDL::VertexBlendV14& vBlend)
{
	stream
		<< CLASS_INFO(VertexBlendV14) << "\n"
		<< "[\n";

	for ( uint32_t index = 0; index < ArraySize(vBlend.boneIndex); ++index )
	{
		stream << "    Bone index [" << index << "]: " << static_cast<int32_t>(vBlend.boneIndex[index]) << "\n";
	}

	stream
		<< "]";

	return stream;
}

std::ostream& operator <<(std::ostream& stream, const NFMDL::VertexBlendScaleV14& vBlendScale)
{
	stream
		<< CLASS_INFO(VertexBlendScaleV14) << "\n"
		<< "[\n";

	for ( uint32_t index = 0; index < ArraySize(vBlendScale.scale); ++index )
	{
		stream << "    Scale [" << index << "]: " << vBlendScale.scale[index] << "\n";
	}

	stream
		<< "]";

	return stream;
}

std::ostream& operator <<(std::ostream& stream, const NFMDL::BoneFixUpV14& boneFixUp)
{
	stream
		<< CLASS_INFO(BoneFixUpV14) << "\n"
		<< "[\n"
		<< "    X scale: " << boneFixUp.xScale << "\n"
		<< "    X skew Y: " << boneFixUp.xSkewY << "\n"
		<< "    X skew Z: " << boneFixUp.xSkewZ << "\n"
		<< "    X position: " << boneFixUp.xPosition << "\n"
		<< "    Y skew X: " << boneFixUp.ySkewX << "\n"
		<< "    Y scale: " << boneFixUp.yScale << "\n"
		<< "    Y skew Z: " << boneFixUp.ySkewZ << "\n"
		<< "    Y position: " << boneFixUp.yPosition << "\n"
		<< "    Z skew X: " << boneFixUp.zSkewX << "\n"
		<< "    Z skew Y: " << boneFixUp.zSkewY << "\n"
		<< "    Z scale: " << boneFixUp.zScale << "\n"
		<< "    Z position: " << boneFixUp.zPosition << "\n"
		<< "]";

	return stream;
}

std::ostream& operator <<(std::ostream& stream, const NFMDL::Skin& skin)
{
	stream
		<< CLASS_INFO(Skin) << "\n"
		<< "[\n"
		<< "    Value: " << skin.value << "\n"
		<< "]";

	return stream;
}

std::ostream& operator <<(std::ostream& stream, const NFMDL::ModelV14& model)
{
	stream
		<< CLASS_INFO(ModelV14) << "\n"
		<< "[\n"
		<< "    Name: " << model.name << "\n"
		<< "    Index: " << model.index << "\n";

	for ( uint32_t index = 0; index < ArraySize(model.modelInfoOffset); ++index )
	{
		stream << "    Model info offset [" << index << "]: " << model.modelInfoOffset[index] << "\n";
	}

	stream
		<< "]";

	return stream;
}

std::ostream& operator <<(std::ostream& stream, const NFMDL::ModelInfoV14& modelInfo)
{
	stream
		<< CLASS_INFO(ModelInfoV14) << "\n"
		<< "[\n"
		<< "    Skin reference: " << modelInfo.skinReference << "\n"
		<< "    Meshes: " << modelInfo.meshes << "\n"
		<< "]";

	return stream;
}

std::ostream& operator <<(std::ostream& stream, const NFMDL::MeshV14& mesh)
{
	stream
		<< CLASS_INFO(MeshV14) << "\n"
		<< "[\n";

	for ( uint32_t index = 0; index < ArraySize(mesh.boneMappings); ++index )
	{
		stream << "    Bone [" << index << "]: " << static_cast<int32_t>(mesh.boneMappings[index]) << "\n";
	}

	stream
		<< "    Triangles index: " << mesh.trianglesIndex << "\n"
		<< "    Triangles count: " << mesh.trianglesCount << "\n"
		<< "    Vertices index (from): " << mesh.verticesIndexFrom << "\n"
		<< "    Vertices index (to): " << mesh.verticesIndexTo << "\n"
		<< "]";

	return stream;
}

std::ostream& operator <<(std::ostream& stream, const NFMDL::AnimationDataValueList& list)
{
	stream
		<< CLASS_INFO(AnimationDataValueList) << "\n"
		<< "[\n"
		<< "    Value count: " << list.size() << "\n"
		<< "]";

	return stream;
}

std::ostream& operator <<(std::ostream& stream, const NFMDL::Event& event)
{
	stream
		<< CLASS_INFO(Event) << "\n"
		<< "[\n"
		<< "    Trigger frame: " << event.triggerFrame << "\n"
		<< "    ID: " << event.eventID << "\n"
		<< "    Type: " << event.type << "\n"
		<< "    Options: " << event.options << "\n"
		<< "]";

	return stream;
}

std::ostream& operator <<(std::ostream& stream, const NFMDL::FootPivot& footPivot)
{
	stream
		<< CLASS_INFO(FootPivot) << "\n"
		<< "[\n"
		<< "    Position: " << footPivot.position << "\n"
		<< "    Start: " << footPivot.start << "\n"
		<< "    End: " << footPivot.end << "\n"
		<< "]";

	return stream;
}

std::ostream& operator <<(std::ostream& stream, const NFMDL::SkinCollectionKey& key)
{
	stream << "[Family " << key.skinFamily << ", Reference " << key.skinReference << "]";
	return stream;
}

std::ostream& operator <<(std::ostream& stream, const NFMDL::TOwnedItemKey<NFMDL::ModelInfoV14>& key)
{
	return WriteOwnedItemKey(stream, key, "Parent model", "Model info");
}

std::ostream& operator <<(std::ostream& stream, const NFMDL::MeshCollectionKey& key)
{
	stream << "[Parent model index " << key.modelIndex << ", Parent model info index " << key.modelInfoIndex << ", Mesh index " << key.meshIndex << "]";
	return stream;
}

std::ostream& operator <<(std::ostream& stream, const NFMDL::TOwnedItemKey<NFMDL::SoundV14>& key)
{
	return WriteOwnedItemKey(stream, key, "Parent sound group", "Sound");
}

std::ostream& operator <<(std::ostream& stream, const NFMDL::AnimationDataCollectionKey& key)
{
	stream << "[Sequence " << key.sequenceIndex << ", Blend " << key.blendIndex << ", Bone " << key.boneIndex << ", Component " << key.componentIndex << "]";
	return stream;
}

std::ostream& operator <<(std::ostream& stream, const NFMDL::TOwnedItemKey<NFMDL::Event>& key)
{
	return WriteOwnedItemKey(stream, key, "Parent sequence", "Event");
}

std::ostream& operator <<(std::ostream& stream, const NFMDL::TOwnedItemKey<NFMDL::FootPivot>& key)
{
	return WriteOwnedItemKey(stream, key, "Parent sequence", "Foot pivot");
}
