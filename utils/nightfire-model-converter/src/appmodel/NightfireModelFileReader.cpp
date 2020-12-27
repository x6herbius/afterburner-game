#include <fstream>
#include <iterator>
#include <iostream>

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

	bool NightfireModelFileReader::Verbose() const
	{
		return m_Verbose;
	}

	void NightfireModelFileReader::SetVerbose(bool verbose)
	{
		m_Verbose = verbose;
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

		m_InputFileData = CreateStreamBuffer(input);

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
		ReadElements(header.hitBoxes, file.HitBoxes);
		ReadElements(header.sequences, file.Sequences);
		ReadElements(header.sequenceGroups, file.SequenceGroups);
		ReadElements(header.textures, file.Textures);
		ReadElements(header.attachments, file.Attachments);
		ReadLevelsOfDetail();
		ReadElements(header.bodyGroups, file.BodyGroups);
		ReadSkins();

		// Sound data
		ReadElements(header.soundGroups, file.SoundGroups);
		ReadSounds();

		// Triangle data
		ReadElements(header.triangleMapOffset, header.triangleCount, file.TriangleMaps);

		// Vertex data
		ReadElements(header.vertexOffset, header.vertexCount, file.Vertices);
		ReadElements(header.normalOffset, header.vertexCount, file.Normals);
		ReadElements(header.textureCoOrdOffset, header.vertexCount, file.TextureCoOrdinates);
		ReadElements(header.vertexBlendScaleOffset, header.vertexCount, file.VertexBlendScales);
		ReadElements(header.vertexBlendOffset, header.vertexCount, file.VertexBlends);

		// Bone data
		ReadElements(header.bones, file.Bones);
		ReadElements(header.boneFixUpOffset, header.bones.count, file.BoneFixUps);

		// Model data
		ReadModelsAndChildElements();

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
									 " did not match expected version " +
									 std::to_string(FormatTraits<HeaderV14>::TARGET_VERSION) +
									 ".");
		}

		if ( m_InputFileData->size() != m_ModelFile->Header.length )
		{
			throw std::runtime_error("Input file header specified length of " +
									 std::to_string(m_ModelFile->Header.length) +
									 " bytes when file was actually " +
									 std::to_string(m_InputFileData->size()) +
									 "bytes long.");
		}

		if ( m_Verbose )
		{
			const HeaderV14& header = m_ModelFile->Header;

			std::cout
				<< "Header: "
				<< header.ident.ToString()
				<< " version " << header.version
				<< ", file length: " << header.length << " bytes."
				<< std::endl;
		}
	}

	void NightfireModelFileReader::ReadModelsAndChildElements()
	{
		m_ModelFile->Models.Clear();
		m_ModelFile->ModelInfos.Clear();
		m_ModelFile->Meshes.Clear();

		for ( auto it : m_ModelFile->BodyGroups )
		{
			for ( size_t bodyGroupModelIndex = 0; bodyGroupModelIndex < it.element->modelCount; ++bodyGroupModelIndex )
			{
				const uint32_t modelOffset = it.element->modelOffset + (bodyGroupModelIndex * sizeof(ModelV14));
				const size_t headerModelIndex = ModelIndexForOffset(modelOffset);

				TOwnedItemKey<ModelV14> modelKey;
				modelKey.ownerIndex = it.index;
				modelKey.itemIndex = bodyGroupModelIndex;

				const size_t modelIndex = m_ModelFile->Models.Count();

				if ( m_Verbose )
				{
					std::cout
						<< "Reading model ("
						<< sizeof(ModelV14)
						<< " bytes) from offset "
						<< modelOffset
						<< " (index "
						<< modelIndex				// Index in models collection
						<< ", header index "
						<< headerModelIndex			// Index in models array in header
						<< ", item "
						<< modelKey.itemIndex		// Child number in owner body group
						<< " owned by body group "
						<< modelKey.ownerIndex		// Index of owner body group
						<< ")"
						<< std::endl;
				}

				m_ModelFile->Models.AppendFrom(GetElement<ModelV14>(modelOffset), 1);
				m_ModelFile->Models.AssignMapping(modelKey, modelIndex);

				ModelUserDataV14* const userData = m_ModelFile->Models.UserDataAt(modelIndex);
				userData->SetFileOffset(modelOffset);
				userData->SetIndexInHeader(headerModelIndex);

				BodyGroupUserDataV14* const bodyGroupUserData = it.userData;

				if ( bodyGroupUserData->ChildModelsBeginIndex() == INVALID_CONTAINER_INDEX )
				{
					bodyGroupUserData->SetChildModelsBeginIndex(modelIndex);
				}

				bodyGroupUserData->IncrementChildModelsCount();

				ReadModelInfos(it.index, modelIndex);
			}
		}

		if ( m_ModelFile->Models.Count() != m_ModelFile->Header.modelCount )
		{
			throw std::runtime_error("Header provided model count of " +
									 std::to_string(m_ModelFile->Header.modelCount) +
									 " when number of valid models was " +
									 std::to_string(m_ModelFile->Models.Count()) +
									 ".");
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

		ReadElements(m_ModelFile->LODOffset, lodCount, m_ModelFile->LevelsOfDetail);
	}

	void NightfireModelFileReader::ReadSkins()
	{
		m_ModelFile->Skins.Clear();

		const uint32_t skinCount = m_ModelFile->Header.skinReferenceCount * m_ModelFile->Header.skinFamilyCount;
		m_ModelFile->Skins.AllocateDefault(skinCount);
		size_t currentSkinIndex = 0;

		const Skin* skinElements = GetElement<Skin>(m_ModelFile->Header.skinDataOffset, skinCount);

		for ( uint32_t family = 0; family < m_ModelFile->Header.skinFamilyCount; ++family )
		{
			for ( uint32_t reference = 0; reference < m_ModelFile->Header.skinReferenceCount; ++reference )
			{
				SkinCollectionKey key;
				key.skinFamily = family;
				key.skinReference = reference;

				if ( m_Verbose )
				{
					uint32_t offset = m_ModelFile->Header.skinDataOffset;
					offset += (family * m_ModelFile->Header.skinReferenceCount) * sizeof(Skin);
					offset += reference * sizeof(Skin);

					std::cout
						<< "Reading skin ("
						<< sizeof(Skin)
						<< " bytes from offset "
						<< offset
						<< ") for family "
						<< family
						<< ", reference "
						<< reference
						<< std::endl;
				}

				m_ModelFile->Skins.AssignMappingAndValue(key, currentSkinIndex++, *(skinElements++));
			}
		}
	}

	void NightfireModelFileReader::ReadModelInfos(size_t bodyGroupGlobalIndex, size_t modelGlobalIndex)
	{
		const ModelV14* const model = m_ModelFile->Models.ElementAt(modelGlobalIndex);
		ModelUserDataV14* const modelUserData = m_ModelFile->Models.UserDataAt(modelGlobalIndex);
		assert(model);
		assert(modelUserData);

		const size_t modelInfoElementBase = m_ModelFile->ModelInfos.Count();

		for ( size_t modelInfoArrayIndex = 0; modelInfoArrayIndex < ArraySize(model->modelInfoOffset); ++modelInfoArrayIndex )
		{
			const uint32_t modelInfoOffset = model->modelInfoOffset[modelInfoArrayIndex];

			if ( modelInfoOffset < 1 )
			{
				continue;
			}

			ModelInfoCollectionKeyV14 key;
			key.bodyGroupIndex = bodyGroupGlobalIndex;
			key.modelIndex = m_ModelFile->Models.KeyFor(modelGlobalIndex).itemIndex;
			key.modelInfoIndex = m_ModelFile->ModelInfos.Count() - modelInfoElementBase;

			const size_t modelInfoElementIndex = m_ModelFile->ModelInfos.Count();

			if ( m_Verbose )
			{
				std::cout
					<< "Reading model info ("
					<< sizeof(ModelInfoV14)
					<< " bytes) from offset "
					<< modelInfoOffset
					<< " (global index "
					<< modelInfoElementIndex
					<< ", local index "
					<< key.modelInfoIndex
					<< ", owned by body group "
					<< key.bodyGroupIndex
					<< " and model "
					<< key.modelIndex
					<< ")"
					<< std::endl;
			}

			m_ModelFile->ModelInfos.AppendDefault();
			m_ModelFile->ModelInfos.AssignMappingAndValue(key, modelInfoElementIndex, *GetElement<ModelInfoV14>(modelInfoOffset));
			m_ModelFile->ModelInfos.UserDataAt(modelInfoElementIndex)->SetFileOffset(modelInfoOffset);

			if ( modelUserData->ChildModelInfosBeginIndex() == INVALID_CONTAINER_INDEX )
			{
				modelUserData->SetChildModelInfosBeginIndex(modelInfoElementIndex);
			}

			modelUserData->IncrementChildModelInfosCount();
		}
	}

	void NightfireModelFileReader::ReadMeshes(size_t bodyGroupGlobalIndex, size_t modelGlobalIndex, size_t modelInfoGlobalIndex)
	{
		const ModelInfoV14* modelInfo = m_ModelFile->ModelInfos.ElementAt(modelInfoGlobalIndex);
		ModelInfoUserDataV14* modelInfoUserData = m_ModelFile->ModelInfos.UserDataAt(modelInfoGlobalIndex);
		assert(modelInfo);
		assert(modelInfoUserData);

		const size_t meshElementBase = m_ModelFile->Meshes.Count();
		const uint32_t meshCount = modelInfo->meshes.count;
		const MeshV14* meshElements = GetElement<MeshV14>(modelInfo->meshes.offset, modelInfo->meshes.count);

		for ( uint32_t meshIndex = 0; meshIndex < meshCount; ++meshIndex )
		{
			MeshCollectionKeyV14 meshKey;
			meshKey.bodyGroupIndex = bodyGroupGlobalIndex;
			meshKey.modelIndex = m_ModelFile->Models.KeyFor(modelGlobalIndex).itemIndex;
			meshKey.modelInfoIndex = m_ModelFile->ModelInfos.KeyFor(modelInfoGlobalIndex).modelInfoIndex;
			meshKey.meshIndex = meshIndex;

			const size_t meshElementIndex = m_ModelFile->Meshes.Count();

			if ( m_Verbose )
			{
				std::cout
					<< "Reading mesh ("
					<< sizeof(MeshV14)
					<< " bytes) from offset "
					<< modelInfo->meshes.offset + (meshIndex * sizeof(MeshV14))
					<< " (global index "
					<< meshElementIndex
					<< ", local index "
					<< meshIndex
					<< ", owned by body group "
					<< meshKey.bodyGroupIndex
					<< ", model "
					<< meshKey.modelIndex
					<< ", and model info "
					<< meshKey.modelInfoIndex
					<< ")"
					<< std::endl;
			}

			m_ModelFile->Meshes.AppendDefault();
			m_ModelFile->Meshes.AssignMappingAndValue(meshKey, meshElementIndex, meshElements[meshIndex]);
			m_ModelFile->Meshes.UserDataAt(meshElementIndex)->SetFileOffset(modelInfo->meshes.offset + (meshIndex * sizeof(MeshV14)));

			if ( modelInfoUserData->ChildMeshesBeginIndex() == INVALID_CONTAINER_INDEX )
			{
				modelInfoUserData->SetChildMeshesBeginIndex(meshElementIndex);
			}

			modelInfoUserData->IncrementChildMeshesCount();
		}
	}

	void NightfireModelFileReader::ReadSounds()
	{
		m_ModelFile->Sounds.Clear();

		const size_t soundGroupCount = m_ModelFile->SoundGroups.Count();
		const uint32_t soundsOffset = m_ModelFile->Header.soundGroups.offset + (m_ModelFile->Header.soundGroups.count * sizeof(SoundGroupV14));

		for ( uint32_t soundGroupIndex = 0; soundGroupIndex < soundGroupCount; ++soundGroupIndex )
		{
			const SoundGroupV14* soundGroup = m_ModelFile->SoundGroups.ElementAt(soundGroupIndex);

			TOwnedItemKey<SoundV14> key;
			key.itemIndex = 0;
			key.ownerIndex = soundGroupIndex;

			const size_t soundElementIndex = m_ModelFile->Sounds.Count();

			if ( m_Verbose )
			{
				std::cout
					<< "Reading model ("
					<< sizeof(SoundV14)
					<< " bytes) from offset "
					<< soundsOffset + soundGroup->offset
					<< " (global index "
					<< soundElementIndex
					<< ", owned by sound group "
					<< soundGroupIndex
					<< ")"
					<< std::endl;
			}

			m_ModelFile->Sounds.AppendDefault();
			m_ModelFile->Sounds.AssignMappingAndValue(key, soundElementIndex, *GetElement<SoundV14>(soundsOffset + soundGroup->offset));
		}
	}

	void NightfireModelFileReader::ReadEvents()
	{
		m_ModelFile->Events.Clear();
		const size_t sequenceCount = m_ModelFile->Sequences.Count();

		for ( uint32_t sequenceIndex = 0; sequenceIndex < sequenceCount; ++sequenceIndex )
		{
			const SequenceV14* sequence = m_ModelFile->Sequences.ElementAt(sequenceIndex);
			const Event* events = GetElement<Event>(sequence->events.offset, sequence->events.count);

			const size_t eventBaseIndex = m_ModelFile->Events.Count();
			m_ModelFile->Events.AppendDefault(sequence->events.count);

			for ( uint32_t eventIndex = 0; eventIndex < sequence->events.count; ++eventIndex )
			{
				if ( m_Verbose )
				{
					std::cout
						<< "Reading event ("
						<< sizeof(Event)
						<< " bytes) from offset "
						<< sequence->events.offset + (eventIndex * sizeof(Event))
						<< " (global index "
						<< eventBaseIndex + eventIndex
						<< ", owned by sequence "
						<< sequenceIndex
						<< ")"
						<< std::endl;
				}

				TOwnedItemKey<Event> key;
				key.ownerIndex = sequenceIndex;
				key.itemIndex = eventIndex;

				m_ModelFile->Events.AssignMappingAndValue(key, eventBaseIndex + eventIndex, events[eventIndex]);
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
				if ( m_Verbose )
				{
					std::cout
						<< "Reading foot pivot ("
						<< sizeof(FootPivot)
						<< " bytes) from offset "
						<< sequence->footPivots.offset + (footPivotIndex * sizeof(FootPivot))
						<< " (global index "
						<< footPivotsBaseIndex + footPivotIndex
						<< ", owned by sequence "
						<< sequenceIndex
						<< ")"
						<< std::endl;
				}

				TOwnedItemKey<FootPivot> key;
				key.ownerIndex = sequenceIndex;
				key.itemIndex = footPivotIndex;

				m_ModelFile->FootPivots.AssignMappingAndValue(key, footPivotsBaseIndex + footPivotIndex, footPivots[footPivotIndex]);
			}
		}
	}

	void NightfireModelFileReader::ReadSequenceAnimationData()
	{
		m_ModelFile->AnimationData.Clear();

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

						const size_t animationDataElementIndex = m_ModelFile->AnimationData.Count();
						m_ModelFile->AnimationData.AppendDefault();
						m_ModelFile->AnimationData.AssignMappingAndValue(key, animationDataElementIndex, AnimationDataValueList());

						// Locate the raw data.
						const uint32_t rawDataOffset = rawDataOffsetBase + currentDataOffsets.dataOffsetForComponent[componentIndex];

						if ( m_Verbose )
						{
							std::cout
								<< "Reading animation data ("
								<< sequence->frameCount
								<< " frames) from offset "
								<< rawDataOffset
								<< " (owned by sequence "
								<< sequenceIndex
								<< ", blend "
								<< blendIndex
								<< ", bone "
								<< boneIndex
								<< ", component "
								<< componentIndex
								<< ")"
								<< std::endl;
						}

						try
						{
							ReadRLEAnimationData(*m_ModelFile->AnimationData.ElementAt(animationDataElementIndex), rawDataOffset, sequence->frameCount);
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

	TOwnedItemKey<ModelV14> NightfireModelFileReader::BodyGroupReferencingModelAtOffset(uint32_t modelOffset) const
	{
		if ( modelOffset < 1 )
		{
			throw std::runtime_error("Cannot find bodygroup referencing model: model offset " +
									 std::to_string(modelOffset) +
									 " is invalid.");
		}

		TOwnedItemKey<ModelV14> key;

		for ( auto it : m_ModelFile->BodyGroups )
		{
			for ( uint32_t modelIndex = 0; modelIndex < it.element->modelCount; ++modelIndex )
			{
				const uint32_t checkModelOffset = it.element->modelOffset + (modelIndex * sizeof(ModelV14));

				if ( checkModelOffset == modelOffset )
				{
					// Check that no other body group also referenced this model.
					if ( key )
					{
						throw std::runtime_error("Cannot find bodygroup referencing model: body group " +
												 std::to_string(it.index) +
												 " references model at offset " +
												 std::to_string(modelOffset) +
												 " when this model is already referenced by body group " +
												 std::to_string(key.ownerIndex) +
												 ".");
					}
					else
					{
						key.ownerIndex = it.index;
						key.itemIndex = modelIndex;
					}
				}
			}
		}

		if ( !key )
		{
			throw std::runtime_error("Cannot find bodygroup referencing model: model at offset " +
									 std::to_string(modelOffset) +
									 " is not owned by any body group.");
		}

		return key;
	}

	TOwnedItemKey<ModelV14> NightfireModelFileReader::BodyGroupReferencingModel(size_t index) const
	{
		if ( index >= ArraySize(m_ModelFile->Header.modelOffset) )
		{
			throw std::runtime_error("Cannot find bodygroup referencing model: model at index " +
									 std::to_string(index) +
									 " does not exist.");
		}

		if ( m_ModelFile->Header.modelOffset[index] == 0 )
		{
			throw std::runtime_error("Cannot find bodygroup referencing model: model at index " +
									 std::to_string(index) +
									 " has invalid offset.");
		}

		return BodyGroupReferencingModelAtOffset(m_ModelFile->Header.modelOffset[index]);
	}

	size_t NightfireModelFileReader::ModelIndexForOffset(uint32_t offset) const
	{
		for ( size_t index = 0; index < ArraySize(m_ModelFile->Header.modelOffset); ++index )
		{
			if ( m_ModelFile->Header.modelOffset[index] == offset )
			{
				return index;
			}
		}

		throw std::runtime_error("No model present at offset " + std::to_string(offset));
	}

	void NightfireModelFileReader::LogElementsToRead(const CountOffsetPair& cop, const std::string& elementName, size_t elementSize)
	{
		std::cout
			<< "Reading "
			<< cop.count
			<< " "
			<< elementName
			<< " elements at "
			<< elementSize
			<< " bytes each from offset "
			<< cop.offset
			<< std::endl;
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
