#pragma once

#include <cstddef>
#include "containers/ElementContainer.h"

namespace NFMDL
{
	struct OwnedItemKey
	{
		size_t ownerIndex = INVALID_CONTAINER_INDEX;
		size_t itemIndex = INVALID_CONTAINER_INDEX;

		inline operator bool() const
		{
			return ownerIndex != INVALID_CONTAINER_INDEX &&
				   itemIndex != INVALID_CONTAINER_INDEX;
		}

		inline bool operator ==(const OwnedItemKey& other) const
		{
			if ( static_cast<bool>(*this) != static_cast<bool>(other) )
			{
				return false;
			}

			if ( !static_cast<bool>(*this) && !static_cast<bool>(other) )
			{
				return true;
			}

			return ownerIndex == other.ownerIndex &&
					itemIndex == other.itemIndex;
		}

		inline bool operator !=(const OwnedItemKey& other) const
		{
			return !(*this == other);
		}

		inline bool operator <(const OwnedItemKey& other) const
		{
			if ( ownerIndex != other.ownerIndex )
			{
				return ownerIndex < other.ownerIndex;
			}

			return itemIndex < other.itemIndex;
		}

		struct Hash
		{
			inline std::size_t operator()(const OwnedItemKey& key) const noexcept
			{
				size_t hash = std::hash<size_t>{}(key.ownerIndex);
				hash = std::hash<size_t>{}(key.itemIndex) ^ (hash << 1);

				return hash;
			}
		};
	};

	// So that we can specialise the stream operator when dumping keys
	// from containers of different types.
	template<typename T>
	struct TOwnedItemKey : public OwnedItemKey {};
}
