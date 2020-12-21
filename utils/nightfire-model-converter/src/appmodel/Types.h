#pragma once

#include <cstdint>
#include <cstddef>
#include <vector>
#include <map>

#include "containers/ElementContainer.h"
#include "elements/AnimationValue.h"
#include "elements/ElementTraits.h"

namespace NFMDL
{
	struct AnimationDataCollectionKey
	{
		size_t sequenceIndex = INVALID_CONTAINER_INDEX;
		size_t blendIndex = INVALID_CONTAINER_INDEX;
		size_t boneIndex = INVALID_CONTAINER_INDEX;
		size_t componentIndex = INVALID_CONTAINER_INDEX;

		inline operator bool() const
		{
			return sequenceIndex != INVALID_CONTAINER_INDEX &&
				   blendIndex != INVALID_CONTAINER_INDEX &&
				   boneIndex != INVALID_CONTAINER_INDEX &&
				   componentIndex != INVALID_CONTAINER_INDEX;
		}

		inline bool operator ==(const AnimationDataCollectionKey& other) const
		{
			if ( static_cast<bool>(*this) != static_cast<bool>(other) )
			{
				return false;
			}

			if ( !static_cast<bool>(*this) && !static_cast<bool>(other) )
			{
				return true;
			}

			return sequenceIndex == other.sequenceIndex &&
				   blendIndex == other.blendIndex &&
				   boneIndex == other.boneIndex &&
				   componentIndex == other.componentIndex;
		}

		inline bool operator !=(const AnimationDataCollectionKey& other) const
		{
			return !(*this == other);
		}

		inline bool operator <(const AnimationDataCollectionKey& other) const
		{
			if ( sequenceIndex != other.sequenceIndex )
			{
				return sequenceIndex < other.sequenceIndex;
			}

			if ( blendIndex != other.blendIndex )
			{
				return blendIndex < other.blendIndex;
			}

			if ( boneIndex != other.boneIndex )
			{
				return boneIndex < other.boneIndex;
			}

			return componentIndex < other.componentIndex;
		}

		struct Hash
		{
			inline std::size_t operator()(const AnimationDataCollectionKey& key) const noexcept
			{
				size_t hash = std::hash<size_t>{}(key.sequenceIndex);
				hash = std::hash<size_t>{}(key.blendIndex) ^ (hash << 1);
				hash = std::hash<size_t>{}(key.boneIndex) ^ (hash << 1);
				hash = std::hash<size_t>{}(key.componentIndex) ^ (hash << 1);

				return hash;
			}
		};
	};

	struct SkinCollectionKey
	{
		size_t skinFamily = INVALID_CONTAINER_INDEX;
		size_t skinReference = INVALID_CONTAINER_INDEX;

		inline operator bool() const
		{
			return skinFamily != INVALID_CONTAINER_INDEX &&
				   skinReference != INVALID_CONTAINER_INDEX;
		}

		inline bool operator ==(const SkinCollectionKey& other) const
		{
			if ( static_cast<bool>(*this) != static_cast<bool>(other) )
			{
				return false;
			}

			if ( !static_cast<bool>(*this) && !static_cast<bool>(other) )
			{
				return true;
			}

			return skinFamily == other.skinFamily &&
				   skinReference == other.skinReference;
		}

		inline bool operator !=(const SkinCollectionKey& other) const
		{
			return !(*this == other);
		}

		inline bool operator <(const SkinCollectionKey& other) const
		{
			if ( skinFamily != other.skinFamily )
			{
				return skinFamily < other.skinFamily;
			}

			return skinReference < other.skinReference;
		}

		struct Hash
		{
			inline std::size_t operator()(const SkinCollectionKey& key) const noexcept
			{
				size_t hash = std::hash<size_t>{}(key.skinFamily);
				hash = std::hash<size_t>{}(key.skinReference) ^ (hash << 1);

				return hash;
			}
		};
	};

	struct MeshCollectionKeyV14
	{
		size_t modelIndex = INVALID_CONTAINER_INDEX;
		size_t modelInfoIndex = INVALID_CONTAINER_INDEX;
		size_t meshIndex = INVALID_CONTAINER_INDEX;

		inline operator bool() const
		{
			return modelIndex != INVALID_CONTAINER_INDEX &&
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

			return modelIndex == other.modelIndex &&
				   modelInfoIndex == other.modelInfoIndex &&
				   meshIndex == other.meshIndex;
		}

		inline bool operator !=(const MeshCollectionKeyV14& other) const
		{
			return !(*this == other);
		}

		inline bool operator <(const MeshCollectionKeyV14& other) const
		{
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
				size_t hash = std::hash<size_t>{}(key.modelIndex);
				hash = std::hash<size_t>{}(key.modelInfoIndex) ^ (hash << 1);
				hash = std::hash<size_t>{}(key.meshIndex) ^ (hash << 1);

				return hash;
			}
		};
	};

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

	class AnimationDataValueList : public std::vector<decltype(AnimationValue::value)>
	{
	public:
		using ValueType = decltype(AnimationValue::value);
	};

	template<>
	struct ElementTraits<AnimationDataValueList>
	{
		static constexpr const char* ELEMENT_NAME = "AnimationDataValueList";
	};

	// So that we can specialise the stream operator when dumping keys
	// from containers of different types.
	template<typename T>
	struct TOwnedItemKey : public OwnedItemKey {};

	template<typename T>
	using OwnedItemCollection = std::map<TOwnedItemKey<T>, T>;
}
