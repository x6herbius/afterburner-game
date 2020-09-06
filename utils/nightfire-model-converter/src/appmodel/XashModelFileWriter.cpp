#include "XashModelFileWriter.h"
#include "elements/Conversions.h"

namespace NFMDL
{
	template<typename T>
	static inline void CopyElementArray(const ElementArray<T>& src, ElementArray<T>& dest)
	{
		dest.AllocateAndZero(src.Count());

		src.ForEach([](uint32_t index, const T& element)
		{
			dest[index] = element;
		});
	}

	template<typename TS, typename TD>
	static inline void CopyElementArray(const ElementArray<TS>& src, ElementArray<TD>& dest)
	{
		dest.AllocateAndZero(src.Count());

		src.ForEach([](uint32_t index, const TS& element)
		{
			Convert(element, dest[index]);
		});
	}

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
		Convert(m_InModelFile->Header, m_OutModelFile->Header);

		CopyElementArray(m_InModelFile->Bones, m_OutModelFile->Bones);
		CopyElementArray(m_InModelFile->BoneControllers, m_OutModelFile->BoneControllers);
		CopyElementArray(m_InModelFile->HitBoxes, m_OutModelFile->HitBoxes);
		CopyElementArray(m_InModelFile->Sequences, m_OutModelFile->Sequences);
		CopyElementArray(m_InModelFile->SequenceGroups, m_OutModelFile->SequenceGroups);
		CopyElementArray(m_InModelFile->Attachments, m_OutModelFile->Attachments);

		m_OutModelFile->Skins = m_InModelFile->Skins;
		m_OutModelFile->Events = m_InModelFile->Events;
		m_OutModelFile->FootPivots = m_InModelFile->FootPivots;
		m_OutModelFile->AnimationData = m_InModelFile->AnimationData;

		// TODO: The rest
	}

	void XashModelFileWriter::WriteEntireFile()
	{
		// TODO
	}
}
