#include "XashModelFileWriter.h"

namespace NFMDL
{
	XashModelFileWriter::XashModelFileWriter(const std::shared_ptr<XashModelFile>& outModelFile) :
		m_OutModelFile(outModelFile)
	{
	}

	void XashModelFileWriter::SetSourceModelFile(const std::shared_ptr<const NightfireModelFile>& inModelFile)
	{
		m_InModelFile = inModelFile;
	}

	void XashModelFileWriter::WriteToFile(const std::string& filePath)
	{
		if ( !m_OutModelFile )
		{
			throw std::runtime_error("Output ModelFile container was not set!");
		}

		if ( !m_InModelFile )
		{
			throw std::runtime_error("Source ModelFile container was not set!");
		}

		m_OutStream = std::make_unique<std::ofstream>(filePath, std::ios::binary);

		if ( !m_OutStream || !m_OutStream->good() )
		{
			if ( m_OutStream )
			{
				m_OutStream.reset();
			}

			throw std::runtime_error("Output file was not valid.");
		}

		try
		{
			ConvertSourceFile();
			WriteEntireFile();
			m_OutStream.reset();
		}
		catch ( const std::runtime_error& ex )
		{
			m_OutStream.reset();
			throw ex;
		}

		catch ( const std::exception& ex )
		{
			m_OutStream.reset();
			throw ex;
		}
	}

	void XashModelFileWriter::ConvertSourceFile()
	{
	}

	void XashModelFileWriter::WriteEntireFile()
	{
	}
}
