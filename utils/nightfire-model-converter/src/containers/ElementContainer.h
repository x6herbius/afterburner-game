#pragma once

#include <cassert>
#include <vector>
#include <map>
#include <memory>
#include <type_traits>

namespace NFMDL
{
	static constexpr size_t INVALID_CONTAINER_INDEX = ~0;

	struct NullElementUserData
	{
	};

	struct NullElementKey
	{
		inline operator bool() const
		{
			return false;
		}
	};

	template<typename T, typename U = NullElementUserData, typename K = NullElementKey>
	class ElementContainer
	{
	public:
		using ElementType = T;
		using UserDataType = U;
		using KeyType = K;

		static constexpr bool ELEMENT_IS_POD = std::is_pod<ElementType>::value;

		class BaseIterator;
		class ConstIterator;
		class Iterator;

		struct ConstIteratorData
		{
			size_t index = INVALID_CONTAINER_INDEX;
			const ElementType* element = nullptr;
			const UserDataType* userData = nullptr;
			const KeyType* key = nullptr;
		};

		struct IteratorData
		{
			size_t index = INVALID_CONTAINER_INDEX;
			ElementType* element = nullptr;
			UserDataType* userData = nullptr;
			const KeyType* key = nullptr;
		};

		inline void Clear()
		{
			m_Items.clear();
			m_KeyToItemIndex.clear();
		}

		inline size_t Count() const
		{
			return m_Items.size();
		}

		inline void AllocateDefault(size_t itemCount)
		{
			ConstructItems(itemCount, true);
		}

		inline void AppendDefault(size_t itemCount = 1)
		{
			ConstructItems(itemCount, false);
		}

		template<typename T2>
		inline void AllocateFrom(const T2* source, size_t itemCount)
		{
			ConstructItemsFrom(source, itemCount, true);
		}

		template<typename T2>
		inline void AppendFrom(const T2* source, size_t itemCount)
		{
			ConstructItemsFrom(source, itemCount, false);
		}

		inline void AssignMapping(const KeyType& key, size_t index)
		{
			if ( !key || index >= m_Items.size() )
			{
				return;
			}

			RemoveMapping(key);

			m_KeyToItemIndex[key] = index;
			m_Items[index].key = key;
		}

		inline void AssignMappingAndValue(const KeyType& key, size_t index, const ElementType& element)
		{
			if ( !key || index >= m_Items.size() )
			{
				return;
			}

			AssignMapping(key, index);
			m_Items[index].element = element;
		}

		inline void AssignMappingAndValue(const KeyType& key, size_t index, ElementType&& element)
		{
			if ( !key || index >= m_Items.size() )
			{
				return;
			}

			AssignMapping(key, index);
			m_Items[index].element = std::move(element);
		}

		inline void RemoveMapping(const KeyType& key)
		{
			if ( !key )
			{
				return;
			}

			KeyToItemIndexMap::iterator it = m_KeyToItemIndex.find(key);

			if ( it != m_KeyToItemIndex.end() )
			{
				assert(it->second < m_Items.size());

				m_Items[it->second].key = KeyType();
				m_KeyToItemIndex.erase(key);
			}
		}

		inline const ElementType* ElementAt(size_t index) const
		{
			return index < m_Items.size() ? &m_Items[index].element : nullptr;
		}

		inline ElementType* ElementAt(size_t index)
		{
			return index < m_Items.size() ? &m_Items[index].element : nullptr;
		}

		inline const ElementType* ElementAt(const KeyType& key) const
		{
			const KeyToItemIndexMap::const_iterator it = m_KeyToItemIndex.find(key);
			return it != m_KeyToItemIndex.end() ? ElementAt(it->second) : nullptr;
		}

		inline ElementType* ElementAt(const KeyType& key)
		{
			const KeyToItemIndexMap::iterator it = m_KeyToItemIndex.find(key);
			return it != m_KeyToItemIndex.end() ? ElementAt(it->second) : nullptr;
		}

		inline const UserDataType* UserDataAt(size_t index) const
		{
			return index < m_Items.size() ? &m_Items[index].userData : nullptr;
		}

		inline UserDataType* UserDataAt(size_t index)
		{
			return index < m_Items.size() ? &m_Items[index].userData : nullptr;
		}

		inline const UserDataType* UserDataAt(const KeyType& key) const
		{
			const KeyToItemIndexMap::const_iterator it = m_KeyToItemIndex.find(key);
			return it != m_KeyToItemIndex.end() ? UserDataAt(it->second) : nullptr;
		}

		inline UserDataType* UserDataAt(const KeyType& key)
		{
			const KeyToItemIndexMap::iterator it = m_KeyToItemIndex.find(key);
			return it != m_KeyToItemIndex.end() ? UserDataAt(it->second) : nullptr;
		}

		inline const KeyType& KeyFor(size_t index) const
		{
			static const KeyType invalidKey;

			return index < m_Items.size() ? m_Items[index].key : invalidKey;
		}

		inline size_t IndexForKey(const KeyType& key) const
		{
			const KeyToItemIndexMap::iterator it = m_KeyToItemIndex.find(key);
			return it != m_KeyToItemIndex.end() ? it->second : INVALID_CONTAINER_INDEX;
		}

		inline ConstIterator begin() const
		{
			return ConstIterator(this, 0);
		}

		inline ConstIterator end() const
		{
			return ConstIterator();
		}

		inline Iterator begin()
		{
			return Iterator(this, 0);
		}

		inline Iterator end()
		{
			return Iterator();
		}

		inline ConstIterator ConstIteratorFor(size_t index) const
		{
			return index < m_Items.size() ? ConstIterator(this, index) : ConstIterator();
		}

		inline Iterator IteratorFor(size_t index)
		{
			return index < m_Items.size() ? Iterator(this, index) : Iterator();
		}

		inline void Clone(ElementContainer<ElementType, UserDataType, KeyType>& dest) const
		{
			dest.AllocateDefault(Count());

			for ( const ConstIteratorData& it : *this )
			{
				assert(it.index < dest.Count());

				Item& destItem = dest.m_Items[it.index];

				destItem.element = *it.element;
				destItem.userData = *it.userData;

				if ( it.key )
				{
					dest.AssignMapping(*it.key, it.index);
				}
			}
		}

	private:
		struct Item;
		using KeyToItemIndexMap = std::map<KeyType, size_t>;

		inline void ConstructItems(size_t itemCount, bool clearFirst)
		{
			if ( clearFirst )
			{
				Clear();
			}

			if ( itemCount < 1 )
			{
				return;
			}

			m_Items.resize(m_Items.size() + itemCount);
		}

		template<typename T2>
		inline void ConstructItemsFrom(const T2* source, size_t itemCount, bool clearFirst)
		{
			if ( clearFirst )
			{
				Clear();
			}

			if ( !source || itemCount < 1 )
			{
				return;
			}

			const size_t oldCount = m_Items.size();
			m_Items.resize(oldCount + itemCount);
			const size_t newCount = m_Items.size();

			for ( size_t index = oldCount; index < newCount; ++index )
			{
				const size_t sourceIndex = index - oldCount;
				assert(sourceIndex < itemCount);

				m_Items[index].element = source[sourceIndex];
			}
		}

		std::vector<Item> m_Items;
		KeyToItemIndexMap m_KeyToItemIndex;
	};

	template<typename T, typename U, typename K>
	struct ElementContainer<T, U, K>::Item
	{
	public:
		using ContainerType = ElementContainer<T, U, K>;
		using ElementType = typename ContainerType::ElementType;
		using UserDataType = typename ContainerType::UserDataType;
		using KeyType = typename ContainerType::KeyType;

		size_t index = INVALID_CONTAINER_INDEX;
		KeyType key;
		ElementType element;
		UserDataType userData;

		inline Item() :
			key(),
			element(),
			userData()
		{
		}

		inline Item(Item&& other)
		{
			Move(std::forward<Item>(other));
		}

		inline Item& operator =(Item&& other)
		{
			Move(std::forward<Item>(other));
			return *this;
		}

		Item(const Item& other) = delete;
		Item& operator =(const Item& other) = delete;

	private:
		inline void Move(Item&& other)
		{
			index = other.index;
			other.index = INVALID_CONTAINER_INDEX;

			key = std::move(other.key);
			element = std::move(other.element);
			userData = std::move(other.userData);
		}
	};

	template<typename T, typename U, typename K>
	class ElementContainer<T, U, K>::BaseIterator
	{
	public:
		using ContainerType = ElementContainer<T, U, K>;
		using ElementType = typename ContainerType::ElementType;
		using UserDataType = typename ContainerType::UserDataType;
		using KeyType = typename ContainerType::KeyType;

		inline BaseIterator()
		{
		}

		inline bool IsValid() const
		{
			return m_Container && m_Index < m_Container->Count();
		}

		inline size_t Index() const
		{
			return IsValid() ? m_Index : INVALID_CONTAINER_INDEX;
		}

		inline operator bool() const
		{
			return IsValid();
		}

		inline BaseIterator& operator ++()
		{
			if ( IsValid() )
			{
				++m_Index;
			}

			return *this;
		}

		inline bool operator ==(const BaseIterator& other) const
		{
			if ( IsValid() != other.IsValid() )
			{
				return false;
			}

			if ( !IsValid() && !other.IsValid() )
			{
				return true;
			}

			return m_Index == other.m_Index;
		}

		inline bool operator !=(const BaseIterator& other) const
		{
			return !(*this == other);
		}

	protected:
		inline BaseIterator(const ContainerType* container, size_t index) :
			m_Container(container),
			m_Index(index)
		{
		}

		const ContainerType* m_Container = nullptr;
		size_t m_Index = INVALID_CONTAINER_INDEX;
	};

	template<typename T, typename U, typename K>
	class ElementContainer<T, U, K>::ConstIterator : public ElementContainer<T, U, K>::BaseIterator
	{
	public:
		using ConstIteratorData = typename ContainerType::ConstIteratorData;

		inline ConstIterator() :
			BaseIterator()
		{
		}

		inline const ConstIteratorData& operator *() const
		{
			UpdateOutputData();
			return m_OutData;
		}

		inline ConstIterator& operator ++()
		{
			return static_cast<ConstIterator&>(BaseIterator::operator++());
		}

	protected:
		friend class ContainerType;

		inline ConstIterator(const ContainerType* container, size_t index) :
			BaseIterator(container, index)
		{
		}

		inline void UpdateOutputData() const
		{
			m_OutData = {};

			if ( IsValid() )
			{
				m_OutData.index = m_Index;
				m_OutData.element = m_Container->ElementAt(m_Index);
				m_OutData.userData = m_Container->UserDataAt(m_Index);
				m_OutData.key = &m_Container->KeyFor(m_Index);

				assert(m_OutData.element);
				assert(m_OutData.userData);
				assert(m_OutData.key);
			}
		}

		mutable ConstIteratorData m_OutData;
	};

	template<typename T, typename U, typename K>
	class ElementContainer<T, U, K>::Iterator : public ElementContainer<T, U, K>::BaseIterator
	{
	public:
		using IteratorData = typename ContainerType::IteratorData;

		inline Iterator() :
			BaseIterator()
		{
		}

		inline const IteratorData& operator *() const
		{
			UpdateOutputData();
			return m_OutData;
		}

		inline Iterator& operator ++()
		{
			return static_cast<Iterator&>(BaseIterator::operator++());
		}

	protected:
		friend class ContainerType;

		inline Iterator(ContainerType* container, size_t index) :
			BaseIterator(container, index)
		{
		}

		inline void UpdateOutputData() const
		{
			m_OutData = {};

			if ( IsValid() )
			{
				// This is OK since the original container we were constructed with was mutable.
				ContainerType* mutableContainer = const_cast<ContainerType*>(m_Container);

				m_OutData.index = m_Index;
				m_OutData.element = mutableContainer->ElementAt(m_Index);
				m_OutData.userData = mutableContainer->UserDataAt(m_Index);
				m_OutData.key = &mutableContainer->KeyFor(m_Index);

				assert(m_OutData.element);
				assert(m_OutData.userData);
				assert(m_OutData.key);
			}
		}

		mutable IteratorData m_OutData;
	};
}
