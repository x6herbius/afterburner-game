#include <iostream>
#include <sstream>
#include <iomanip>
#include "DumpMDLFileItems.h"
#include "elements/DumpElements.h"
#include "Utils.h"

template<typename T, typename OFFSETFUNC>
static inline void DumpItems(const std::string& itemName,
							 const std::string& itemNamePlural,
							 const NFMDL::ElementArray<T>& elementArray,
							 const OFFSETFUNC& offsetFunc)
{
	std::cout << elementArray.Count() << " " << itemNamePlural << ":" << std::endl;

	elementArray.ForEach([itemName, offsetFunc](uint32_t index, const T& item)
	{
		const uint32_t itemOffset = offsetFunc(index);

		if ( itemOffset > 0 )
		{
			std::cout
				<< itemName << " " << index
				<< " (file offset " << itemOffset
				<< "/0x" << std::hex << std::setw(8) << std::setfill('0') << itemOffset << std::dec
				<< "):\n" << item << std::endl;
		}
		else
		{
			std::cout << itemName << " " << index << ":\n" << item << std::endl;
		}
	});
}

template<typename T>
static inline void DumpItems(const std::string& itemName,
							 const std::string& itemNamePlural,
							 const NFMDL::ElementArray<T>& elementArray,
							 uint32_t baseOffset)
{
	DumpItems(itemName, itemNamePlural, elementArray, [baseOffset](uint32_t index)
	{
		return baseOffset + (index * sizeof(T));
	});
}

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
		DumpItems("Sound", "sounds", modelFile.Sounds, [&modelFile](const NFMDL::NightfireModelFile::SoundCollection::const_iterator& it) -> uint32_t
		{
			return it->first.ownerIndex < modelFile.SoundGroups.Count()
				? modelFile.SoundGroups[it->first.ownerIndex].offset
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
		DumpItems("Skin", "skins", modelFile.Skins, [&modelFile](const NFMDL::NightfireModelFile::SkinCollection::const_iterator& it) -> uint32_t
		{
			const uint32_t skinIndex = (it->first.skinFamily * modelFile.Header.skinReferenceCount) + it->first.skinReference;
			return modelFile.Header.skinDataOffset + (skinIndex * sizeof(NFMDL::Skin));
		});
	}

	if ( options.dumpModels )
	{
		DumpItems("Model", "models", modelFile.Models, [&modelFile](uint32_t index) -> uint32_t
		{
			return index < ArraySize(modelFile.Header.modelOffset)
				? modelFile.Header.modelOffset[index]
				: 0;
		});
	}

	if ( options.dumpModelInfos )
	{
		DumpItems("Model info", "model infos", modelFile.ModelInfos, [&modelFile](const NFMDL::NightfireModelFile::ModelInfoCollection::const_iterator& it) -> uint32_t
		{
			if ( it->first.ownerIndex >= modelFile.Models.Count() )
			{
				return 0;
			}

			const NFMDL::ModelV14& model = modelFile.Models[it->first.ownerIndex];

			return it->first.itemIndex < ArraySize(model.modelInfoOffset)
				? model.modelInfoOffset[it->first.itemIndex]
				: 0;
		});
	}

	if ( options.dumpMeshes )
	{
		DumpItems("Mesh", "meshes", modelFile.Meshes, [&modelFile](const NFMDL::NightfireModelFile::MeshCollection::const_iterator& it) -> uint32_t
		{
			NFMDL::TOwnedItemKey<NFMDL::AugmentedModelInfoV14> modelInfoKey;
			modelInfoKey.ownerIndex = it->first.modelIndex;
			modelInfoKey.itemIndex = it->first.modelInfoIndex;

			const NFMDL::NightfireModelFile::ModelInfoCollection::const_iterator modelInfoIt = modelFile.ModelInfos.find(modelInfoKey);

			if ( modelInfoIt == modelFile.ModelInfos.cend() )
			{
				return 0;
			}

			return modelInfoIt->second.meshes.offset + (it->first.meshIndex * sizeof(NFMDL::MeshV14));
		});
	}

	if ( options.dumpAnimationData )
	{
		DumpItems("Animation data", "animation data items", modelFile.AnimationData,
		[](const NFMDL::NightfireModelFile::AnimationDataCollection::const_iterator&) -> uint32_t
		{
			return 0;
		});
	}

	if ( options.dumpEvents )
	{
		DumpItems("Event", "events", modelFile.Events, [&modelFile](const NFMDL::NightfireModelFile::EventCollection::const_iterator& it) -> uint32_t
		{
			if ( it->first.ownerIndex >= modelFile.Sequences.Count() )
			{
				return 0;
			}

			const NFMDL::SequenceV14* const ownerSequence = modelFile.Sequences.ElementAt(it->first.ownerIndex);

			if ( !ownerSequence )
			{
				return 0;
			}

			return ownerSequence->events.offset + (it->first.itemIndex * sizeof(NFMDL::Event));
		});
	}

	if ( options.dumpFootPivots )
	{
		DumpItems("Foot pivot", "foot pivots", modelFile.FootPivots, [&modelFile](const NFMDL::NightfireModelFile::FootPivotCollection::const_iterator& it) -> uint32_t
		{
			if ( it->first.ownerIndex >= modelFile.Sequences.Count() )
			{
				return 0;
			}

			const NFMDL::SequenceV14* const ownerSequence = modelFile.Sequences.ElementAt(it->first.ownerIndex);

			if ( !ownerSequence )
			{
				return 0;
			}

			return ownerSequence->footPivots.offset + (it->first.itemIndex * sizeof(NFMDL::FootPivot));
		});
	}
}
