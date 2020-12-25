#pragma once

#include <cstdint>
#include "Types.h"
#include "ElementTraits.h"

namespace NFMDL
{
#pragma pack(push, 1)
	struct MeshV10Xash
	{
		CountOffsetPair triangles;
		uint32_t skinref;
		CountOffsetPair normals;
	};
#pragma pack(pop)

	struct MeshCollectionKeyV10Xash
	{
		size_t bodyGroupIndex = INVALID_CONTAINER_INDEX;
		size_t modelIndex = INVALID_CONTAINER_INDEX;
		size_t meshIndex = INVALID_CONTAINER_INDEX;

		inline operator bool() const
		{
			return bodyGroupIndex != INVALID_CONTAINER_INDEX &&
				   modelIndex != INVALID_CONTAINER_INDEX &&
				   meshIndex != INVALID_CONTAINER_INDEX;
		}

		inline bool operator ==(const MeshCollectionKeyV10Xash& other) const
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
				   meshIndex == other.meshIndex;
		}

		inline bool operator !=(const MeshCollectionKeyV10Xash& other) const
		{
			return !(*this == other);
		}

		inline bool operator <(const MeshCollectionKeyV10Xash& other) const
		{
			if ( bodyGroupIndex != other.bodyGroupIndex )
			{
				return bodyGroupIndex < other.bodyGroupIndex;
			}

			if ( modelIndex != other.modelIndex )
			{
				return modelIndex < other.modelIndex;
			}

			return meshIndex < other.meshIndex;
		}

		struct Hash
		{
			inline std::size_t operator()(const MeshCollectionKeyV10Xash& key) const noexcept
			{
				size_t hash = std::hash<size_t>{}(key.bodyGroupIndex);
				hash = std::hash<size_t>{}(key.modelIndex) ^ (hash << 1);
				hash = std::hash<size_t>{}(key.meshIndex) ^ (hash << 1);

				return hash;
			}
		};
	};

	template<>
	struct ElementTraits<MeshV10Xash>
	{
		static constexpr const char* ELEMENT_NAME = "MeshV10Xash";
	};
}
