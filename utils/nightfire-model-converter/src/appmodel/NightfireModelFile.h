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
#include "elements/Animation.h"
#include "elements/AnimationValue.h"
#include "elements/TriangleMapV14.h"
#include "elements/ModelInfoV14.h"
#include "elements/MeshV14.h"
#include "elements/SoundV14.h"

namespace NFMDL
{
	class NightfireModelFile
	{
	public:
		struct AnimationCollectionKey
		{
			uint32_t sequenceIndex = 0;
			uint32_t blendIndex = 0;
			uint32_t boneIndex = 0;
			uint32_t blendComponent = 0;

			inline bool operator ==(const AnimationCollectionKey& other) const
			{
				return sequenceIndex == other.sequenceIndex &&
					   blendIndex == other.blendIndex &&
					   boneIndex == other.boneIndex &&
					   blendComponent == other.blendComponent;
			}

			inline bool operator !=(const AnimationCollectionKey& other) const
			{
				return !(*this == other);
			}

			struct Hash
			{
				inline std::size_t operator()(const AnimationCollectionKey& key) const noexcept
				{
					size_t hash = std::hash<uint32_t>{}(key.sequenceIndex);
					hash = std::hash<uint32_t>{}(key.blendIndex) ^ (hash << 1);
					hash = std::hash<uint32_t>{}(key.boneIndex) ^ (hash << 1);
					hash = std::hash<uint32_t>{}(key.blendComponent) ^ (hash << 1);

					return hash;
				}
			};
		};

		struct SkinCollectionKey
		{
			uint32_t skinReferences = 0;
			uint32_t skinFamilies = 0;

			inline bool operator ==(const SkinCollectionKey& other) const
			{
				return skinReferences == other.skinReferences &&
					   skinFamilies == other.skinFamilies;
			}

			inline bool operator !=(const SkinCollectionKey& other) const
			{
				return !(*this == other);
			}

			struct Hash
			{
				inline std::size_t operator()(const SkinCollectionKey& key) const noexcept
				{
					size_t hash = std::hash<uint32_t>{}(key.skinReferences);
					hash = std::hash<uint32_t>{}(key.skinFamilies) ^ (hash << 1);

					return hash;
				}
			};
		};

		struct OwnedItemKey
		{
			uint32_t ownerIndex = 0;
			uint32_t itemIndex;

			inline bool operator ==(const OwnedItemKey& other) const
			{
				return ownerIndex == other.ownerIndex &&
					   itemIndex == other.itemIndex;
			}

			inline bool operator !=(const OwnedItemKey& other) const
			{
				return !(*this == other);
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

		template<typename T>
		using OwnedItemCollection = std::unordered_map<OwnedItemKey, T, OwnedItemKey::Hash>;

		using BlendedAnimationValueList = std::vector<decltype(AnimationValue::value)>;
		using BlendedAnimationCollection = std::unordered_map<AnimationCollectionKey, BlendedAnimationValueList, AnimationCollectionKey::Hash>;
		using SkinCollection = std::unordered_map<SkinCollectionKey, int16_t, SkinCollectionKey::Hash>;
		using EventCollection = OwnedItemCollection<Event>;
		using PivotCollection = OwnedItemCollection<Pivot>;
		using SoundCollection = OwnedItemCollection<SoundV14>;
		using ModelInfoCollection = OwnedItemCollection<ModelInfoV14>;
		using MeshCollection = OwnedItemCollection<MeshV14>;

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
		ElementArray<Animation> Animations;
		ElementArray<BodyGroupV14> BodyGroups;

		SkinCollection Skins;

		// Owned by sequences:
		EventCollection Events;
		PivotCollection Pivots;
		BlendedAnimationCollection BlendedAnimationData;

		// Owned by sound groups:
		SoundCollection Sounds;

		// Owned by models:
		ModelInfoCollection ModelInfos;
		MeshCollection Meshes;
	};
}
