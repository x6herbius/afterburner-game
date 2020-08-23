#pragma once

#include <ostream>
#include "HeaderV14.h"
#include "Bone.h"
#include "BoneController.h"
#include "SequenceV14.h"

std::ostream& operator <<(std::ostream& stream, const NFMDL::HeaderV14& header);
std::ostream& operator <<(std::ostream& stream, const NFMDL::Bone& bone);
std::ostream& operator <<(std::ostream& stream, const NFMDL::BoneController& boneController);
std::ostream& operator <<(std::ostream& stream, const NFMDL::SequenceV14& sequence);
