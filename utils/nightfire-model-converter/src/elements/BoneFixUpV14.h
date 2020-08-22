#pragma once

#include <cstdint>

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
}
