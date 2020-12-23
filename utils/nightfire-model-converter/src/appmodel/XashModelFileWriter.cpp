#include "XashModelFileWriter.h"
#include "elements/Conversions.h"

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

		offset = SetOffsetForBlock(header.bodyGroups, m_OutModelFile->BodyGroups, offset);
		offset = SetOffsetForBlock(header.attachments, m_OutModelFile->Attachments, offset);

		header.studiohdr2Offset = 0;
		header.soundsOffset = 0; // TODO: cater for sounds
		header.soundGroups = CountOffsetPair(); // TODO: cater for sound groups
		header.transitions = CountOffsetPair(); // TODO: cater for transitions

		// Other blocks not referenced directly in header:
		m_AdditionalOffsets.eventsOffset = offset;
		offset += m_OutModelFile->Events.Count() * sizeof(Event);

		m_AdditionalOffsets.footPivotsOffset = offset;
		offset += m_OutModelFile->FootPivots.Count() * sizeof(FootPivot);

		m_AdditionalOffsets.modelsOffset = offset;
		offset += m_OutModelFile->Models.Count() * sizeof(ModelV10Xash);

		m_AdditionalOffsets.meshesOffset = offset;
		offset += m_OutModelFile->Meshes.Count() * sizeof(MeshV10Xash);

		m_AdditionalOffsets.animationDataOffset = offset;
		for ( auto& it : m_OutModelFile->AnimationData )
		{
			offset += it.element->size() * sizeof(AnimationDataValueList::ValueType);
		}

		m_TotalFileSize = offset;
	}

	void XashModelFileWriter::WriteHeader()
	{
		HeaderV10Xash& header = m_OutModelFile->Header;
		m_FileBeginOffset = m_OutStream->tellp();

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

		for ( auto& it : m_OutModelFile->AnimationData )
		{
			m_OutStream->write(reinterpret_cast<const char*>(it.element->data()), it.element->size());
		}
	}
}
