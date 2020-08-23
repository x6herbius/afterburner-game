#include <sstream>
#include <iomanip>
#include "DumpElements.h"
#include "Utils.h"

using namespace NFMDL;

#define AS_HEX(value) "0x" << std::hex << std::setfill('0') << std::setw(8) << (value) << std::dec

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
		<< "BoundingBox("
		<< bbox.min[0] << ", " << bbox.min[1] << ", " << bbox.min[2]
		<< " - "
		<< bbox.max[0] << ", " << bbox.max[1] << ", " << bbox.max[2]
		<< ")"
		;

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
		<< "    LOD flags: " << AS_HEX(header.lodFlags) << "\n"
		<< "    Model count: " << header.modelCount << "\n"
		<< "    Vertex count: " << header.vertexCount << "\n"
		<< "    Triangle count: " << header.triangleCount << "\n"
		<< "    Triangle maps offset: " << header.triangleMapOffset << "\n"
		<< "    Vertices offset: " << header.vertexOffset << "\n"
		<< "    Normals offset: " << header.normalOffset << "\n"
		<< "    Texture co-ords offset: " << header.textureCoOrdOffset << "\n"
		<< "    Blending scales offset: " << header.textureCoOrdOffset << "\n"
		<< "    Bone blends offset: " << header.boneBlendOffset << "\n"
		<< "    Bone fix-ups offset: " << header.boneFixUpOffset << "\n"
		;

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
