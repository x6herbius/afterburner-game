#pragma once

#include <cstdint>
#include <unordered_map>
#include <vector>
#include <tuple>

#include "utils/ElementArray.h"
#include "utils/ElementMap.h"

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
#include "elements/BoneBlendScaleV14.h"
#include "elements/BoneBlendV14.h"
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
			uint32_t blendIndex = 0;
			uint32_t boneIndex = 0;
			uint32_t blendComponent = 0;

			inline bool operator ==(const AnimationCollectionKey& other) const
			{
				return blendIndex == other.blendIndex &&
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
					const std::size_t blendIndexHash = std::hash<uint32_t>{}(key.blendIndex);
					const std::size_t boneIndexHash  = std::hash<uint32_t>{}(key.boneIndex);
					const std::size_t blendComponentHash  = std::hash<uint32_t>{}(key.blendComponent);

					return blendIndexHash ^ (boneIndexHash << 1) ^ (blendComponentHash << 2);
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
					const std::size_t skinReferencesHash = std::hash<uint32_t>{}(key.skinReferences);
					const std::size_t skinFamiliesHash  = std::hash<uint32_t>{}(key.skinFamilies);

					return skinReferencesHash ^ (skinFamiliesHash << 1);
				}
			};
		};

		using BlendedAnimationValueList = std::vector<decltype(AnimationValue::value)>;
		using BlendedAnimationCollection = std::unordered_map<AnimationCollectionKey, BlendedAnimationValueList, AnimationCollectionKey::Hash>;
		using SkinCollection = std::unordered_map<SkinCollectionKey, int16_t, SkinCollectionKey::Hash>;

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
		ElementArray<BoneBlendScaleV14> BoneBlendScales;
		ElementArray<BoneBlendV14> BoneBlends;
		ElementArray<BoneFixUpV14> BoneFixUps;
		ElementArray<ModelV14> Models;
		ElementArray<LevelOfDetailV14> LevelsOfDetail;
		ElementArray<Event> Events;
		ElementArray<Pivot> Pivots;
		ElementArray<Animation> Animations;
		ElementArray<BodyGroupV14> BodyGroups;

		ElementMap<ModelInfoV14> ModelInfos;
		ElementMap<MeshV14> Meshes;
		ElementMap<SoundV14> Sounds;

		BlendedAnimationCollection BlendedAnimationData;
		SkinCollection Skins;
	};
}
