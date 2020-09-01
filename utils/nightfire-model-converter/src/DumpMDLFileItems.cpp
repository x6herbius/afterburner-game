#include <iostream>
#include <sstream>
#include "DumpMDLFileItems.h"
#include "elements/DumpElements.h"

template<typename T>
static inline void DumpItems(const std::string& itemName,
							 const std::string& itemNamePlural,
							 const NFMDL::ElementArray<T>& elementArray)
{
	std::cout << elementArray.Count() << " " << itemNamePlural << ":" << std::endl;

	elementArray.ForEach([itemName](uint32_t index, const T& item)
	{
		std::cout << itemName << " " << index << ":\n" << item << std::endl;
	});
}

template<typename K, typename V>
static inline void DumpItems(const std::string& itemName,
							 const std::string& itemNamePlural,
							 const std::map<K, V>& hashMap)
{
	std::cout << hashMap.size() << " " << itemNamePlural << ":" << std::endl;

	for ( typename std::map<K, V>::const_iterator it = hashMap.cbegin(); it != hashMap.cend(); ++it )
	{
		std::cout << itemName << " " << it->first << ":\n" << it->second << std::endl;
	}
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

	if ( options.dumpSounds )
	{
		DumpItems("Sound", "sounds", modelFile.Sounds);
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

	if ( options.dumpNormals )
	{
		DumpItems("Normal", "normals", modelFile.Normals);
	}

	if ( options.dumpTextureCoOrdinates )
	{
		DumpItems("Texture co-ordinate", "texture co-ordinates", modelFile.TextureCoOrdinates);
	}

	if ( options.dumpVertexBlends )
	{
		DumpItems("Vertex blend", "vertex blends", modelFile.VertexBlends);
	}

	if ( options.dumpVertexBlendScales )
	{
		DumpItems("Vertex blend scale", "vertex blend scales", modelFile.VertexBlendScales);
	}

	if ( options.dumpBoneFixUps )
	{
		DumpItems("Bone fix-up", "bone fix-ups", modelFile.BoneFixUps);
	}

	if ( options.dumpSkins )
	{
		DumpItems("Skin", "skins", modelFile.Skins);
	}

	if ( options.dumpModels )
	{
		DumpItems("Model", "models", modelFile.Models);
	}

	if ( options.dumpModelInfos )
	{
		DumpItems("Model info", "model infos", modelFile.ModelInfos);
	}

	if ( options.dumpMeshes )
	{
		DumpItems("Mesh", "meshes", modelFile.Meshes);
	}

	if ( options.dumpAnimationData )
	{
		DumpItems("Animation data", "animation data items", modelFile.AnimationData);
	}

	if ( options.dumpEvents )
	{
		DumpItems("Event", "events", modelFile.Events);
	}

	if ( options.dumpFootPivots )
	{
		DumpItems("Foot pivot", "foot pivots", modelFile.FootPivots);
	}
}
