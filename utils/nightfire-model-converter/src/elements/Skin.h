#pragma once

#include <cstdint>
#include "ElementTraits.h"

namespace NFMDL
{
#pragma pack(push, 1)
	struct Skin
	{
		int16_t value;
	};
#pragma pack(pop)

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

	template<>
	struct ElementTraits<Skin>
	{
		static constexpr const char* ELEMENT_NAME = "Skin";
	};
}
