#pragma once

#include "containers/ElementArray.h"
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
		ElementArray<AugmentedModelV14> Models;
		ElementArray<LevelOfDetailV14> LevelsOfDetail;
		ElementArray<BodyGroup> BodyGroups;

		SkinCollection Skins;

		// Owned by sequences:
		EventCollection Events;
		FootPivotCollection FootPivots;
		AnimationDataCollection AnimationData;

		// Owned by sound groups:
		SoundCollection Sounds;

		// Owned by models:
		ModelInfoCollection ModelInfos;
		MeshCollection Meshes;

		// Other related information, useful for dumping later.
		uint32_t LODOffset = 0;
	};
}
