#pragma once

#include <cstdint>

namespace NFMDL
{
#pragma pack(push, 1)
	struct MeshV14
	{
		int8_t boneMappings[24];
		uint16_t trianglesIndex;
		uint16_t trianglesCount;
		uint16_t verticesIndexFrom;
		uint16_t verticesIndexTo;
	};
#pragma pack(pop)
}
