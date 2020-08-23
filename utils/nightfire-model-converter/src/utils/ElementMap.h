#pragma once

#include <map>
#include <type_traits>
#include <cstring>
#include <cassert>

namespace NFMDL
{
	template<typename T, typename I = uint32_t>
	class ElementMap
	{
	public:
		using IndexType = I;
		using ValueType = T;

		inline ElementMap()
		{
			static_assert(std::is_pod<ValueType>::value, "This container only works with POD elements.");
		}

		inline void Clear()
		{
			m_Elements.clear();
		}

		inline bool Contains(const IndexType& index) const
		{
			return m_Elements.find(index) != m_Elements.end();
		}

		inline ValueType& operator [](const IndexType& index)
		{
			assert(Contains(index));
			return m_Elements[index];
		}

		inline const ValueType& operator [](const IndexType& index) const
		{
			assert(Contains(index));
			return m_Elements[index];
		}

		template<typename... Args>
		inline ValueType& Emplace(Args&&... args)
		{
			return m_Elements.emplace(std::forward<Args>(args)...).first;
		}

		inline size_t Count() const
		{
			return m_Elements.size();
		}

	private:
		std::map<IndexType, ValueType> m_Elements;
	};
}
