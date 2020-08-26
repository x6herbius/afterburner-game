#include <fstream>
#include <iterator>

#include "NightfireModelFileReader.h"
#include "elements/HeaderV14.h"
#include "Utils.h"

namespace NFMDL
{
	NightfireModelFileReader::NightfireModelFileReader(const std::shared_ptr<NightfireModelFile>& modelFile) :
		m_ModelFile(modelFile)
	{
	}

	bool NightfireModelFileReader::ReadHeaderOnly() const
	{
		return m_ReadHeaderOnly;
	}

	void NightfireModelFileReader::SetReadHeaderOnly(bool headerOnly)
	{
		m_ReadHeaderOnly = headerOnly;
	}

	void NightfireModelFileReader::ReadFromFile(const std::string& filePath)
	{
		if ( !m_ModelFile )
		{
			throw std::runtime_error("ModelFile container was not set!");
		}

		std::ifstream input(filePath, std::ios::binary);

		if ( !input.good() )
		{
			throw std::runtime_error("Input file was not valid.");
		}

		m_InputFileData = std::make_unique<FileBuffer>(std::istreambuf_iterator<char>(input), std::istreambuf_iterator<char>());

		try
		{
			ReadEntireFile();
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

	void NightfireModelFileReader::ReadEntireFile()
	{
		*m_ModelFile = NightfireModelFile();

		ReadHeader();

		if ( m_ReadHeaderOnly )
		{
			return;
		}

		NFMDL::NightfireModelFile& file = *m_ModelFile;
		const NFMDL::HeaderV14& header = file.Header;

		ReadElementArray(header.boneControllers, file.BoneControllers);
		ReadElementArray(header.hitBoxes, file.HitBoxes);
		ReadElementArray(header.sequences, file.Sequences);
		ReadElementArray(header.sequenceGroups, file.SequenceGroups);
		ReadElementArray(header.textures, file.Textures);
		ReadElementArray(header.attachments, file.Attachments);
		ReadElementArray(header.soundGroups, file.SoundGroups);
		ReadLevelsOfDetail();
		// TODO: Animations
		ReadElementArray(header.bodyGroups, file.BodyGroups);

		// Triangle data
		ReadElementArray(header.triangleMapOffset, header.triangleCount, file.TriangleMaps);

		// Vertex data
		ReadElementArray(header.vertexOffset, header.vertexCount, file.Vertices);
		ReadElementArray(header.normalOffset, header.vertexCount, file.Normals);
		ReadElementArray(header.textureCoOrdOffset, header.vertexCount, file.TextureCoOrdinates);
		ReadElementArray(header.boneBlendScaleOffset, header.vertexCount, file.BoneBlendScales);
		ReadElementArray(header.boneBlendOffset, header.vertexCount, file.BoneBlends);

		// Bone data
		ReadElementArray(header.bones, file.Bones);
		ReadElementArray(header.boneFixUpOffset, header.bones.count, file.BoneFixUps);

		// Model data
		ReadModels();
		// TODO: Model infos
		// TODO: Meshes

		// Sequence data
		// TODO: Events
		// TODO: Pivots
		// TODO: Blended animation data
	}

	void NightfireModelFileReader::ReadHeader()
	{
		m_ModelFile->Header = *GetElement<HeaderV14>();

		if ( m_ModelFile->Header.ident != HeaderIdentifier(FormatTraits<HeaderV14>::TARGET_IDENTIFIER) )
		{
			throw std::runtime_error("Input file header identifier did not match expected value.");
		}

		if ( m_ModelFile->Header.version != FormatTraits<HeaderV14>::TARGET_VERSION )
		{
			throw std::runtime_error("Input file header version " +
									 std::to_string(m_ModelFile->Header.version) +
									 "did not match expected version " +
									 std::to_string(FormatTraits<HeaderV14>::TARGET_VERSION) +
									 ".");
		}
	}

	void NightfireModelFileReader::ReadModels()
	{
		size_t modelCount = 0;

		for ( uint32_t index = 0; index < ArraySize(m_ModelFile->Header.modelOffset); ++index )
		{
			if ( m_ModelFile->Header.modelOffset[index] < 1 )
			{
				modelCount = index;
				break;
			}
		}

		m_ModelFile->Models.AllocateAndZero(modelCount);

		for ( uint32_t index = 0; index < modelCount; ++index )
		{
			m_ModelFile->Models[index] = *GetElement<ModelV14>(m_ModelFile->Header.modelOffset[index]);
		}
	}

	void NightfireModelFileReader::ReadLevelsOfDetail()
	{
		if ( m_ModelFile->Header.lodFlags == LOD_None )
		{
			return;
		}

		const size_t lodCount = LevelOfDetailFlagsToCount(m_ModelFile->Header.lodFlags);

		uint32_t offset = m_ModelFile->Header.sequenceGroups.offset +
			(m_ModelFile->Header.sequenceGroups.count * sizeof(SequenceGroup));

		offset = AlignTo16Bytes(offset);

		ReadElementArray(offset, lodCount, m_ModelFile->LevelsOfDetail);
	}

	uint32_t NightfireModelFileReader::AlignTo16Bytes(uint32_t offset)
	{
		// This function was called StaticMethods.Buffer(..., 16) in Ulti/Ford's
		// code, and I wasn't provided with the implementation. Given the surrounding
		// context of its use, this is my best guess as to what it was doing.

		const uint32_t modulo = offset % 16;

		if ( modulo > 0 )
		{
			offset += (16 - modulo);
		}

		return offset;
	}
}
