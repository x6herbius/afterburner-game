#pragma once

#include <vector>
#include <memory>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <cstdint>
#include <string>
#include <type_traits>
#include "elements/ElementTraits.h"

namespace NFMDL
{
	using StreamBuffer = std::vector<char>;

	static inline std::unique_ptr<StreamBuffer> CreateStreamBuffer(std::ifstream& input)
	{
		return std::make_unique<StreamBuffer>(std::istreambuf_iterator<char>(input), std::istreambuf_iterator<char>());
	}

	template<typename T>
	static inline const T* GetElementsFromStreamBuffer(const StreamBuffer& buffer, uint32_t offset, uint32_t count, const std::string& typeName)
	{
		if ( count < 1 )
		{
			count = 1;
		}

		const size_t bytesRequired = count * sizeof(T);
		const size_t bytesAvailable = buffer.size() - offset;

		if ( bytesRequired > bytesAvailable )
		{
			std::stringstream stream;

			stream
				<< "Tried to get "
				<< bytesRequired
				<< " bytes of data when only "
				<< bytesAvailable
				<< " bytes were available. ("
				<< buffer.size()
				<< " total input bytes; attempt was made to get "
				<< count
				<< " "
				<< typeName
				<< " elements at "
				<< sizeof(T)
				<< " bytes each from input offset "
				<< offset
				<< ".)";

			throw std::runtime_error(stream.str());
		}

		return reinterpret_cast<const T*>(buffer.data() + offset);
	}

	// Function used for types that are not arithmetic (ie. structs/classes).
	template<typename T>
	static inline typename std::enable_if<!std::is_arithmetic<T>::value, const T*>::type
	GetElementsFromStreamBuffer(const StreamBuffer& buffer, uint32_t offset = 0, uint32_t count = 0)
	{
		return GetElementsFromStreamBuffer<T>(buffer, offset, count, std::string("'") + ElementTraits<T>::ELEMENT_NAME + std::string("'"));
	}

	// Function used for types that are arithmetic (ie. integers).
	template<typename T>
	static inline typename std::enable_if<std::is_arithmetic<T>::value, const T*>::type
	GetElementsFromStreamBuffer(const StreamBuffer& buffer, uint32_t offset = 0, uint32_t count = 0)
	{
		return GetElementsFromStreamBuffer<T>(buffer, offset, count, "arithmetic");
	}
}
