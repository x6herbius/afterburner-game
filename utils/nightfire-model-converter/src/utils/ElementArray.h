#pragma once

#include <vector>
#include <type_traits>
#include <cstring>
#include <cassert>

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

	private:
		std::vector<ValueType> m_Elements;
	};
}
