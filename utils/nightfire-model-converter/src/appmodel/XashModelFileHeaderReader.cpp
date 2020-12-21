#include <fstream>
#include "appmodel/XashModelFileHeaderReader.h"

namespace NFMDL
{
	XashModelFileHeaderReader::XashModelFileHeaderReader(HeaderV10Xash& header) :
		m_Header(header)
	{
	}

	void XashModelFileHeaderReader::ReadFromFile(const std::string& filePath)
	{
		std::ifstream input(filePath, std::ios::binary);

		if ( !input.good() )
		{
			throw std::runtime_error("Input file was not valid.");
		}

		m_InputFileData = CreateStreamBuffer(input);

		try
		{
			ReadHeader();
			m_InputFileData.reset();
		}
		catch ( const std::runtime_error& ex )
		{
			m_InputFileData.reset();
			throw ex;
		}

		catch ( const std::exception& ex )
		{
			m_InputFileData.reset();
			throw ex;
		}
	}

	void XashModelFileHeaderReader::ReadHeader()
	{
		m_Header = *GetElementsFromStreamBuffer<HeaderV10Xash>(*m_InputFileData);

		if ( m_Header.ident != HeaderIdentifier(FormatTraits<HeaderV10Xash>::TARGET_IDENTIFIER) )
		{
			throw std::runtime_error("Input file header identifier did not match expected value.");
		}

		if ( m_Header.version != FormatTraits<HeaderV10Xash>::TARGET_VERSION )
		{
			throw std::runtime_error("Input file header version " +
									 std::to_string(m_Header.version) +
									 " did not match expected version " +
									 std::to_string(FormatTraits<HeaderV10Xash>::TARGET_VERSION) +
									 ".");
		}
	}
}
