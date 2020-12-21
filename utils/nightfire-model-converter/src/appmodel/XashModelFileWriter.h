#pragma once

#include <memory>
#include <ostream>
#include <sstream>
#include <type_traits>
#include <functional>
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

			cop.count = container.Count();

			if ( cop.count > 0 )
			{
				cop.offset = currentOffset;
				return currentOffset + (cop.count * sizeof(T));
			}
			else
			{
				cop.offset = 0;
				return currentOffset;
			}
		}

		std::shared_ptr<XashModelFile> m_OutModelFile;
		std::shared_ptr<std::ostream> m_OutStream;

		AdditionalOffsets m_AdditionalOffsets;
		size_t m_TotalFileSize = 0;
		uint32_t m_FileBeginOffset = 0;
	};
}
