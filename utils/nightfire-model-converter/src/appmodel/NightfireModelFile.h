#pragma once

#include <cstdint>
#include <unordered_map>
#include <vector>
#include <tuple>

#include "utils/ElementArray.h"
#include "utils/ElementMap.h"

#include "elements/HeaderV14.h"
#include "elements/Bone.h"
#include "elements/BoneController.h"
#include "elements/HitBox.h"
#include "elements/SequenceV14.h"
#include "elements/SequenceGroup.h"
#include "elements/TextureV14.h"
#include "elements/BodyGroupV14.h"
#include "elements/Attachment.h"
#include "elements/SoundGroupV14.h"
#include "elements/TriangleMapV14.h"
#include "elements/VertexV14.h"
#include "elements/NormalV14.h"
#include "elements/TextureCoOrdinateV14.h"
#include "elements/VertexBlendScaleV14.h"
#include "elements/VertexBlendV14.h"
#include "elements/BoneFixUpV14.h"
#include "elements/ModelV14.h"
#include "elements/LevelOfDetailV14.h"
#include "elements/Event.h"
#include "elements/Pivot.h"
#include "elements/AnimationValue.h"
#include "elements/TriangleMapV14.h"
#include "elements/ModelInfoV14.h"
#include "elements/MeshV14.h"
#include "elements/SoundV14.h"
#include "elements/Skin.h"

#include "elements/ElementTraits.h"

namespace NFMDL
{
	class NightfireModelFile
	{
	public:
		struct AnimationDataCollectionKey
		{
			uint32_t sequenceIndex = 0;
			uint32_t blendIndex = 0;
			uint32_t boneIndex = 0;
			uint32_t componentIndex = 0;

			inline bool operator ==(const AnimationDataCollectionKey& other) const
			{
				return sequenceIndex == other.sequenceIndex &&
					   blendIndex == other.blendIndex &&
					   boneIndex == other.boneIndex &&
					   componentIndex == other.componentIndex;
			}

			inline bool operator !=(const AnimationDataCollectionKey& other) const
			{
				return !(*this == other);
			}

			inline bool operator <(const AnimationDataCollectionKey& other) const
			{
				if ( sequenceIndex != other.sequenceIndex )
				{
					return sequenceIndex < other.sequenceIndex;
				}

				if ( blendIndex != other.blendIndex )
				{
					return blendIndex < other.blendIndex;
				}

				if ( boneIndex < other.boneIndex )
				{
					return boneIndex < other.boneIndex;
				}

				return componentIndex < other.componentIndex;
			}

			struct Hash
			{
				inline std::size_t operator()(const AnimationDataCollectionKey& key) const noexcept
				{
					size_t hash = std::hash<uint32_t>{}(key.sequenceIndex);
					hash = std::hash<uint32_t>{}(key.blendIndex) ^ (hash << 1);
					hash = std::hash<uint32_t>{}(key.boneIndex) ^ (hash << 1);
					hash = std::hash<uint32_t>{}(key.componentIndex) ^ (hash << 1);

					return hash;
				}
			};
		};

		struct SkinCollectionKey
		{
			uint32_t skinFamily = 0;
			uint32_t skinReference = 0;

			inline bool operator ==(const SkinCollectionKey& other) const
			{
				return skinFamily == other.skinFamily &&
					   skinReference == other.skinReference;
			}

			inline bool operator !=(const SkinCollectionKey& other) const
			{
				return !(*this == other);
			}

			inline bool operator <(const SkinCollectionKey& other) const
			{
				if ( skinFamily != other.skinFamily )
				{
					return skinFamily < other.skinFamily;
				}

				return skinReference < other.skinReference;
			}

			struct Hash
			{
				inline std::size_t operator()(const SkinCollectionKey& key) const noexcept
				{
					size_t hash = std::hash<uint32_t>{}(key.skinFamily);
					hash = std::hash<uint32_t>{}(key.skinReference) ^ (hash << 1);

					return hash;
				}
			};
		};

		struct MeshCollectionKey
		{
			uint32_t modelIndex = 0;
			uint32_t modelInfoIndex = 0;
			uint32_t meshIndex = 0;

			inline bool operator ==(const MeshCollectionKey& other) const
			{
				return modelIndex == other.modelIndex &&
					   modelInfoIndex == other.modelInfoIndex &&
					   meshIndex == other.meshIndex;
			}

			inline bool operator !=(const MeshCollectionKey& other) const
			{
				return !(*this == other);
			}

			inline bool operator <(const MeshCollectionKey& other) const
			{
				if ( modelIndex != other.modelIndex )
				{
					return modelIndex < other.modelIndex;
				}

				if ( modelInfoIndex != other.modelInfoIndex )
				{
					return modelInfoIndex < other.modelInfoIndex;
				}

				return meshIndex < other.meshIndex;
			}

			struct Hash
			{
				inline std::size_t operator()(const MeshCollectionKey& key) const noexcept
				{
					size_t hash = std::hash<uint32_t>{}(key.modelIndex);
					hash = std::hash<uint32_t>{}(key.modelInfoIndex) ^ (hash << 1);
					hash = std::hash<uint32_t>{}(key.meshIndex) ^ (hash << 1);

					return hash;
				}
			};
		};

		struct OwnedItemKey
		{
			uint32_t ownerIndex = 0;
			uint32_t itemIndex = 0;

			inline bool operator ==(const OwnedItemKey& other) const
			{
				return ownerIndex == other.ownerIndex &&
					   itemIndex == other.itemIndex;
			}

			inline bool operator !=(const OwnedItemKey& other) const
			{
				return !(*this == other);
			}

			inline bool operator <(const OwnedItemKey& other) const
			{
				if ( ownerIndex != other.ownerIndex )
				{
					return ownerIndex < other.ownerIndex;
				}

				return itemIndex < other.itemIndex;
			}

			struct Hash
			{
				inline std::size_t operator()(const OwnedItemKey& key) const noexcept
				{
					size_t hash = std::hash<uint32_t>{}(key.ownerIndex);
					hash = std::hash<uint32_t>{}(key.itemIndex) ^ (hash << 1);

					return hash;
				}
			};
		};

		class AnimationDataValueList : public std::vector<decltype(AnimationValue::value)>
		{
		};

		// So that we can specialise the stream operator when dumping keys
		// from containers of different types.
		template<typename T>
		struct TOwnedItemKey : public OwnedItemKey {};

		template<typename T>
		using OwnedItemCollection = std::map<TOwnedItemKey<T>, T>;

		using AnimationDataCollection = std::map<AnimationDataCollectionKey, AnimationDataValueList>;
		using SkinCollection = std::map<SkinCollectionKey, Skin>;
		using EventCollection = OwnedItemCollection<Event>;
		using PivotCollection = OwnedItemCollection<Pivot>;
		using SoundCollection = OwnedItemCollection<SoundV14>;
		using ModelInfoCollection = OwnedItemCollection<ModelInfoV14>;
		using MeshCollection = std::map<MeshCollectionKey, MeshV14>;

		HeaderV14 Header;

		ElementArray<Bone> Bones;
		ElementArray<BoneController> BoneControllers;
		ElementArray<HitBox> HitBoxes;
		ElementArray<SequenceV14> Sequences;
		ElementArray<SequenceGroup> SequenceGroups;
		ElementArray<TextureV14> Textures;
		ElementArray<Attachment> Attachments;
		ElementArray<SoundGroupV14> SoundGroups;
		ElementArray<TriangleMapV14> TriangleMaps;
		ElementArray<VertexV14> Vertices;
		ElementArray<NormalV14> Normals;
		ElementArray<TextureCoOrdinateV14> TextureCoOrdinates;
		ElementArray<VertexBlendScaleV14> VertexBlendScales;
		ElementArray<VertexBlendV14> VertexBlends;
		ElementArray<BoneFixUpV14> BoneFixUps;
		ElementArray<ModelV14> Models;
		ElementArray<LevelOfDetailV14> LevelsOfDetail;
		ElementArray<BodyGroupV14> BodyGroups;

		SkinCollection Skins;

		// Owned by sequences:
		EventCollection Events;
		PivotCollection Pivots;
		AnimationDataCollection AnimationData;

		// Owned by sound groups:
		SoundCollection Sounds;

		// Owned by models:
		ModelInfoCollection ModelInfos;
		MeshCollection Meshes;
	};

	template<>
	struct ElementTraits<NightfireModelFile::AnimationDataValueList>
	{
		static constexpr const char* ELEMENT_NAME = "AnimationDataValueList";
	};
}
