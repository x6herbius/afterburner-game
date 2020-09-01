#pragma once

#include <exception>
#include <memory>
#include <string>
#include <vector>
#include <sstream>
#include <type_traits>
#include "NightfireModelFile.h"
#include "elements/ElementTraits.h"

namespace NFMDL
{
	class NightfireModelFileReader
	{
	public:
		NightfireModelFileReader(const std::shared_ptr<NightfireModelFile>& modelFile);

		void ReadFromFile(const std::string& filePath);

		bool ReadHeaderOnly() const;
		void SetReadHeaderOnly(bool headerOnly);

	private:
		using FileBuffer = std::vector<char>;

		static uint32_t AlignTo16Bytes(uint32_t offset);

		// Function used for types that are not arithmetic (ie. structs/classes).
		template<typename T>
		inline typename std::enable_if<!std::is_arithmetic<T>::value, const T*>::type
		GetElement(uint32_t offset = 0, uint32_t count = 0) const
		{
			return GetElementInternal<T>(offset, count, std::string("'") + ElementTraits<T>::ELEMENT_NAME + std::string("'"));
		}

		// Function used for types that are arithmetic (ie. integers).
		template<typename T>
		inline typename std::enable_if<std::is_arithmetic<T>::value, const T*>::type
		GetElement(uint32_t offset = 0, uint32_t count = 0) const
		{
			return GetElementInternal<T>(offset, count, "arithmetic");
		}

		template<typename T>
		inline const T* GetElementInternal(uint32_t offset, uint32_t count, const std::string& typeName) const
		{
			const size_t bytesRequired = (count == 0 ? 1 : count) * sizeof(T);
			const size_t bytesAvailable = m_InputFileData->size() - offset;

			if ( bytesRequired > bytesAvailable )
			{
				std::stringstream stream;

				stream
					<< "Tried to get "
					<< bytesRequired
					<< " bytes of data when only "
					<< bytesAvailable
					<< " bytes were available. ("
					<< m_InputFileData->size()
					<< " total input bytes; attempt was made to get "
					<< (count == 0 ? 1 : count)
					<< " "
					<< typeName
					<< " elements at "
					<< sizeof(T)
					<< " bytes each from input offset "
					<< offset
					<< ".";

				throw std::runtime_error(stream.str());
			}

			return reinterpret_cast<const T*>(m_InputFileData->data() + offset);
		}

		template<typename T>
		inline void ReadElementArray(const CountOffsetPair& cop, ElementArray<T>& array)
		{
			array.AllocateAndZero(cop.count);

			if ( cop.count > 0 )
			{
				array.CopyDataFrom(GetElement<T>(cop.offset, cop.count), cop.count);
			}
		}

		template<typename T>
		inline void ReadElementArray(uint32_t offset, uint32_t count, ElementArray<T>& array)
		{
			ReadElementArray(CountOffsetPair{count, offset}, array);
		}

		void ReadEntireFile();
		void ReadHeader();
		void ReadModels();
		void ReadLevelsOfDetail();
		void ReadSkins();
		void ReadModelInfos();
		void ReadMeshes();
		void ReadSounds();
		void ReadSequenceAnimationData();

		void ReadRLEAnimationData(NightfireModelFile::AnimationDataValueList& valueList,
								  uint32_t dataOffset,
								  uint32_t frameCount);

		std::shared_ptr<NightfireModelFile> m_ModelFile;
		std::unique_ptr<FileBuffer> m_InputFileData;
		bool m_ReadHeaderOnly = false;
	};
}
