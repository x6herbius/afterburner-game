#include "XashModelFileWriter.h"
#include "elements/Conversions.h"
#include "elements/DumpElements.h"

namespace NFMDL
{
	XashModelFileWriter::XashModelFileWriter(const std::shared_ptr<XashModelFile>& outModelFile) :
		m_OutModelFile(outModelFile)
	{
	}

	void XashModelFileWriter::SetOutputStream(const std::shared_ptr<std::ostream>& stream)
	{
		m_OutStream = stream;
	}

	bool XashModelFileWriter::Verbose() const
	{
		return m_Verbose;
	}

	void XashModelFileWriter::SetVerbose(bool verbose)
	{
		m_Verbose = verbose;
	}

	void XashModelFileWriter::Write()
	{
		if ( !m_OutModelFile )
		{
			throw std::runtime_error("Output ModelFile container was not set!");
		}

		if ( !m_OutStream )
		{
			throw std::runtime_error("Output stream was not set!");
		}

		if ( !m_OutStream->good() )
		{
			throw std::runtime_error("Output stream was not valid!");
		}

		WriteEntireFile();
	}

	void XashModelFileWriter::WriteEntireFile()
	{
		// TODO: Innards of these are quite rough and could be refactored.
		CalculateBlockOffsets();
		WriteHeader();
	}

	void XashModelFileWriter::CalculateBlockOffsets()
	{
		memset(&m_AdditionalOffsets, 0, sizeof(m_AdditionalOffsets));
		m_TotalFileSize = 0;

		HeaderV10Xash& header = m_OutModelFile->Header;
		uint32_t offset = sizeof(header);

		offset = SetOffsetForBlock(header.bones, m_OutModelFile->Bones, offset);
		offset = SetOffsetForBlock(header.boneControllers, m_OutModelFile->BoneControllers, offset);
		offset = SetOffsetForBlock(header.hitBoxes, m_OutModelFile->HitBoxes, offset);
		offset = SetOffsetForBlock(header.sequences, m_OutModelFile->Sequences, offset);
		offset = SetOffsetForBlock(header.sequenceGroups, m_OutModelFile->SequenceGroups, offset);
		offset = SetOffsetForBlock(header.textures, m_OutModelFile->Textures, offset);

		header.textureDataOffset = 0; // TODO: Cater for texture data

		header.skinOffset = offset;
		offset += m_OutModelFile->Skins.Count() * sizeof(Skin);

		if ( m_Verbose )
		{
			LogComputedOffset<Skin>(header.skinOffset, m_OutModelFile->Skins.Count());
		}

		offset = SetOffsetForBlock(header.bodyGroups, m_OutModelFile->BodyGroups, offset);
		offset = SetOffsetForBlock(header.attachments, m_OutModelFile->Attachments, offset);

		header.studiohdr2Offset = 0;
		header.soundsOffset = 0; // TODO: cater for sounds
		header.soundGroups = CountOffsetPair(); // TODO: cater for sound groups
		header.transitions = CountOffsetPair(); // TODO: cater for transitions

		// Other blocks not referenced directly in header:
		m_AdditionalOffsets.eventsOffset = m_OutModelFile->Events.Count() > 0 ? offset : 0;
		offset += m_OutModelFile->Events.Count() * sizeof(Event);

		if ( m_Verbose )
		{
			LogComputedOffset<Event>(m_AdditionalOffsets.eventsOffset, m_OutModelFile->Events.Count());
		}

		m_AdditionalOffsets.footPivotsOffset = m_OutModelFile->FootPivots.Count() > 0 ? offset : 0;
		offset += m_OutModelFile->FootPivots.Count() * sizeof(FootPivot);

		if ( m_Verbose )
		{
			LogComputedOffset<FootPivot>(m_AdditionalOffsets.footPivotsOffset, m_OutModelFile->FootPivots.Count());
		}

		m_AdditionalOffsets.modelsOffset = m_OutModelFile->Models.Count() > 0 ? offset : 0;
		offset += m_OutModelFile->Models.Count() * sizeof(ModelV10Xash);

		if ( m_Verbose )
		{
			LogComputedOffset<ModelV10Xash>(m_AdditionalOffsets.modelsOffset, m_OutModelFile->Models.Count());
		}

		m_AdditionalOffsets.meshesOffset = m_OutModelFile->Meshes.Count() > 0 ? offset : 0;
		offset += m_OutModelFile->Meshes.Count() * sizeof(MeshV10Xash);

		if ( m_Verbose )
		{
			LogComputedOffset<MeshV10Xash>(m_AdditionalOffsets.meshesOffset, m_OutModelFile->Meshes.Count());
		}

		m_AdditionalOffsets.animationDataOffset = offset;
		for ( auto& it : m_OutModelFile->AnimationData )
		{
			offset += it.element->size() * sizeof(AnimationDataValueList::ValueType);
		}

		if ( offset == m_AdditionalOffsets.animationDataOffset )
		{
			// No values.
			m_AdditionalOffsets.animationDataOffset = 0;
		}

		if ( m_Verbose )
		{
			LogComputedOffset<AnimationValue>(m_AdditionalOffsets.animationDataOffset,
											  (offset - m_AdditionalOffsets.animationDataOffset) / sizeof(AnimationDataValueList::ValueType));
		}

		m_TotalFileSize = offset;
	}

	void XashModelFileWriter::WriteHeader()
	{
		HeaderV10Xash& header = m_OutModelFile->Header;
		m_FileBeginOffset = m_OutStream->tellp();

		if ( m_Verbose )
		{
			std::cout << "Writing header: " << m_OutModelFile->Header << std::endl;
		}

		m_OutStream->write(reinterpret_cast<const char*>(&header), sizeof(header));

		WriteAllElements(header.bones.offset, m_OutModelFile->Bones);
		WriteAllElements(header.boneControllers.offset, m_OutModelFile->BoneControllers);
		WriteAllElements(header.hitBoxes.offset, m_OutModelFile->HitBoxes);
		WriteAllElements(header.sequences.offset, m_OutModelFile->Sequences);
		WriteAllElements(header.sequenceGroups.offset, m_OutModelFile->SequenceGroups);
		WriteAllElements(header.textures.offset, m_OutModelFile->Textures);
		WriteAllElements(header.skinOffset, m_OutModelFile->Skins);
		WriteAllElements(header.bodyGroups.offset, m_OutModelFile->BodyGroups);
		WriteAllElements(header.attachments.offset, m_OutModelFile->Attachments);
		WriteAllElements(m_AdditionalOffsets.eventsOffset, m_OutModelFile->Events);
		WriteAllElements(m_AdditionalOffsets.footPivotsOffset, m_OutModelFile->FootPivots);
		WriteAllElements(m_AdditionalOffsets.modelsOffset, m_OutModelFile->Models);
		WriteAllElements(m_AdditionalOffsets.meshesOffset, m_OutModelFile->Meshes);

		assert(static_cast<uint32_t>(m_OutStream->tellp()) == m_AdditionalOffsets.animationDataOffset);

		if ( m_Verbose )
		{
			std::cout
				<< "Beginning write of "
				<< m_OutModelFile->AnimationData.Count()
				<< " sets of animation data to file from offset "
				<< m_OutStream->tellp()
				<< std::endl;
		}

		for ( auto& it : m_OutModelFile->AnimationData )
		{
			if ( m_Verbose )
			{
				std::cout
					<< "Writing "
					<< it.element->size()
					<< " "
					<< ElementTraits<AnimationValue>::ELEMENT_NAME
					<< " elements to file at offset "
					<< m_OutStream->tellp()
					<< std::endl;
			}

			m_OutStream->write(reinterpret_cast<const char*>(it.element->data()), it.element->size());
		}
	}
}
