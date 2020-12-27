#pragma once

#include <cstdint>
#include "Types.h"
#include "ElementTraits.h"

namespace NFMDL
{
#pragma pack(push, 1)
	struct ModelInfoV14
	{
		uint32_t skinReference;
		CountOffsetPair meshes;
	};
#pragma pack(pop)

	class ModelInfoUserDataV14 : public BaseElementUserData
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
		size_t m_IndexInHeader = INVALID_CONTAINER_INDEX;
		size_t m_ChildMeshesBeginIndex = INVALID_CONTAINER_INDEX;
		size_t m_ChildMeshesCount = 0;
	};

	struct ModelInfoCollectionKeyV14
	{
		size_t bodyGroupIndex = INVALID_CONTAINER_INDEX;
		size_t modelIndex = INVALID_CONTAINER_INDEX;
		size_t modelInfoIndex = INVALID_CONTAINER_INDEX;

		inline operator bool() const
		{
			return bodyGroupIndex != INVALID_CONTAINER_INDEX &&
				   modelIndex != INVALID_CONTAINER_INDEX &&
				   modelInfoIndex != INVALID_CONTAINER_INDEX;
		}

		inline bool operator ==(const ModelInfoCollectionKeyV14& other) const
		{
			if ( static_cast<bool>(*this) != static_cast<bool>(other) )
			{
				return false;
			}

			if ( !static_cast<bool>(*this) && !static_cast<bool>(other) )
			{
				return true;
			}

			return bodyGroupIndex == other.bodyGroupIndex &&
				   modelIndex == other.modelIndex &&
				   modelInfoIndex == other.modelInfoIndex;
		}

		inline bool operator !=(const ModelInfoCollectionKeyV14& other) const
		{
			return !(*this == other);
		}

		inline bool operator <(const ModelInfoCollectionKeyV14& other) const
		{
			if ( bodyGroupIndex != other.bodyGroupIndex )
			{
				return bodyGroupIndex < other.bodyGroupIndex;
			}

			if ( modelIndex != other.modelIndex )
			{
				return modelIndex < other.modelIndex;
			}

			return modelInfoIndex < other.modelInfoIndex;
		}

		struct Hash
		{
			inline std::size_t operator()(const ModelInfoCollectionKeyV14& key) const noexcept
			{
				size_t hash = std::hash<size_t>{}(key.bodyGroupIndex);
				hash = std::hash<size_t>{}(key.modelIndex) ^ (hash << 1);
				hash = std::hash<size_t>{}(key.modelInfoIndex) ^ (hash << 1);

				return hash;
			}
		};
	};

	template<>
	struct ElementTraits<ModelInfoV14>
	{
		static constexpr const char* ELEMENT_NAME = "ModelInfoV14";
	};
}
