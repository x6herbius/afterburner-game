#pragma once

#include <exception>
#include <memory>
#include <string>
#include <vector>
#include <sstream>
#include <type_traits>
#include "NightfireModelFile.h"
#include "elements/ElementTraits.h"
#include "io/ReadHelpers.h"

namespace NFMDL
{
	class NightfireModelFileReader
	{
	public:
		explicit NightfireModelFileReader(const std::shared_ptr<NightfireModelFile>& modelFile);

		void ReadFromFile(const std::string& filePath);

		bool ReadHeaderOnly() const;
		void SetReadHeaderOnly(bool headerOnly);

		bool Verbose() const;
		void SetVerbose(bool verbose);

	private:
		static uint32_t AlignTo16Bytes(uint32_t offset);

		// Function used for types that are not arithmetic (ie. structs/classes).
		template<typename T>
		inline const T* GetElement(uint32_t offset = 0, uint32_t count = 0) const
		{
			return GetElementsFromStreamBuffer<T>(*m_InputFileData, offset, count);
		}

		template<typename T, typename U, typename K>
		inline void ReadElements(const CountOffsetPair& cop, ElementContainer<T, U, K>& container)
		{
			if ( m_Verbose )
			{
				LogElementsToRead(cop, ElementTraits<T>::ELEMENT_NAME, sizeof(T));
			}

			if ( cop.count > 0 )
			{
				container.AllocateFrom(GetElement<T>(cop.offset, cop.count), cop.count);
			}
			else
			{
				container.Clear();
			}
		}

		template<typename T, typename U, typename K>
		inline void ReadElements(uint32_t offset, uint32_t count, ElementContainer<T, U, K>& container)
		{
			ReadElements(CountOffsetPair{count, offset}, container);
		}

		void ReadEntireFile();
		void ReadHeader();
		void ReadModelsAndChildElements();
		void ReadLevelsOfDetail();
		void ReadSkins();
		void ReadModelInfos(size_t bodyGroupGlobalIndex, size_t modelGlobalIndex);
		void ReadMeshes(size_t bodyGroupGlobalIndex, size_t modelGlobalIndex, size_t modelInfoGlobalIndex);
		void ReadSounds();
		void ReadSequenceAnimationData();
		void ReadEvents();
		void ReadFootPivots();

		void ReadRLEAnimationData(AnimationDataValueList& valueList,
								  uint32_t dataOffset,
								  uint32_t frameCount);

		TOwnedItemKey<ModelV14> BodyGroupReferencingModelAtOffset(uint32_t modelOffset) const;
		TOwnedItemKey<ModelV14> BodyGroupReferencingModel(size_t index) const;
		size_t ModelIndexForOffset(uint32_t offset) const;

		void LogElementsToRead(const CountOffsetPair& cop, const std::string& elementName, size_t elementSize);

		std::shared_ptr<NightfireModelFile> m_ModelFile;
		std::unique_ptr<StreamBuffer> m_InputFileData;
		bool m_ReadHeaderOnly = false;
		bool m_Verbose = false;
	};
}
