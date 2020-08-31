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
		ReadLevelsOfDetail();
		ReadElementArray(header.bodyGroups, file.BodyGroups);
		ReadSkins();

		// TODO: Animations

		// Sound data
		ReadElementArray(header.soundGroups, file.SoundGroups);
		ReadSounds();

		// Triangle data
		ReadElementArray(header.triangleMapOffset, header.triangleCount, file.TriangleMaps);

		// Vertex data
		ReadElementArray(header.vertexOffset, header.vertexCount, file.Vertices);
		ReadElementArray(header.normalOffset, header.vertexCount, file.Normals);
		ReadElementArray(header.textureCoOrdOffset, header.vertexCount, file.TextureCoOrdinates);
		ReadElementArray(header.vertexBlendScaleOffset, header.vertexCount, file.VertexBlendScales);
		ReadElementArray(header.vertexBlendOffset, header.vertexCount, file.VertexBlends);

		// Bone data
		ReadElementArray(header.bones, file.Bones);
		ReadElementArray(header.boneFixUpOffset, header.bones.count, file.BoneFixUps);

		// Model data
		ReadModels();
		ReadModelInfos();
		ReadMeshes();

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
		const uint32_t modelCount = m_ModelFile->Header.modelCount;
		const size_t maxModelCount = ArraySize(m_ModelFile->Header.modelOffset);

		if ( modelCount > maxModelCount )
		{
			throw std::runtime_error("Header provided model count of " +
									 std::to_string(modelCount) +
									 " when maximum allowed is " +
									 std::to_string(maxModelCount) +
									 ".");
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

	void NightfireModelFileReader::ReadSkins()
	{
		const uint32_t skinCount = m_ModelFile->Header.skinReferenceCount * m_ModelFile->Header.skinFamilyCount;
		const Skin* skinElements = GetElement<Skin>(m_ModelFile->Header.skinDataOffset, skinCount);

		for ( uint32_t family = 0; family < m_ModelFile->Header.skinFamilyCount; ++family )
		{
			for ( uint32_t reference = 0; reference < m_ModelFile->Header.skinReferenceCount; ++reference )
			{
				NightfireModelFile::SkinCollectionKey key;
				key.skinFamily = family;
				key.skinReference = reference;

				m_ModelFile->Skins.emplace(key, *(skinElements++));
			}
		}
	}

	void NightfireModelFileReader::ReadModelInfos()
	{
		const size_t modelCount = m_ModelFile->Models.Count();

		for ( uint32_t modelIndex = 0; modelIndex < modelCount; ++modelIndex )
		{
			const ModelV14& model = m_ModelFile->Models[modelIndex];

			for ( uint32_t modelInfoIndex = 0; modelInfoIndex < ArraySize(model.modelInfoOffset); ++modelInfoIndex )
			{
				const uint32_t modelInfoOffset = model.modelInfoOffset[modelInfoIndex];

				if ( modelInfoOffset < 1 )
				{
					continue;
				}

				NightfireModelFile::TOwnedItemKey<ModelInfoV14> key;
				key.ownerIndex = modelIndex;
				key.itemIndex = modelInfoIndex;

				m_ModelFile->ModelInfos.emplace(key, *GetElement<ModelInfoV14>(modelInfoOffset));
			}
		}
	}

	// TODO: Combine with above function?
	void NightfireModelFileReader::ReadMeshes()
	{
		const size_t modelCount = m_ModelFile->Models.Count();

		for ( uint32_t modelIndex = 0; modelIndex < modelCount; ++modelIndex )
		{
			const ModelV14& model = m_ModelFile->Models[modelIndex];

			for ( uint32_t modelInfoIndex = 0; modelInfoIndex < ArraySize(model.modelInfoOffset); ++modelInfoIndex )
			{
				if ( model.modelInfoOffset[modelInfoIndex] < 1 )
				{
					continue;
				}

				NightfireModelFile::TOwnedItemKey<ModelInfoV14> key;
				key.ownerIndex = modelIndex;
				key.itemIndex = modelInfoIndex;

				const ModelInfoV14& modelInfo = m_ModelFile->ModelInfos[key];
				const uint32_t meshCount = modelInfo.meshes.count;
				const MeshV14* meshElements = GetElement<MeshV14>(modelInfo.meshes.offset, modelInfo.meshes.count);

				for ( uint32_t meshIndex = 0; meshIndex < meshCount; ++meshIndex )
				{
					NightfireModelFile::MeshCollectionKey meshKey;
					meshKey.modelIndex = modelIndex;
					meshKey.modelInfoIndex = modelInfoIndex;
					meshKey.meshIndex = meshIndex;

					m_ModelFile->Meshes.emplace(meshKey, meshElements[meshIndex]);
				}
			}
		}
	}

	void NightfireModelFileReader::ReadSounds()
	{
		const size_t soundGroupCount = m_ModelFile->SoundGroups.Count();
		const uint32_t soundsOffset = m_ModelFile->Header.soundGroups.offset + (m_ModelFile->Header.soundGroups.count * sizeof(SoundGroupV14));

		for ( uint32_t soundGroupIndex = 0; soundGroupIndex < soundGroupCount; ++soundGroupIndex )
		{
			const SoundGroupV14& soundGroup = m_ModelFile->SoundGroups[soundGroupIndex];

			NightfireModelFile::TOwnedItemKey<SoundV14> key;
			key.itemIndex = 0;
			key.ownerIndex = soundGroupIndex;

			m_ModelFile->Sounds.emplace(key, *GetElement<SoundV14>(soundsOffset + soundGroup.offset));
		}
	}

	void NightfireModelFileReader::ReadSequenceAnimationData()
	{
#if 0
		const size_t sequenceCount = m_ModelFile->Sequences.Count();
		const size_t boneCount = m_ModelFile->Bones.Count();

		for ( uint32_t sequenceIndex = 0; sequenceIndex < sequenceCount; ++sequenceIndex )
		{
			const SequenceV14& sequence = m_ModelFile->Sequences[sequenceIndex];
			const uint32_t animationDataOffset = sequence.animationDataOffset;
			uint32_t currentDataOffset = animationDataOffset;

			for ( uint32_t blendIndex = 0; blendIndex < sequence.blendCount; ++blendIndex )
			{
				for ( uint32_t boneIndex = 0; boneIndex < boneCount; ++boneIndex )
				{
					const uint32_t boneDataOffsetBase = currentDataOffset;

					// We assume that each of the 6 components correspond to X/Y/Z/XR/YR/ZR.
					// At the beginning of the row are 6 offsets, corresponding to the beginning of the data
					// in the row for for each component.
					// If an offset is 0, it means there is no data for this component.
					const uint16_t* dataOffsetForComponent = GetElement<uint16_t>(currentDataOffset, FormatTraits<HeaderV14>::NUM_ANIMATION_COMPONENTS);

					// Check each component.
					for ( uint32_t componentIndex = 0; componentIndex < FormatTraits<HeaderV14>::NUM_ANIMATION_COMPONENTS; ++componentIndex )
					{
						if ( dataOffsetForComponent[componentIndex] < 1 )
						{
							continue;
						}

						NightfireModelFile::BlendedAnimationCollectionKey key;
						key.sequenceIndex = sequenceIndex;
						key.blendIndex = blendIndex;
						key.boneIndex = boneIndex;
						key.componentIndex = componentIndex;

						m_ModelFile->BlendedAnimationData[key] = NightfireModelFile::BlendedAnimationValueList();
						NightfireModelFile::BlendedAnimationValueList& valueList = m_ModelFile->BlendedAnimationData[key];

						currentDataOffset = boneDataOffsetBase + dataOffsetForComponent[componentIndex];

						size_t numFramesRead = 0;

						while ( numFramesRead < sequence.frameCount )
						{
							const AnimationValue* animValue = GetElement<AnimationValue>(currentDataOffset);
							currentDataOffset += sizeof(AnimationValue);

							// We will read as many frames as the specified total.
							const uint8_t numFramesToRead = animValue->span.total;

							// Resize the value list to account for the new frames.
							const size_t valueOffset = valueList.size();
							valueList.resize(valueOffset + numFramesToRead);

							// The number of "valid" frames will be <= the number of total frames.
							// I'm assuming that when blending sequences of different lengths, a
							// shorter sequence's length is "valid" frames long, and the longest
							// sequence's length is "total" frames long. The shorter sequences are
							// then padded to the total length by copying their last frame.
							const uint8_t numValidFrames = animValue->span.valid;

							const AnimationValue* valuesToRead = GetElement<AnimationValue>(currentDataOffset, numFramesToRead);

							// First of all, read all the valid frames.
							for ( uint32_t validFrameIndex = 0;
								  validFrameIndex < numValidFrames;
								  ++validFrameIndex, ++numFramesRead )
							{
								valueList[valueOffset + validFrameIndex] = valuesToRead[validFrameIndex].value;
							}

							// Then pad with the final frame.
							const int16_t finalValue = valuesToRead[numValidFrames - 1].value;
							for ( uint32_t paddingFrameIndex = numValidFrames;
								  paddingFrameIndex < numFramesToRead;
								  ++paddingFrameIndex, ++numFramesRead )
							{
								valueList[valueOffset + paddingFrameIndex] = finalValue;
							}
						}
					}
				}
			}
		}
#endif
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
