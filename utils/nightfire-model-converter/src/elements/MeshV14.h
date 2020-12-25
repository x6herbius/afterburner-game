#pragma once

#include <cstdint>
#include "ElementTraits.h"

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

	struct MeshCollectionKeyV14
	{
		size_t bodyGroupIndex = INVALID_CONTAINER_INDEX;
		size_t modelIndex = INVALID_CONTAINER_INDEX;
		size_t modelInfoIndex = INVALID_CONTAINER_INDEX;
		size_t meshIndex = INVALID_CONTAINER_INDEX;

		inline operator bool() const
		{
			return bodyGroupIndex != INVALID_CONTAINER_INDEX &&
				   modelIndex != INVALID_CONTAINER_INDEX &&
				   modelInfoIndex != INVALID_CONTAINER_INDEX &&
				   meshIndex != INVALID_CONTAINER_INDEX;
		}

		inline bool operator ==(const MeshCollectionKeyV14& other) const
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
				   modelInfoIndex == other.modelInfoIndex &&
				   meshIndex == other.meshIndex;
		}

		inline bool operator !=(const MeshCollectionKeyV14& other) const
		{
			return !(*this == other);
		}

		inline bool operator <(const MeshCollectionKeyV14& other) const
		{
			if ( bodyGroupIndex != other.bodyGroupIndex )
			{
				return bodyGroupIndex < other.bodyGroupIndex;
			}

			if ( modelIndex != other.modelIndex )
			{
				return modelIndex < other.modelIndex;
			}

			if ( modelInfoIndex != other.modelInfoIndex )
			{
				return modelInfoIndex < other.modelInfoIndex;
			}

			return meshIndex < other.meshIndex;
		}

		struct Hash
		{
			inline std::size_t operator()(const MeshCollectionKeyV14& key) const noexcept
			{
				size_t hash = std::hash<size_t>{}(key.bodyGroupIndex);
				hash = std::hash<size_t>{}(key.modelIndex) ^ (hash << 1);
				hash = std::hash<size_t>{}(key.modelInfoIndex) ^ (hash << 1);
				hash = std::hash<size_t>{}(key.meshIndex) ^ (hash << 1);

				return hash;
			}
		};
	};

	template<>
	struct ElementTraits<MeshV14>
	{
		static constexpr const char* ELEMENT_NAME = "MeshV14";
	};
}
