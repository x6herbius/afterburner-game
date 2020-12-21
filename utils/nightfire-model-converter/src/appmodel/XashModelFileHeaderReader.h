#pragma once

#include <exception>
#include <memory>
#include <string>
#include <vector>
#include "elements/HeaderV10Xash.h"
#include "io/ReadHelpers.h"

namespace NFMDL
{
	class XashModelFileHeaderReader
	{
	public:
		explicit XashModelFileHeaderReader(HeaderV10Xash& header);

		void ReadFromFile(const std::string& filePath);

	private:
		void ReadHeader();

		HeaderV10Xash& m_Header;
		std::unique_ptr<StreamBuffer> m_InputFileData;
	};
}
