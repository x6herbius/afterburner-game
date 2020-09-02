#pragma once

#include "utils/ElementArray.h"
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

namespace NFMDL
{
	class XashModelFile
	{
	public:
		using AnimationDataCollection = std::map<AnimationDataCollectionKey, AnimationDataValueList>;
		using EventCollection = OwnedItemCollection<Event>;
		using FootPivotCollection = OwnedItemCollection<FootPivot>;
		using SkinCollection = std::map<SkinCollectionKey, Skin>;

		HeaderV10Xash Header;

		ElementArray<Bone> Bones;
		ElementArray<BoneController> BoneControllers;
		ElementArray<HitBox> HitBoxes;
		ElementArray<SequenceV10Xash> Sequences;
		ElementArray<SequenceGroup> SequenceGroups;
		ElementArray<TextureV10Xash> Textures;
		ElementArray<Attachment> Attachments;

		SkinCollection Skins;

		// Owned by sequences:
		EventCollection Events;
		FootPivotCollection FootPivots;
		AnimationDataCollection AnimationData;
	};
}
