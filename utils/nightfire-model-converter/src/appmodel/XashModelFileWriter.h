#pragma once

#include <memory>
#include <ostream>
#include <sstream>
#include <type_traits>
#include <functional>
#include <cassert>
#include <iostream>
#include "NightfireModelFile.h"
#include "XashModelFile.h"
#include "containers/ElementContainer.h"

namespace NFMDL
{
	class XashModelFileWriter
	{
	public:
		explicit XashModelFileWriter(const std::shared_ptr<XashModelFile>& outModelFile);

		void SetOutputStream(const std::shared_ptr<std::ostream>& stream);
		bool Verbose() const;
		void SetVerbose(bool verbose);

		void Write();

	private:
		struct AdditionalOffsets
		{
			uint32_t eventsOffset;
			uint32_t footPivotsOffset;
			uint32_t modelsOffset;
			uint32_t meshesOffset;
			uint32_t animationDataOffset;
		};

		void WriteEntireFile();
		void CalculateBlockOffsets();
		void WriteHeader();

		template<typename T, typename U, typename K>
		inline uint32_t SetOffsetForBlock(CountOffsetPair& cop,
										  const ElementContainer<T, U, K>& container,
										  uint32_t currentOffset)
		{
			static_assert(ElementContainer<T, U, K>::ELEMENT_IS_POD, "Only permitted for POD elements.");

			cop.count = static_cast<uint32_t>(container.Count());
			cop.offset = cop.count > 0 ? currentOffset : 0;

			if ( m_Verbose )
			{
				LogComputedOffset<T>(cop);
			}

			return currentOffset + (cop.count * sizeof(T));
		}

		template<typename T, typename U, typename K>
		inline void WriteAllElements(uint32_t targetOffset, const ElementContainer<T, U, K>& container)
		{
			if ( targetOffset < 1 )
			{
				return;
			}

			assert(m_OutStream && static_cast<uint32_t>(m_OutStream->tellp()) == targetOffset);

			if ( m_Verbose )
			{
				std::cout
					<< "Writing "
					<< container.Count()
					<< " "
					<< ElementTraits<T>::ELEMENT_NAME
					<< " elements to file at offset "
					<< targetOffset
					<< std::endl;
			}

			for ( auto it : container )
			{
				m_OutStream->write(reinterpret_cast<const char*>(it.element), sizeof(*(it.element)));
			}
		}

		template<typename T>
		inline void LogComputedOffset(const CountOffsetPair& cop)
		{
			std::cout
				<< "Computed output file offset of "
				<< cop.offset
				<< " for "
				<< cop.count
				<< " "
				<< ElementTraits<T>::ELEMENT_NAME
				<< " elements @ "
				<< sizeof(T)
				<< " bytes each"
				<< std::endl;
		}

		template<typename T>
		inline void LogComputedOffset(uint32_t offset, size_t count)
		{
			LogComputedOffset<T>(CountOffsetPair{static_cast<uint32_t>(count), offset});
		}

		std::shared_ptr<XashModelFile> m_OutModelFile;
		std::shared_ptr<std::ostream> m_OutStream;
		bool m_Verbose = false;

		AdditionalOffsets m_AdditionalOffsets;
		size_t m_TotalFileSize = 0;
		uint32_t m_FileBeginOffset = 0;
	};
}
