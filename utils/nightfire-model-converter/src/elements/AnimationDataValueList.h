#pragma once

#include <cstdint>
#include <vector>
#include "ElementTraits.h"
#include "AnimationValue.h"

namespace NFMDL
{
	class AnimationDataValueList : public std::vector<decltype(AnimationValue::value)>
	{
	public:
		using ValueType = decltype(AnimationValue::value);
	};

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

	template<>
	struct ElementTraits<AnimationDataValueList>
	{
		static constexpr const char* ELEMENT_NAME = "AnimationDataValueList";
	};
}
