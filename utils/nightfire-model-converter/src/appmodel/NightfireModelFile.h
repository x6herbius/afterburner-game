#pragma once

#include "containers/ElementContainer.h"
#include "Types.h"

#include "elements/BaseElementUserData.h"
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
#include "elements/ModelV14.h"
#include "elements/LevelOfDetailV14.h"
#include "elements/Event.h"
#include "elements/FootPivot.h"
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
		ElementContainer<ModelV14, BaseElementUserData, TOwnedItemKey<ModelV14>> Models;
		ElementContainer<LevelOfDetailV14> LevelsOfDetail;
		ElementContainer<BodyGroup> BodyGroups;

		ElementContainer<Skin, NullElementUserData, SkinCollectionKey> Skins;

		// Owned by sequences:
		ElementContainer<Event, NullElementUserData, TOwnedItemKey<Event>> Events;
		ElementContainer<FootPivot, NullElementUserData, TOwnedItemKey<FootPivot>> FootPivots;
		ElementContainer<AnimationDataValueList, NullElementUserData, AnimationDataCollectionKey> AnimationData;

		// Owned by sound groups:
		ElementContainer<SoundV14, NullElementUserData, TOwnedItemKey<SoundV14>> Sounds;

		// Owned by models:
		ElementContainer<ModelInfoV14, BaseElementUserData, ModelInfoCollectionKeyV14> ModelInfos;
		ElementContainer<MeshV14, BaseElementUserData, MeshCollectionKeyV14> Meshes;

		// Other related information, useful for dumping later:
		uint32_t LODOffset = 0;
	};
}
