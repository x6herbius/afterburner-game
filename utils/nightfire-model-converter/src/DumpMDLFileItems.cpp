#include <iostream>
#include <sstream>
#include "DumpMDLFileItems.h"
#include "elements/DumpElements.h"

template<typename T>
static inline void DumpItems(const std::string& itemName,
							 const std::string& itemNamePlural,
							 const NFMDL::ElementArray<T>& elementArray)
{
	std::cout << elementArray.Count() << " " + itemNamePlural + ":" << std::endl;

	elementArray.ForEach([itemName](uint32_t index, const T& item)
	{
		std::stringstream stream;
		stream << item;
		std::cout << itemName << " " << index << ":\n" << stream.str() << std::endl;
	});
}

void DumpMDLFileItems(const AppOptions& options, const NFMDL::NightfireModelFile& modelFile)
{
	if ( options.dumpHeader )
	{
		std::stringstream stream;
		stream << modelFile.Header;
		std::cout << stream.str() << std::endl;
	}

	if ( options.readHeaderOnly )
	{
		return;
	}

	if ( options.dumpBones )
	{
		DumpItems("Bone", "bones", modelFile.Bones);
	}

	if ( options.dumpBoneControllers )
	{
		DumpItems("Bone controller", "bone controllers", modelFile.BoneControllers);
	}

	if ( options.dumpSequences )
	{
		DumpItems("Sequence", "sequences", modelFile.Sequences);
	}

	if ( options.dumpSequenceGroups )
	{
		DumpItems("Sequence group", "sequence groups", modelFile.SequenceGroups);
	}

	if ( options.dumpHitBoxes )
	{
		DumpItems("Hitbox", "hitboxes", modelFile.HitBoxes);
	}

	if ( options.dumpTextures )
	{
		DumpItems("Texture", "textures", modelFile.Textures);
	}

	if ( options.dumpAttachments )
	{
		DumpItems("Attachment", "attachments", modelFile.Attachments);
	}

	if ( options.dumpSoundGroups )
	{
		DumpItems("Sound group", "sound groups", modelFile.SoundGroups);
	}

	if ( options.dumpLevelsOfDetail )
	{
		DumpItems("Level of detail", "levels of detail", modelFile.LevelsOfDetail);
	}

	if ( options.dumpBodyGroups )
	{
		DumpItems("Body group", "body groups", modelFile.BodyGroups);
	}

	if ( options.dumpTriangleMaps )
	{
		DumpItems("Triangle map", "triangle maps", modelFile.TriangleMaps);
	}

	if ( options.dumpVertices )
	{
		DumpItems("Vertex", "vertices", modelFile.Vertices);
	}
}
