#pragma once

#include "containers/ElementArray.h"
#include "containers/ElementContainer.h"
#include "Types.h"

#include "elements/HeaderV14.h"
#include "elements/Bone.h"
#include "elements/BoneController.h"
#include "elements/HitBox.h"
#include "elements/SequenceV14.h"
#include "elements/SequenceGroup.h"
#include "elements/TextureV14.h"
#include "elements/BodyGroup.h"
#include "elements/Attachment.h"
#include "elements/SoundGroupV14.h"
#include "elements/TriangleMapV14.h"
#include "elements/VertexV14.h"
#include "elements/NormalV14.h"
#include "elements/TextureCoOrdinateV14.h"
#include "elements/VertexBlendScaleV14.h"
#include "elements/VertexBlendV14.h"
#include "elements/BoneFixUpV14.h"
#include "elements/AugmentedModelV14.h"
#include "elements/LevelOfDetailV14.h"
#include "elements/Event.h"
#include "elements/FootPivot.h"
#include "elements/AnimationValue.h"
#include "elements/TriangleMapV14.h"
#include "elements/AugmentedModelInfoV14.h"
#include "elements/MeshV14.h"
#include "elements/SoundV14.h"
#include "elements/Skin.h"

#include "elements/ElementTraits.h"

namespace NFMDL
{
	class NightfireModelFile
	{
	public:
		using AnimationDataCollection = std::map<AnimationDataCollectionKey, AnimationDataValueList>;
		using SkinCollection = std::map<SkinCollectionKey, Skin>;
		using EventCollection = OwnedItemCollection<Event>;
		using FootPivotCollection = OwnedItemCollection<FootPivot>;
		using SoundCollection = OwnedItemCollection<SoundV14>;
		using ModelInfoCollection = OwnedItemCollection<AugmentedModelInfoV14>;
		using MeshCollection = std::map<MeshCollectionKeyV14, MeshV14>;

		HeaderV14 Header;

		ElementContainer<Bone> Bones;
		ElementContainer<BoneController> BoneControllers;
		ElementContainer<HitBox> HitBoxes;
		ElementContainer<SequenceV14> Sequences;
		ElementContainer<SequenceGroup> SequenceGroups;
		ElementContainer<TextureV14> Textures;
		ElementContainer<Attachment> Attachments;
		ElementContainer<SoundGroupV14> SoundGroups;
		ElementContainer<TriangleMapV14> TriangleMaps;
		ElementContainer<VertexV14> Vertices;
		ElementContainer<NormalV14> Normals;
		ElementContainer<TextureCoOrdinateV14> TextureCoOrdinates;
		ElementContainer<VertexBlendScaleV14> VertexBlendScales;
		ElementContainer<VertexBlendV14> VertexBlends;
		ElementContainer<BoneFixUpV14> BoneFixUps;
		ElementArray<AugmentedModelV14> Models;
		ElementContainer<LevelOfDetailV14> LevelsOfDetail;
		ElementArray<BodyGroup> BodyGroups;

		ElementContainer<Skin, NullElementUserData, SkinCollectionKey> Skins;

		// Owned by sequences:
		ElementContainer<Event, NullElementUserData, TOwnedItemKey<Event>> Events;
		ElementContainer<FootPivot, NullElementUserData, TOwnedItemKey<FootPivot>> FootPivots;
		ElementContainer<AnimationDataValueList, NullElementUserData, AnimationDataCollectionKey> AnimationData;

		// Owned by sound groups:
		ElementContainer<SoundV14, NullElementUserData, TOwnedItemKey<SoundV14>> Sounds;

		// Owned by models:
		ModelInfoCollection ModelInfos;
		MeshCollection Meshes;

		// Other related information, useful for dumping later:
		uint32_t LODOffset = 0;
	};
}
