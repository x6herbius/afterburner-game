#pragma once

#include <ostream>
#include "appmodel/NightfireModelFile.h"
#include "HeaderV14.h"
#include "Bone.h"
#include "BoneController.h"
#include "SequenceV14.h"
#include "HitBox.h"
#include "SequenceGroup.h"
#include "TextureV14.h"
#include "Attachment.h"
#include "SoundGroupV14.h"
#include "SoundV14.h"
#include "LevelOfDetailV14.h"
#include "BodyGroupV14.h"
#include "TriangleMapV14.h"
#include "VertexV14.h"
#include "NormalV14.h"
#include "TextureCoOrdinateV14.h"
#include "VertexBlendV14.h"
#include "VertexBlendScaleV14.h"
#include "BoneFixUpV14.h"
#include "Skin.h"
#include "ModelV14.h"
#include "ModelInfoV14.h"
#include "MeshV14.h"

std::ostream& operator <<(std::ostream& stream, const NFMDL::HeaderV14& header);
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
std::ostream& operator <<(std::ostream& stream, const NFMDL::BodyGroupV14& bodyGroup);
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

std::ostream& operator <<(std::ostream& stream, const NFMDL::NightfireModelFile::SkinCollectionKey& key);
std::ostream& operator <<(std::ostream& stream, const NFMDL::NightfireModelFile::TOwnedItemKey<NFMDL::ModelInfoV14>& key);
std::ostream& operator <<(std::ostream& stream, const NFMDL::NightfireModelFile::MeshCollectionKey& key);
std::ostream& operator <<(std::ostream& stream, const NFMDL::NightfireModelFile::TOwnedItemKey<NFMDL::SoundV14>& key);
