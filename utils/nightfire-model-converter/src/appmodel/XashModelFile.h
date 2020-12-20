#pragma once

#include "containers/ElementContainer.h"
#include "Types.h"

#include "elements/HeaderV10Xash.h"
#include "elements/Bone.h"
#include "elements/BoneController.h"
#include "elements/HitBox.h"
#include "elements/SequenceV10Xash.h"
#include "elements/SequenceGroup.h"
#include "elements/TextureV10Xash.h"
#include "elements/Attachment.h"
#include "elements/Event.h"
#include "elements/FootPivot.h"
#include "elements/Skin.h"
#include "elements/BodyGroup.h"
#include "elements/ModelV10Xash.h"
#include "elements/MeshV10Xash.h"

namespace NFMDL
{
	class XashModelFile
	{
	public:
		HeaderV10Xash Header;

		ElementContainer<Bone> Bones;
		ElementContainer<BoneController> BoneControllers;
		ElementContainer<HitBox> HitBoxes;
		ElementContainer<SequenceV10Xash> Sequences;
		ElementContainer<SequenceGroup> SequenceGroups;
		ElementContainer<TextureV10Xash> Textures;
		ElementContainer<Attachment> Attachments;
		ElementContainer<BodyGroup> BodyGroups;

		ElementContainer<Skin, NullElementUserData, SkinCollectionKey> Skins;

		// Owned by sequences:
		ElementContainer<Event, NullElementUserData, TOwnedItemKey<Event>> Events;
		ElementContainer<FootPivot, NullElementUserData, TOwnedItemKey<FootPivot>> FootPivots;
		ElementContainer<AnimationDataValueList, NullElementUserData, AnimationDataCollectionKey> AnimationData;

		// Owned by body groups:
		ElementContainer<ModelV10Xash, NullElementUserData, TOwnedItemKey<ModelV10Xash>> Models;

		// Owned by models:
		ElementContainer<MeshV10Xash, NullElementUserData, MeshCollectionKeyV10Xash> Meshes;
	};
}
