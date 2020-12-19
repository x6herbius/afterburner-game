#pragma once

#include <cassert>
#include <type_traits>
#include <vector>
#include <map>
#include <memory>

namespace NFMDL
{
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

		static constexpr bool USES_KEYS = sizeof(KeyType) > 0;
		static constexpr size_t INVALID_INDEX = ~0;

		class BaseIterator;
		class ConstIterator;
		class Iterator;

		struct ConstIteratorData
		{
			size_t index = INVALID_INDEX;
			const ElementType* element = nullptr;
			const UserDataType* userData = nullptr;
			const KeyType* key = nullptr;
		};

		struct IteratorData
		{
			size_t index = INVALID_INDEX;
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

		inline void AppendDefault(size_t itemCount)
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
			if ( !key || index < m_Items.size() )
			{
				return;
			}

			RemoveMapping(key);

			m_KeyToItemIndex[key] = index;
			m_Items[index].SetKey(key);
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

				m_Items[it->second].SetKey(KeyType());
				m_KeyToItemIndex.erase(key);
			}
		}

		inline const ElementType* ElementAt(size_t index) const
		{
			return index < m_Items.size() ? m_Items[index].Element() : nullptr;
		}

		inline ElementType* ElementAt(size_t index)
		{
			return index < m_Items.size() ? m_Items[index].Element() : nullptr;
		}

		inline const UserDataType* UserDataAt(size_t index) const
		{
			return index < m_Items.size() ? m_Items[index].UserData() : nullptr;
		}

		inline UserDataType* UserDataAt(size_t index)
		{
			return index < m_Items.size() ? m_Items[index].UserData() : nullptr;
		}

		inline const KeyType& KeyFor(size_t index) const
		{
			static const KeyType invalidKey;

			return index < m_Items.size() ? m_Items[index].Key() : invalidKey;
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

		inline void Clone(ElementContainer<ElementType, UserDataType, KeyType>& dest) const
		{
			dest.AllocateDefault(Count());

			for ( const ConstIteratorData& it : *this )
			{
				ElementType* element = dest.ElementAt(it.index);
				UserDataType* userData = dest.UserDataAt(it.index);

				assert(userData && it.userData);
				assert(element && it.element);
				assert(it.key);

				*element = *it.element;
				*userData = *it.userData;

				dest.AssignMapping(*it.key, it.index);
			}
		}

	private:
		class Item;
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

			const size_t oldCount = m_Items.size();
			m_Items.resize(oldCount + itemCount);
			const size_t newCount = m_Items.size();

			for ( size_t index = oldCount; index < newCount; ++index )
			{
				m_Items[index].InitialiseToDefaults(index);
			}
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

				m_Items[index].InitialiseFromElement(index, source[sourceIndex]);
			}
		}

		std::vector<Item> m_Items;
		KeyToItemIndexMap m_KeyToItemIndex;
	};

	template<typename T, typename U, typename K>
	class ElementContainer<T, U, K>::Item
	{
	public:
		using ContainerType = ElementContainer<T, U, K>;
		using ElementType = typename ContainerType::ElementType;
		using UserDataType = typename ContainerType::UserDataType;
		using KeyType = typename ContainerType::KeyType;

		inline Item()
		{
			static_assert(std::is_pod<ElementType>::value, "Element type must be POD.");
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

		inline bool IsValid() const
		{
			return m_Element.get() != nullptr;
		}

		inline const KeyType& Key() const
		{
			static const KeyType invalidKey;

			return IsValid() ? m_Key : invalidKey;
		}

		inline void SetKey(const KeyType& key)
		{
			m_Key = key;
		}

		inline ElementType* Element() const
		{
			return m_Element.get();
		}

		inline UserDataType* UserData() const
		{
			return m_UserData.get();
		}

		inline size_t Index() const
		{
			return m_Index;
		}

		inline void InitialiseToDefaults(size_t index)
		{
			m_Index = index;
			m_Element = std::make_unique<ElementType>();
			m_UserData = std::make_unique<UserDataType>();
		}

		template<typename T2>
		inline void InitialiseFromElement(size_t index, const T2& element)
		{
			m_Index = index;
			m_Element = std::make_unique<ElementType>(element);
			m_UserData = std::make_unique<UserDataType>();
		}

	private:
		inline void Move(Item&& other)
		{
			m_Index = other.m_Index;
			other.m_Index = ContainerType::INVALID_INDEX;

			m_Key = std::move(other.m_Key);
			m_Element = std::move(other.m_Element);
			m_UserData = std::move(other.m_UserData);
		}

		size_t m_Index = ContainerType::INVALID_INDEX;
		KeyType m_Key;
		std::unique_ptr<ElementType> m_Element;
		std::unique_ptr<UserDataType> m_UserData;
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
			return IsValid() ? m_Index : ContainerType::INVALID_INDEX;
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
		size_t m_Index = ContainerType::INVALID_INDEX;
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
			}
		}

		mutable IteratorData m_OutData;
	};
}
