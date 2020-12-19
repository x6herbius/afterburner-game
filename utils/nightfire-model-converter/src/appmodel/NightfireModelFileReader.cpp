#include <fstream>
#include <iterator>

#include "Types.h"
#include "NightfireModelFileReader.h"
#include "elements/HeaderV14.h"
#include "elements/BoneAnimationDataOffsets.h"
#include "elements/AnimationValueMeta.h"
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

		ReadElements(header.boneControllers, file.BoneControllers);
		ReadElementArray(header.hitBoxes, file.HitBoxes);
		ReadElements(header.sequences, file.Sequences);
		ReadElementArray(header.sequenceGroups, file.SequenceGroups);
		ReadElementArray(header.textures, file.Textures);
		ReadElementArray(header.attachments, file.Attachments);
		ReadLevelsOfDetail();
		ReadElementArray(header.bodyGroups, file.BodyGroups);
		ReadSkins();

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
		ReadEvents();
		ReadFootPivots();
		ReadSequenceAnimationData();
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

		m_ModelFile->Models.AllocateDefault(modelCount);

		for ( uint32_t index = 0; index < modelCount; ++index )
		{
			const uint32_t offset = m_ModelFile->Header.modelOffset[index];
			AugmentedModelV14& model = m_ModelFile->Models[index];

			model = *GetElement<ModelV14>(offset);
			model.SetFileOffset(offset);
		}
	}

	void NightfireModelFileReader::ReadLevelsOfDetail()
	{
		if ( m_ModelFile->Header.lodFlags == LOD_None )
		{
			return;
		}

		const size_t lodCount = LevelOfDetailFlagsToCount(m_ModelFile->Header.lodFlags);

		m_ModelFile->LODOffset = m_ModelFile->Header.sequenceGroups.offset +
			(m_ModelFile->Header.sequenceGroups.count * sizeof(SequenceGroup));

		m_ModelFile->LODOffset = AlignTo16Bytes(m_ModelFile->LODOffset);

		ReadElementArray(m_ModelFile->LODOffset, lodCount, m_ModelFile->LevelsOfDetail);
	}

	void NightfireModelFileReader::ReadSkins()
	{
		m_ModelFile->Skins.clear();

		const uint32_t skinCount = m_ModelFile->Header.skinReferenceCount * m_ModelFile->Header.skinFamilyCount;
		const Skin* skinElements = GetElement<Skin>(m_ModelFile->Header.skinDataOffset, skinCount);

		for ( uint32_t family = 0; family < m_ModelFile->Header.skinFamilyCount; ++family )
		{
			for ( uint32_t reference = 0; reference < m_ModelFile->Header.skinReferenceCount; ++reference )
			{
				SkinCollectionKey key;
				key.skinFamily = family;
				key.skinReference = reference;

				m_ModelFile->Skins.emplace(key, *(skinElements++));
			}
		}
	}

	void NightfireModelFileReader::ReadModelInfos()
	{
		m_ModelFile->ModelInfos.clear();

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

				TOwnedItemKey<AugmentedModelInfoV14> key;
				key.ownerIndex = modelIndex;
				key.itemIndex = modelInfoIndex;

				m_ModelFile->ModelInfos.emplace(key, *GetElement<ModelInfoV14>(modelInfoOffset));
				m_ModelFile->ModelInfos[key].SetFileOffset(modelInfoOffset);
			}
		}
	}

	// TODO: Combine with above function?
	void NightfireModelFileReader::ReadMeshes()
	{
		m_ModelFile->Meshes.clear();

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

				TOwnedItemKey<AugmentedModelInfoV14> key;
				key.ownerIndex = modelIndex;
				key.itemIndex = modelInfoIndex;

				const AugmentedModelInfoV14& modelInfo = m_ModelFile->ModelInfos[key];
				const uint32_t meshCount = modelInfo.meshes.count;
				const MeshV14* meshElements = GetElement<MeshV14>(modelInfo.meshes.offset, modelInfo.meshes.count);

				for ( uint32_t meshIndex = 0; meshIndex < meshCount; ++meshIndex )
				{
					MeshCollectionKeyV14 meshKey;
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
		m_ModelFile->Sounds.clear();

		const size_t soundGroupCount = m_ModelFile->SoundGroups.Count();
		const uint32_t soundsOffset = m_ModelFile->Header.soundGroups.offset + (m_ModelFile->Header.soundGroups.count * sizeof(SoundGroupV14));

		for ( uint32_t soundGroupIndex = 0; soundGroupIndex < soundGroupCount; ++soundGroupIndex )
		{
			const SoundGroupV14& soundGroup = m_ModelFile->SoundGroups[soundGroupIndex];

			TOwnedItemKey<SoundV14> key;
			key.itemIndex = 0;
			key.ownerIndex = soundGroupIndex;

			m_ModelFile->Sounds.emplace(key, *GetElement<SoundV14>(soundsOffset + soundGroup.offset));
		}
	}

	void NightfireModelFileReader::ReadEvents()
	{
		m_ModelFile->Events.clear();

		const size_t sequenceCount = m_ModelFile->Sequences.Count();

		for ( uint32_t sequenceIndex = 0; sequenceIndex < sequenceCount; ++sequenceIndex )
		{
			const SequenceV14* sequence = m_ModelFile->Sequences.ElementAt(sequenceIndex);
			const Event* events = GetElement<Event>(sequence->events.offset, sequence->events.count);

			for ( uint32_t eventIndex = 0; eventIndex < sequence->events.count; ++eventIndex )
			{
				TOwnedItemKey<Event> key;
				key.ownerIndex = sequenceIndex;
				key.itemIndex = eventIndex;

				m_ModelFile->Events.emplace(key, events[eventIndex]);
			}
		}
	}

	void NightfireModelFileReader::ReadFootPivots()
	{
		m_ModelFile->FootPivots.Clear();
		const size_t sequenceCount = m_ModelFile->Sequences.Count();

		for ( uint32_t sequenceIndex = 0; sequenceIndex < sequenceCount; ++sequenceIndex )
		{
			const SequenceV14* sequence = m_ModelFile->Sequences.ElementAt(sequenceIndex);
			const FootPivot* footPivots = GetElement<FootPivot>(sequence->footPivots.offset, sequence->footPivots.count);

			const size_t footPivotsBaseIndex = m_ModelFile->FootPivots.Count();
			m_ModelFile->FootPivots.AppendDefault(sequence->footPivots.count);

			for ( uint32_t footPivotIndex = 0; footPivotIndex < sequence->footPivots.count; ++footPivotIndex )
			{
				TOwnedItemKey<FootPivot> key;
				key.ownerIndex = sequenceIndex;
				key.itemIndex = footPivotIndex;

				m_ModelFile->FootPivots.AssignMappingAndValue(key, footPivotsBaseIndex + footPivotIndex, footPivots[footPivotIndex]);
			}
		}
	}

	void NightfireModelFileReader::ReadSequenceAnimationData()
	{
		m_ModelFile->AnimationData.clear();

		const size_t sequenceCount = m_ModelFile->Sequences.Count();
		const size_t boneCount = m_ModelFile->Bones.Count();

		for ( uint32_t sequenceIndex = 0; sequenceIndex < sequenceCount; ++sequenceIndex )
		{
			const SequenceV14* sequence = m_ModelFile->Sequences.ElementAt(sequenceIndex);

			// Strap in - things get a little convoluted here.

			// One entire sequence may be comprised of multiple other sequences that are blended together
			// dynamically by the engine, eg. depending on the direction in which a player character is running.
			// Each sub-sequence (referred to as a "blend sequence") makes use of all the bones that are present
			// in the model, and each bone references per-frame animation data for its X, Y and Z positions, and
			// also its X, Y and Z rotations.

			// The animation data itself is laid out in two macro-blocks: the second block is the raw animation
			// values (what value should be assigned to X/Y/Z/XR/YR/ZR on each frame), and the first block holds
			// offsets into this data depending on which blend sequence, bone and component the data refers to.
			// For example, an offset might be used to work out the value for the X position of bone 5 in blend
			// sequence 2 on frame 15.

			// The offsets block is comprised of many consecutive BoneAnimationDataOffsets elements, and there
			// are (blendCount * boneCount) elements present within the offsets block. Each of these elements
			// holds 6 offsets, to correspond to X/Y/Z/XR/YR/ZR data, and each of the offsets is relative to
			// the beginning of the element. If an offset is zero, it means there is no data for this particular
			// blend/bone/component combination.

			// After adding a BoneAnimationDataOffsets value to the location of the BoneAnimationDataOffsets
			// element itself, the animation values for that particular blend/bone/component combination can be
			// read. The data is provided for as many frames as there are in the original sequence, and is
			// run-length encoded. It begins with a AnimationValueMeta element, which provides two values:
			// "valid", which is how many values may explicitly be found in this particular RLE run, and "total",
			// which is the total number of frames *implicitly* provided in the run. If total > valid, any
			// frames in-between are just duplicates of the final valid frame in the run (index valid-1).
			// Multiple RLE runs may need to be traversed in order to read all frames of data. The animation
			// values themselves are signed 16-bit integers.

			// The following properties should be validated when reading animation data for a blend/bone/component
			// combination:
			// - For all RLE runs in one "row" of values, the sum of all the "totals" should be exactly equal to
			//   the number of frames in the original sequence.
			// - For any RLE run, "valid" and "total" should be at least 1.
			// - For any RLE run, "total" should be >= "valid".

			const size_t numDataOffsets = sequence->blendCount * boneCount;
			const BoneAnimationDataOffsets* dataOffsets = GetElement<BoneAnimationDataOffsets>(sequence->animationDataOffset, numDataOffsets);

			for ( uint32_t blendIndex = 0; blendIndex < sequence->blendCount; ++blendIndex )
			{
				for ( uint32_t boneIndex = 0; boneIndex < boneCount; ++boneIndex )
				{
					// Find which data offset group we should use.
					// Each blend index corresponds to a stride of boneCount items.
					const uint32_t currentDataOffsetsIndex = (blendIndex * boneCount) + boneIndex;
					const BoneAnimationDataOffsets& currentDataOffsets = dataOffsets[currentDataOffsetsIndex];

					// For each component, the offset to the raw animation data is relative to the beginning of the
					// currentDataOffsets data we're reading.
					const uint32_t rawDataOffsetBase = sequence->animationDataOffset + (currentDataOffsetsIndex * sizeof(BoneAnimationDataOffsets));

					for ( uint32_t componentIndex = 0; componentIndex < ArraySize(currentDataOffsets.dataOffsetForComponent); ++componentIndex )
					{
						if ( currentDataOffsets.dataOffsetForComponent[componentIndex] < 1 )
						{
							// No data present - ignore.
							continue;
						}

						AnimationDataCollectionKey key;
						key.sequenceIndex = sequenceIndex;
						key.blendIndex = blendIndex;
						key.boneIndex = boneIndex;
						key.componentIndex = componentIndex;

						m_ModelFile->AnimationData[key] = AnimationDataValueList();

						// Locate the raw data.
						const uint32_t rawDataOffset = rawDataOffsetBase + currentDataOffsets.dataOffsetForComponent[componentIndex];

						try
						{
							ReadRLEAnimationData(m_ModelFile->AnimationData[key], rawDataOffset, sequence->frameCount);
						}
						catch ( const std::runtime_error& ex )
						{
							std::stringstream stream;

							stream
								<< "Error reading animation data for sequence "
								<< sequenceIndex
								<< ", blend "
								<< blendIndex
								<< ", bone "
								<< boneIndex
								<< ", component "
								<< componentIndex
								<< ". "
								<< ex.what();

							throw std::runtime_error(stream.str());
						}
					}
				}
			}
		}
	}

	void NightfireModelFileReader::ReadRLEAnimationData(AnimationDataValueList& valueList,
														uint32_t dataOffset,
														uint32_t frameCount)
	{
		size_t numFramesRead = 0;

		while ( numFramesRead < frameCount )
		{
			// The first item will always be a "meta" item, giving the number of valid
			// frames vs. the total number of frames in the span.
			const AnimationValueMeta* animMeta = GetElement<AnimationValueMeta>(dataOffset);
			dataOffset += sizeof(AnimationValueMeta);

			if ( animMeta->valid < 1 )
			{
				throw std::runtime_error("Encountered RLE block with 0 valid frames.");
			}

			if ( animMeta->total < 1 )
			{
				throw std::runtime_error("Encountered RLE block with 0 total frames.");
			}

			if ( animMeta->total < animMeta->valid )
			{
				throw std::runtime_error("Encountered RLE block with number of valid frames (" +
										 std::to_string(animMeta->valid) +
										 ") greater than number of total frames (" +
										 std::to_string(animMeta->total) +
										 ").");
			}

			// We will read as many frames as the specified total.
			const uint8_t numFramesToRead = animMeta->total;

			// Resize the value list to account for the new frames.
			const size_t valueOffset = valueList.size();
			valueList.resize(valueOffset + numFramesToRead);

			// The number of "valid" frames will be <= the number of total frames.
			// I'm assuming that when blending sequences of different lengths, a
			// shorter sequence's length is "valid" frames long, and the longest
			// sequence's length is "total" frames long. The shorter sequences are
			// then padded to the total length by copying their last frame.
			const uint8_t numValidFrames = animMeta->valid;

			// Get a pointer to the raw data.
			const AnimationValue* animValues = GetElement<AnimationValue>(dataOffset, numValidFrames);
			dataOffset += numValidFrames * sizeof(AnimationValue);

			// First of all, read all the valid frames.
			for ( uint32_t validFrameIndex = 0;
					validFrameIndex < numValidFrames;
					++validFrameIndex, ++numFramesRead )
			{
				valueList[valueOffset + validFrameIndex] = animValues[validFrameIndex].value;
			}

			// Then pad with the final frame.
			const int16_t finalValue = animValues[numValidFrames - 1].value;

			for ( uint32_t paddingFrameIndex = numValidFrames;
					paddingFrameIndex < numFramesToRead;
					++paddingFrameIndex, ++numFramesRead )
			{
				valueList[valueOffset + paddingFrameIndex] = finalValue;
			}
		}

		if ( numFramesRead != frameCount )
		{
			throw std::runtime_error("Expected to read data for " +
									 std::to_string(frameCount) +
									 " frames, but was provided with " +
									 std::to_string(numFramesRead) +
									 " frames.");
		}
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
