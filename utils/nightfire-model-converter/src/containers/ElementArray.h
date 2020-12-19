#pragma once

#include <vector>
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

		inline void AllocateDefault(size_t elementCount)
		{
			m_Elements.resize(elementCount);
		}

		template<typename T2>
		inline void AllocateFrom(const T2* source, size_t numItems)
		{
			ConstructElementsGeneric(source, numItems, true);
		}

		inline void AppendDefault(size_t elementCount)
		{
			m_Elements.resize(m_Elements.size() + elementCount);
		}

		template<typename T2>
		inline void AppendFrom(const T2* source, size_t numItems)
		{
			ConstructElementsGeneric(source, numItems, false);
		}

		inline void Clear()
		{
			m_Elements.clear();
		}

		inline const ValueType* ConstData() const
		{
			return m_Elements.data();
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

		template<typename FUNC>
		inline void MutateEach(const FUNC& callback)
		{
			const size_t count = m_Elements.size();

			for ( uint32_t index = 0; index < count; ++index )
			{
				callback(index, m_Elements[index]);
			}
		}

		template<typename FUNC>
		inline const ValueType* FindFirstMatching(const FUNC& predicate) const
		{
			const size_t count = m_Elements.size();

			for ( uint32_t index = 0; index < count; ++index )
			{
				if ( predicate(index, m_Elements[index]) )
				{
					return &m_Elements[index];
				}
			}

			return nullptr;
		}

	private:
		template<typename T2>
		inline void ConstructElementsGeneric(const T2* source, size_t numItems, bool clearFirst)
		{
			if ( clearFirst )
			{
				m_Elements.clear();
			}

			if ( !source || numItems < 1 )
			{
				return;
			}

			m_Elements.reserve(m_Elements.size() + numItems);

			for ( size_t index = 0; index < numItems; ++index )
			{
				m_Elements.emplace_back(source[index]);
			}
		}

		std::vector<ValueType> m_Elements;
	};
}