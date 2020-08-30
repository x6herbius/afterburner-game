#pragma once

#include <exception>
#include <memory>
#include <string>
#include <vector>
#include <sstream>
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

		template<typename T>
		inline const T* GetElement(uint32_t offset = 0, uint32_t count = 0) const
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
					<< " '"
					<< ElementTraits<T>::ELEMENT_NAME
					<< "' elements at "
					<< sizeof(T)
					<< " bytes each from input offset "
					<< offset
					<< ".";

				throw std::runtime_error(stream.str());
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

		static uint32_t AlignTo16Bytes(uint32_t offset);

		std::shared_ptr<NightfireModelFile> m_ModelFile;
		std::unique_ptr<FileBuffer> m_InputFileData;
		bool m_ReadHeaderOnly = false;
	};
}
