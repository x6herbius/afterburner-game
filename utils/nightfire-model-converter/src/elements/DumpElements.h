#pragma once

#include <ostream>
#include "appmodel/NightfireModelFile.h"
#include "elements/HeaderV14.h"
#include "elements/HeaderV10Xash.h"
#include "elements/Bone.h"
#include "elements/BoneController.h"
#include "elements/SequenceV14.h"
#include "elements/HitBox.h"
#include "elements/SequenceGroup.h"
#include "elements/TextureV14.h"
#include "elements/Attachment.h"
#include "elements/SoundGroupV14.h"
#include "elements/SoundV14.h"
#include "elements/LevelOfDetailV14.h"
#include "elements/BodyGroup.h"
#include "elements/TriangleMapV14.h"
#include "elements/VertexV14.h"
#include "elements/NormalV14.h"
#include "elements/TextureCoOrdinateV14.h"
#include "elements/VertexBlendV14.h"
#include "elements/VertexBlendScaleV14.h"
#include "elements/BoneFixUpV14.h"
#include "elements/Skin.h"
#include "elements/ModelV14.h"
#include "elements/ModelInfoV14.h"
#include "elements/MeshV14.h"
#include "elements/AnimationDataValueList.h"
#include "elements/OwnedItemKey.h"

// Elements
std::ostream& operator <<(std::ostream& stream, const NFMDL::HeaderV14& header);
std::ostream& operator <<(std::ostream& stream, const NFMDL::HeaderV10Xash& header);
std::ostream& operator <<(std::ostream& stream, const NFMDL::Bone& bone);
std::ostream& operator <<(std::ostream& stream, const NFMDL::BoneController& boneController);
std::ostream& operator <<(std::ostream& stream, const NFMDL::SequenceV14& sequence);
std::ostream& operator <<(std::ostream& stream, const NFMDL::HitBox& hitbox);
std::ostream& operator <<(std::ostream& stream, const NFMDL::SequenceGroup& sequenceGroup);
std::ostream& operator <<(std::ostream& stream, const NFMDL::TextureV14& texture);
std::ostream& operator <<(std::ostream& stream, const NFMDL::Attachment& attachment);
std::ostream& operator <<(std::ostream& stream, const NFMDL::SoundGroupV14& soundGroup);
std::ostream& operator <<(std::ostream& stream, const NFMDL::SoundV14& sound);
std::ostream& operator <<(std::ostream& stream, const NFMDL::LevelOfDetailV14& lod);
std::ostream& operator <<(std::ostream& stream, const NFMDL::BodyGroup& bodyGroup);
std::ostream& operator <<(std::ostream& stream, const NFMDL::TriangleMapV14& triangleMap);
std::ostream& operator <<(std::ostream& stream, const NFMDL::VertexV14& vertex);
std::ostream& operator <<(std::ostream& stream, const NFMDL::NormalV14& normal);
std::ostream& operator <<(std::ostream& stream, const NFMDL::TextureCoOrdinateV14& texCoOrd);
std::ostream& operator <<(std::ostream& stream, const NFMDL::VertexBlendV14& vBlend);
std::ostream& operator <<(std::ostream& stream, const NFMDL::VertexBlendScaleV14& vBlendScale);
std::ostream& operator <<(std::ostream& stream, const NFMDL::BoneFixUpV14& boneFixUp);
std::ostream& operator <<(std::ostream& stream, const NFMDL::Skin& skin);
std::ostream& operator <<(std::ostream& stream, const NFMDL::ModelV14& model);
std::ostream& operator <<(std::ostream& stream, const NFMDL::ModelInfoV14& modelInfo);
std::ostream& operator <<(std::ostream& stream, const NFMDL::MeshV14& mesh);
std::ostream& operator <<(std::ostream& stream, const NFMDL::AnimationDataValueList& list);
std::ostream& operator <<(std::ostream& stream, const NFMDL::Event& event);
std::ostream& operator <<(std::ostream& stream, const NFMDL::FootPivot& footPivot);

// Keys
std::ostream& operator <<(std::ostream& stream, const NFMDL::SkinCollectionKey& key);
std::ostream& operator <<(std::ostream& stream, const NFMDL::TOwnedItemKey<NFMDL::ModelInfoV14>& key);
std::ostream& operator <<(std::ostream& stream, const NFMDL::MeshCollectionKeyV14& key);
std::ostream& operator <<(std::ostream& stream, const NFMDL::TOwnedItemKey<NFMDL::SoundV14>& key);
std::ostream& operator <<(std::ostream& stream, const NFMDL::AnimationDataCollectionKey& key);
std::ostream& operator <<(std::ostream& stream, const NFMDL::TOwnedItemKey<NFMDL::Event>& key);
std::ostream& operator <<(std::ostream& stream, const NFMDL::TOwnedItemKey<NFMDL::FootPivot>& key);
