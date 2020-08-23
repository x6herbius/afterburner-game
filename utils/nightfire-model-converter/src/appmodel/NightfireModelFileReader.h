#pragma once

#include <exception>
#include <memory>
#include <string>
#include <vector>
#include "NightfireModelFile.h"
#include "elements/ElementTraits.h"

namespace NFMDL
{
	class NightfireModelFileReader
	{
	public:
		NightfireModelFileReader(const std::shared_ptr<NightfireModelFile>& modelFile);

		void ReadFromFile(const std::string& filePath);

	private:
		using FileBuffer = std::vector<char>;

		template<typename T>
		inline const T* GetElement(uint32_t offset = 0, uint32_t count = 0) const
		{
			if ( count == 0 )
			{
				if ( sizeof(T) > m_InputFileData->size() - offset )
				{
					throw std::runtime_error("Not enough input file data (" +
											 std::to_string(m_InputFileData->size()) +
											 " bytes) to get " +
											 std::string(ElementTraits<T>::ELEMENT_NAME) +
											 " element of " +
											 std::to_string(sizeof(T)) +
											 " bytes at offset " +
											 std::to_string(offset) +
											 ".");
				}
			}
			else
			{
				if ( count * sizeof(T) > m_InputFileData->size() - offset )
				{
					throw std::runtime_error("Not enough input file data (" +
											 std::to_string(m_InputFileData->size()) +
											 " bytes) to get " +
											 std::to_string(count) +
											 " " +
											 std::string(ElementTraits<T>::ELEMENT_NAME) +
											 " elements totalling " +
											 std::to_string(count * sizeof(T)) +
											 " bytes at offset " +
											 std::to_string(offset) +
											 ".");
				}
			}

			return reinterpret_cast<const T*>(m_InputFileData->data() + offset);
		};

		template<typename T>
		inline void ReadElementArray(const CountOffsetPair& cop, ElementArray<T>& array)
		{
			array.AllocateAndZero(cop.count);

			if ( cop.count > 0 )
			{
				array.CopyDataFrom(GetElement<T>(cop.offset, cop.count), cop.count);
			}
		}

		void ReadEntireFile();
		void ReadHeader();

		std::shared_ptr<NightfireModelFile> m_ModelFile;
		std::unique_ptr<FileBuffer> m_InputFileData;
	};
}
