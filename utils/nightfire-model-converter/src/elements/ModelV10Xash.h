#pragma once

#include <cstdint>
#include "Types.h"
#include "ElementTraits.h"

namespace NFMDL
{
#pragma pack(push, 1)
	struct ModelV10Xash
	{
		char name[64];

		uint32_t type;
		float boundingRadius;

		CountOffsetPair meshes;

		uint32_t vertexCount;
		uint32_t vertexParentBonesOffset; // Parent bone index per vertex
		uint32_t verticesOffset;
		uint32_t normalCount;
		uint32_t normalParentBonesOffset; // Parent bone index per normal
		uint32_t normalsOffset;

		uint32_t vertexBlendInfoOffset;
		uint32_t normalBlendInfoOffset;
	};
#pragma pack(pop)

	class ModelUserDataV10Xash
	{
	public:
		inline size_t ChildMeshesBeginIndex() const
		{
			return m_ChildMeshesBeginIndex;
		}

		inline void SetChildMeshesBeginIndex(size_t index)
		{
			m_ChildMeshesBeginIndex = index;
		}

		inline size_t ChildMeshesCount() const
		{
			return m_ChildMeshesCount;
		}

		inline void SetChildMeshesCount(size_t count)
		{
			m_ChildMeshesCount = count;
		}

		inline void IncrementChildMeshesCount()
		{
			++m_ChildMeshesCount;
		}

	private:
		size_t m_ChildMeshesBeginIndex = INVALID_CONTAINER_INDEX;
		size_t m_ChildMeshesCount = 0;
	};

	template<>
	struct ElementTraits<ModelV10Xash>
	{
		static constexpr const char* ELEMENT_NAME = "ModelV10Xash";
	};
}
