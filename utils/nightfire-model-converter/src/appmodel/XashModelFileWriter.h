#pragma once

#include <memory>
#include <fstream>
#include <sstream>
#include <type_traits>
#include "NightfireModelFile.h"
#include "XashModelFile.h"

namespace NFMDL
{
	class XashModelFileWriter
	{
	public:
		explicit XashModelFileWriter(const std::shared_ptr<XashModelFile>& outModelFile);

		void SetSourceModelFile(const std::shared_ptr<const NightfireModelFile>& inModelFile);
		void WriteToFile(const std::string& filePath);

	private:
		// Function used for types that are not arithmetic (ie. structs/classes).
		template<typename T>
		inline typename std::enable_if<!std::is_arithmetic<T>::value, void>::type
		WriteElement(const T* element, size_t count = 0) const
		{
			WriteElementInternal<T>(element, count, std::string("'") + ElementTraits<T>::ELEMENT_NAME + std::string("'"));
		}

		// Function used for types that are arithmetic (ie. integers).
		template<typename T>
		inline typename std::enable_if<std::is_arithmetic<T>::value, void>::type
		WriteElement(const T* element, size_t count = 0) const
		{
			WriteElementInternal<T>(element, count, "arithmetic");
		}

		template<typename T>
		inline void WriteElementInternal(const T* element, size_t count, const std::string& typeName)
		{
			if ( !element )
			{
				throw std::runtime_error("Could not write null " + typeName + " element to output file.");
			}

			if ( count < 1 )
			{
				count = 1;
			}

			const size_t totalBytes = count * sizeof(element);
			m_OutStream->write(reinterpret_cast<const char*>(element), totalBytes);

			if ( !m_OutStream->good() )
			{
				std::stringstream stream;

				stream
					<< "Failed to write "
					<< totalBytes
					<< " bytes to output file at position "
					<< m_OutStream->tellp()
					<< ". (Attempt was made to write "
					<< count
					<< " "
					<< typeName
					<< " elements at "
					<< sizeof(T)
					<< " bytes each.)";

				throw std::runtime_error(stream.str());
			}
		}

		template<typename T>
		void WriteElementArray(const ElementArray<T>& array)
		{
			WriteElement(array.ConstData(), array.Count());
		}

		void ConvertSourceFile();
		void WriteEntireFile();

		std::shared_ptr<const NightfireModelFile> m_InModelFile;
		std::shared_ptr<XashModelFile> m_OutModelFile;
		std::unique_ptr<std::ofstream> m_OutStream;
	};
}
