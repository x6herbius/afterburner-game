#pragma once

#include <cstdint>
#include <cstddef>
#include <vector>
#include <map>

#include "elements/AnimationValue.h"
#include "elements/ElementTraits.h"

namespace NFMDL
{
	struct AnimationDataCollectionKey
	{
		uint32_t sequenceIndex = 0;
		uint32_t blendIndex = 0;
		uint32_t boneIndex = 0;
		uint32_t componentIndex = 0;

		inline bool operator ==(const AnimationDataCollectionKey& other) const
		{
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
				size_t hash = std::hash<uint32_t>{}(key.sequenceIndex);
				hash = std::hash<uint32_t>{}(key.blendIndex) ^ (hash << 1);
				hash = std::hash<uint32_t>{}(key.boneIndex) ^ (hash << 1);
				hash = std::hash<uint32_t>{}(key.componentIndex) ^ (hash << 1);

				return hash;
			}
		};
	};

	struct SkinCollectionKey
	{
		uint32_t skinFamily = 0;
		uint32_t skinReference = 0;

		inline bool operator ==(const SkinCollectionKey& other) const
		{
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
				size_t hash = std::hash<uint32_t>{}(key.skinFamily);
				hash = std::hash<uint32_t>{}(key.skinReference) ^ (hash << 1);

				return hash;
			}
		};
	};

	struct MeshCollectionKeyV14
	{
		uint32_t modelIndex = 0;
		uint32_t modelInfoIndex = 0;
		uint32_t meshIndex = 0;

		inline bool operator ==(const MeshCollectionKeyV14& other) const
		{
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
				size_t hash = std::hash<uint32_t>{}(key.modelIndex);
				hash = std::hash<uint32_t>{}(key.modelInfoIndex) ^ (hash << 1);
				hash = std::hash<uint32_t>{}(key.meshIndex) ^ (hash << 1);

				return hash;
			}
		};
	};

		struct MeshCollectionKeyV10Xash
	{
		uint32_t bodyGroupIndex = 0;
		uint32_t modelIndex = 0;
		uint32_t meshIndex = 0;

		inline bool operator ==(const MeshCollectionKeyV10Xash& other) const
		{
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
				size_t hash = std::hash<uint32_t>{}(key.bodyGroupIndex);
				hash = std::hash<uint32_t>{}(key.modelIndex) ^ (hash << 1);
				hash = std::hash<uint32_t>{}(key.meshIndex) ^ (hash << 1);

				return hash;
			}
		};
	};

	struct OwnedItemKey
	{
		uint32_t ownerIndex = 0;
		uint32_t itemIndex = 0;

		inline bool operator ==(const OwnedItemKey& other) const
		{
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
				size_t hash = std::hash<uint32_t>{}(key.ownerIndex);
				hash = std::hash<uint32_t>{}(key.itemIndex) ^ (hash << 1);

				return hash;
			}
		};
	};

	class AnimationDataValueList : public std::vector<decltype(AnimationValue::value)>
	{
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
