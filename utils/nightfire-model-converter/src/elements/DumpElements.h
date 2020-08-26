#pragma once

#include <ostream>
#include "HeaderV14.h"
#include "Bone.h"
#include "BoneController.h"
#include "SequenceV14.h"
#include "HitBox.h"
#include "SequenceGroup.h"
#include "TextureV14.h"
#include "Attachment.h"
#include "SoundGroupV14.h"
#include "LevelOfDetailV14.h"
#include "BodyGroupV14.h"

std::ostream& operator <<(std::ostream& stream, const NFMDL::HeaderV14& header);
std::ostream& operator <<(std::ostream& stream, const NFMDL::Bone& bone);
std::ostream& operator <<(std::ostream& stream, const NFMDL::BoneController& boneController);
std::ostream& operator <<(std::ostream& stream, const NFMDL::SequenceV14& sequence);
std::ostream& operator <<(std::ostream& stream, const NFMDL::HitBox& hitbox);
std::ostream& operator <<(std::ostream& stream, const NFMDL::SequenceGroup& sequenceGroup);
std::ostream& operator <<(std::ostream& stream, const NFMDL::TextureV14& texture);
std::ostream& operator <<(std::ostream& stream, const NFMDL::Attachment& attachment);
std::ostream& operator <<(std::ostream& stream, const NFMDL::SoundGroupV14& soundGroup);
std::ostream& operator <<(std::ostream& stream, const NFMDL::LevelOfDetailV14& lod);
std::ostream& operator <<(std::ostream& stream, const NFMDL::BodyGroupV14& bodyGroup);
