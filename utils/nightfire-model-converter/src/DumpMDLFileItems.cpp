#include <iostream>
#include <sstream>
#include <iomanip>
#include "DumpMDLFileItems.h"
#include "elements/DumpElements.h"
#include "Utils.h"

template<typename K, typename V, typename OFFSETFUNC>
static inline void DumpItems(const std::string& itemName,
							 const std::string& itemNamePlural,
							 const std::map<K, V>& hashMap,
							 const OFFSETFUNC& offsetFunc)
{
	std::cout << hashMap.size() << " " << itemNamePlural << ":" << std::endl;

	for ( typename std::map<K, V>::const_iterator it = hashMap.cbegin(); it != hashMap.cend(); ++it )
	{
		const uint32_t itemOffset = offsetFunc(it);

		if ( itemOffset > 0 )
		{
			std::cout
				<< itemName << " " << it->first
				<< " (file offset " << itemOffset
				<< "/0x" << std::hex << std::setw(8) << std::setfill('0') << itemOffset << std::dec
				<< "):\n" << it->second << std::endl;
		}
		else
		{
			std::cout << itemName << " " << it->first << ":\n" << it->second << std::endl;
		}
	}
}

template<typename T, typename U, typename K, typename OFFSETFUNC>
static inline void DumpItems(const std::string& itemName,
							 const std::string& itemNamePlural,
							 const NFMDL::ElementContainer<T, U, K>& elementContainer,
							 const OFFSETFUNC& offsetFunc)
{
	using ContainerType = NFMDL::ElementContainer<T, U, K>;

	std::cout << elementContainer.Count() << " " << itemNamePlural << ":" << std::endl;

	for ( const ContainerType::ConstIteratorData& it : elementContainer )
	{
		const uint32_t itemOffset = offsetFunc(it.index);

		if ( itemOffset > 0 )
		{
			std::cout
				<< itemName << " " << it.index
				<< " (file offset " << itemOffset
				<< "/0x" << std::hex << std::setw(8) << std::setfill('0') << itemOffset << std::dec
				<< "):\n" << *(it.element) << std::endl;
		}
		else
		{
			std::cout << itemName << " " << it.index << ":\n" << *(it.element) << std::endl;
		}
	}
}

template<typename T, typename U, typename K>
static inline void DumpItems(const std::string& itemName,
							 const std::string& itemNamePlural,
							 const NFMDL::ElementContainer<T, U, K>& elementContainer,
							 uint32_t baseOffset)
{
	DumpItems(itemName, itemNamePlural, elementContainer, [baseOffset](uint32_t index)
	{
		return baseOffset + (index * sizeof(T));
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
		DumpItems("Bone", "bones", modelFile.Bones, modelFile.Header.bones.offset);
	}

	if ( options.dumpBoneControllers )
	{
		DumpItems("Bone controller", "bone controllers", modelFile.BoneControllers, modelFile.Header.boneControllers.offset);
	}

	if ( options.dumpSequences )
	{
		DumpItems("Sequence", "sequences", modelFile.Sequences, modelFile.Header.sequences.offset);
	}

	if ( options.dumpSequenceGroups )
	{
		DumpItems("Sequence group", "sequence groups", modelFile.SequenceGroups, modelFile.Header.sequenceGroups.offset);
	}

	if ( options.dumpHitBoxes )
	{
		DumpItems("Hitbox", "hitboxes", modelFile.HitBoxes, modelFile.Header.hitBoxes.offset);
	}

	if ( options.dumpTextures )
	{
		DumpItems("Texture", "textures", modelFile.Textures, modelFile.Header.textures.offset);
	}

	if ( options.dumpAttachments )
	{
		DumpItems("Attachment", "attachments", modelFile.Attachments, modelFile.Header.attachments.offset);
	}

	if ( options.dumpSoundGroups )
	{
		DumpItems("Sound group", "sound groups", modelFile.SoundGroups, modelFile.Header.soundGroups.offset);
	}

	if ( options.dumpSounds )
	{
		DumpItems("Sound", "sounds", modelFile.Sounds, [&modelFile](size_t index) -> size_t
		{
			const NFMDL::TOwnedItemKey<NFMDL::SoundV14>& key = modelFile.Sounds.KeyFor(index);

			return key.ownerIndex < modelFile.SoundGroups.Count()
				? modelFile.SoundGroups.ElementAt(key.ownerIndex)->offset
				: 0;
		});
	}

	if ( options.dumpLevelsOfDetail )
	{
		DumpItems("Level of detail", "levels of detail", modelFile.LevelsOfDetail, modelFile.LODOffset);
	}

	if ( options.dumpBodyGroups )
	{
		DumpItems("Body group", "body groups", modelFile.BodyGroups, modelFile.Header.bodyGroups.offset);
	}

	if ( options.dumpTriangleMaps )
	{
		DumpItems("Triangle map", "triangle maps", modelFile.TriangleMaps, modelFile.Header.triangleMapOffset);
	}

	if ( options.dumpVertices )
	{
		DumpItems("Vertex", "vertices", modelFile.Vertices, modelFile.Header.vertexOffset);
	}

	if ( options.dumpNormals )
	{
		DumpItems("Normal", "normals", modelFile.Normals, modelFile.Header.normalOffset);
	}

	if ( options.dumpTextureCoOrdinates )
	{
		DumpItems("Texture co-ordinate", "texture co-ordinates", modelFile.TextureCoOrdinates, modelFile.Header.textureCoOrdOffset);
	}

	if ( options.dumpVertexBlends )
	{
		DumpItems("Vertex blend", "vertex blends", modelFile.VertexBlends, modelFile.Header.vertexBlendOffset);
	}

	if ( options.dumpVertexBlendScales )
	{
		DumpItems("Vertex blend scale", "vertex blend scales", modelFile.VertexBlendScales, modelFile.Header.vertexBlendScaleOffset);
	}

	if ( options.dumpBoneFixUps )
	{
		DumpItems("Bone fix-up", "bone fix-ups", modelFile.BoneFixUps, modelFile.Header.boneFixUpOffset);
	}

	if ( options.dumpSkins )
	{
		DumpItems("Skin", "skins", modelFile.Skins, [&modelFile](size_t index) -> size_t
		{
			const NFMDL::SkinCollectionKey& key = modelFile.Skins.KeyFor(index);
			const uint32_t skinIndex = (key.skinFamily * modelFile.Header.skinReferenceCount) + key.skinReference;
			return modelFile.Header.skinDataOffset + (skinIndex * sizeof(NFMDL::Skin));
		});
	}

	if ( options.dumpModels )
	{
		DumpItems("Model", "models", modelFile.Models, [&modelFile](uint32_t index) -> size_t
		{
			return index < modelFile.Models.Count()
				? modelFile.Models.UserDataAt(index)->FileOffset()
				: 0;
		});
	}

	if ( options.dumpModelInfos )
	{
		DumpItems("Model info", "model infos", modelFile.ModelInfos, [&modelFile](size_t index) -> size_t
		{
			return index < modelFile.ModelInfos.Count()
				? modelFile.ModelInfos.UserDataAt(index)->FileOffset()
				: 0;
		});
	}

	if ( options.dumpMeshes )
	{
		DumpItems("Mesh", "meshes", modelFile.Meshes, [&modelFile](size_t index) -> size_t
		{
			return index < modelFile.Meshes.Count()
				? modelFile.Meshes.UserDataAt(index)->FileOffset()
				: 0;
		});
	}

	if ( options.dumpAnimationData )
	{
		DumpItems("Animation data", "animation data items", modelFile.AnimationData,
		[](size_t) -> size_t
		{
			return 0;
		});
	}

	if ( options.dumpEvents )
	{
		DumpItems("Event", "events", modelFile.Events, [&modelFile](size_t index) -> size_t
		{
			const NFMDL::TOwnedItemKey<NFMDL::Event>& key = modelFile.Events.KeyFor(index);

			if ( key.ownerIndex >= modelFile.Sequences.Count() )
			{
				return 0;
			}

			const NFMDL::SequenceV14* const ownerSequence = modelFile.Sequences.ElementAt(key.ownerIndex);

			if ( !ownerSequence )
			{
				return 0;
			}

			return ownerSequence->events.offset + (key.itemIndex * sizeof(NFMDL::Event));
		});
	}

	if ( options.dumpFootPivots )
	{
		DumpItems("Foot pivot", "foot pivots", modelFile.FootPivots, [&modelFile](size_t index) -> size_t
		{
			const NFMDL::TOwnedItemKey<NFMDL::FootPivot>& key = modelFile.FootPivots.KeyFor(index);

			if ( !key || key.ownerIndex >= modelFile.Sequences.Count() )
			{
				return 0;
			}

			const NFMDL::SequenceV14* const ownerSequence = modelFile.Sequences.ElementAt(key.ownerIndex);

			if ( !ownerSequence )
			{
				return 0;
			}

			return ownerSequence->footPivots.offset + (key.itemIndex * sizeof(NFMDL::FootPivot));
		});
	}
}
