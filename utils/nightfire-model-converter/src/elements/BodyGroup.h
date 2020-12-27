#pragma once

#include <cstdint>
#include "ElementTraits.h"
#include "BaseElementUserData.h"
#include "containers/ElementContainer.h"

namespace NFMDL
{
#pragma pack(push, 1)
	struct BodyGroup
	{
		char name[64];
		uint32_t modelCount;
		uint32_t bodyCount;
		uint32_t modelOffset;
	};
#pragma pack(pop)

	class BodyGroupUserDataV14 : public BaseElementUserData
	{
	public:
		inline size_t ChildModelsBeginIndex() const
		{
			return m_ChildModelsBeginIndex;
		}

		inline void SetChildModelsBeginIndex(size_t index)
		{
			m_ChildModelsBeginIndex = index;
		}

		inline size_t ChildModelsCount() const
		{
			return m_ChildModelsCount;
		}

		inline void SetChildModelsCount(size_t count)
		{
			m_ChildModelsCount = count;
		}

		inline void IncrementChildModelsCount()
		{
			++m_ChildModelsCount;
		}

	private:
		size_t m_ChildModelsBeginIndex = INVALID_CONTAINER_INDEX;
		size_t m_ChildModelsCount = 0;
	};

	class BodyGroupUserDataV10Xash
	{
	public:
		inline size_t ChildModelsBeginIndex() const
		{
			return m_ChildModelsBeginIndex;
		}

		inline void SetChildModelsBeginIndex(size_t index)
		{
			m_ChildModelsBeginIndex = index;
		}

		inline size_t ChildModelsCount() const
		{
			return m_ChildModelsCount;
		}

		inline void SetChildModelsCount(size_t count)
		{
			m_ChildModelsCount = count;
		}

		inline void IncrementChildModelsCount()
		{
			++m_ChildModelsCount;
		}

	private:
		size_t m_ChildModelsBeginIndex = INVALID_CONTAINER_INDEX;
		size_t m_ChildModelsCount = 0;
	};

	template<>
	struct ElementTraits<BodyGroup>
	{
		static constexpr const char* ELEMENT_NAME = "BodyGroup";
	};
}
