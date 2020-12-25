#pragma once

#include <cstdint>
#include "ElementTraits.h"
#include "BaseElementUserData.h"

namespace NFMDL
{
#pragma pack(push, 1)
	struct ModelV14
	{
		char name[32];
		uint32_t index;
		uint32_t modelInfoOffset[24];
	};
#pragma pack(pop)

	class ModelUserDataV14 : public BaseElementUserData
	{
	public:
		inline size_t IndexInHeader() const
		{
			return m_IndexInHeader;
		}

		inline void SetIndexInHeader(size_t index)
		{
			m_IndexInHeader = index;
		}

	private:
		size_t m_IndexInHeader = INVALID_CONTAINER_INDEX;
	};

	template<>
	struct ElementTraits<ModelV14>
	{
		static constexpr const char* ELEMENT_NAME = "ModelV14";
	};
}
