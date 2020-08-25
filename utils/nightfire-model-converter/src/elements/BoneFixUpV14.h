#pragma once

#include <cstdint>
#include "ElementTraits.h"

namespace NFMDL
{
#pragma pack(push, 1)
	struct BoneFixUpV14
	{
		// Essentially a 3x4 matrix.
		float xScale;
		float xSkewY;
		float xSkewZ;
		float xPosition;

		float ySkewX;
		float yScale;
		float ySkewZ;
		float yPosition;

		float zSkewX;
		float zSkewY;
		float zScale;
		float zPosition;
	};
#pragma pack(pop)

	template<>
	struct ElementTraits<BoneFixUpV14>
	{
		static constexpr const char* ELEMENT_NAME = "BoneFixUpV14";
	};
}
