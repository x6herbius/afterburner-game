#pragma once

#include <vector>
#include <type_traits>
#include <cstring>
#include <cassert>
#include <cstddef>

namespace NFMDL
{
	template<typename T>
	class ElementArray
	{
	public:
		using ValueType = T;

		inline ElementArray()
		{
			static_assert(std::is_pod<ValueType>::value, "This container only works with POD elements.");
		}

		inline void AllocateAndZero(size_t elementCount)
		{
			m_Elements.resize(elementCount);
			memset(m_Elements.data(), 0, m_Elements.size() * sizeof(ValueType));
		}

		inline void CopyDataFrom(const ValueType* source, size_t numItems)
		{
			assert(numItems <= m_Elements.size());

			if ( numItems > 0 )
			{
				memcpy(m_Elements.data(), source, numItems * sizeof(ValueType));
			}
		}

		inline void Clear()
		{
			m_Elements.clear();
		}

		inline ValueType& operator [](size_t index)
		{
			assert(index < m_Elements.size());
			return m_Elements[index];
		}

		inline const ValueType& operator [](size_t index) const
		{
			assert(index < m_Elements.size());
			return m_Elements[index];
		}

		inline size_t Count() const
		{
			return m_Elements.size();
		}

		template<typename FUNC>
		inline void ForEach(const FUNC& callback) const
		{
			const size_t count = m_Elements.size();

			for ( uint32_t index = 0; index < count; ++index )
			{
				callback(index, m_Elements[index]);
			}
		}

	private:
		std::vector<ValueType> m_Elements;
	};
}
