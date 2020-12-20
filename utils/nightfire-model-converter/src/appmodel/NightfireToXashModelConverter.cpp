#include <cstring>
#include <cassert>
#include <stdexcept>
#include "NightfireToXashModelConverter.h"
#include "Utils.h"
#include "elements/Conversions.h"
#include "elements/AugmentedModelV14.h"
#include "elements/ModelV10Xash.h"

namespace NFMDL
{
	template<typename T>
	static inline void CopyElementArray(const ElementArray<T>& src, ElementArray<T>& dest)
	{
		dest.AllocateFrom(src.ConstData(), src.Count());
	}

	template<typename TS, typename TD>
	static inline void CopyElementArray(const ElementArray<TS>& src, ElementArray<TD>& dest)
	{
		dest.AllocateDefault(src.Count());

		src.ForEach([&dest](uint32_t index, const TS& element)
		{
			Convert(element, dest[index]);
		});
	}

	template<typename T, typename U, typename K>
	static inline void CopyContainer(const ElementContainer<T, U, K>& src, ElementContainer<T, U, K>& dest)
	{
		src.Clone(dest);
	}

	// Note that this does not copy user data or keys.
	template<typename T1, typename U1, typename K1, typename T2, typename U2, typename K2>
	static inline void CopyContainerElements(const ElementContainer<T1, U1, K1>& src, ElementContainer<T2, U2, K2>& dest)
	{
		using SourceContainerType = ElementContainer<T1, U1, K1>;
		using DestContainerType = ElementContainer<T2, U2, K2>;

		dest.AllocateDefault(src.Count());

		for ( const DestContainerType::IteratorData& it : dest )
		{
			const SourceContainerType::ElementType* source = src.ElementAt(it.index);
			assert(source);

			Convert(*source, *it.element);
		}
	}

	std::string NightfireToXashModelConverter::GetConversionError() const
	{
		return m_ConversionError;
	}

	void NightfireToXashModelConverter::SetInputFile(const std::shared_ptr<const NightfireModelFile>& inFile)
	{
		m_InModelFile = inFile;
	}

	void NightfireToXashModelConverter::SetOutputFile(const std::shared_ptr<XashModelFile>& outFile)
	{
		m_OutModelFile = outFile;
	}

	bool NightfireToXashModelConverter::Convert()
	{
		m_ConversionError.clear();

		if ( !m_InModelFile )
		{
			m_ConversionError = "No input model was provided.";
			return false;
		}

		if ( !m_OutModelFile )
		{
			m_ConversionError = "No output model was provided.";
			return false;
		}

		bool success = false;

		try
		{
			ConvertInternal();
		}
		catch ( const std::exception& ex )
		{
			m_ConversionError = ex.what();
		}

		return success;
	}

	void NightfireToXashModelConverter::ConvertInternal()
	{
		NFMDL::Convert(m_InModelFile->Header, m_OutModelFile->Header);

		CopyContainer(m_InModelFile->Bones, m_OutModelFile->Bones);
		CopyContainer(m_InModelFile->BoneControllers, m_OutModelFile->BoneControllers);
		CopyContainer(m_InModelFile->HitBoxes, m_OutModelFile->HitBoxes);
		CopyContainerElements(m_InModelFile->Sequences, m_OutModelFile->Sequences);
		CopyContainer(m_InModelFile->SequenceGroups, m_OutModelFile->SequenceGroups);
		CopyContainer(m_InModelFile->Attachments, m_OutModelFile->Attachments);

		CopyContainer(m_InModelFile->Skins, m_OutModelFile->Skins);
		CopyContainer(m_InModelFile->Events, m_OutModelFile->Events);
		CopyContainer(m_InModelFile->FootPivots, m_OutModelFile->FootPivots);
		CopyContainer(m_InModelFile->AnimationData, m_OutModelFile->AnimationData);

		ConstructHLBodyGroups();

		// TODO: The rest
	}

	void NightfireToXashModelConverter::ConstructHLBodyGroups()
	{
		/*
			The process of this is as follows:

			Nightfire and Half Life body groups are equivalent. However, for the sub-models of each body group,
			the data layout is a bit different between Nightfire and Half Life.

			Nightfire:
				- Model owns multiple model infos. There is no other useful information in the model element.
				- Model info specifies the skin, and owns multiple meshes.
				- Meshes own triangle mappings and vertices.

			Half Life:
				- mstudiomodel_t owns multiple meshes, and holds info on vertices, normals, and blends.
				- mstudiomesh_t specifies the skin, and owns triangle mappings and normals (apparently unused).

			These can roughly be combined as follows:
				- One mstudiomodel_t should be created for each NF model.
				- One child mstudiomesh_t should be created for each NF model info underneath this model,
				  and should take its skin from that model info.
				- All meshes listed underneath the NF model info should have their triangles accumulated together
				  and recorded in the mstudiomesh_t.
				- All vertices listed in each NF mesh under the NF model info should be accumulated together
				  and recorded in the mstudiomodel_t.
				- All other data required by the mstudiomesh_t should be generated and recorded appropriately.

			We also need to generate the relevant vertex/normal/etc. lists from the Nightfire vertex objects.
		*/

		CopyElementArray(m_InModelFile->BodyGroups, m_OutModelFile->BodyGroups);

		m_OutModelFile->BodyGroups.MutateEach([this](uint32_t index, BodyGroup& bodyGroup)
		{
			ConstructBodyGroup(index, m_InModelFile->BodyGroups[index], bodyGroup);
		});
	}

	void NightfireToXashModelConverter::ConstructBodyGroup(uint32_t inIndex, const BodyGroup& in, BodyGroup& out)
	{
		// Most things should already have been constructed from the input body group,
		// but the model offset should be zeroed for now.
		out.modelOffset = 0;

		// Construct one mstudiomodel_t from each model listed in this input body group.
		for ( uint32_t modelIndex = 0; modelIndex < out.modelCount; ++modelIndex )
		{
			TOwnedItemKey<ModelV10Xash> key;
			key.ownerIndex = inIndex;
			key.itemIndex = modelIndex;

			const uint32_t inModelOffset = in.modelOffset + (modelIndex * sizeof(ModelV14));
			const AugmentedModelV14* inModel = m_InModelFile->Models.FindFirstMatching([inModelOffset](uint32_t index, const AugmentedModelV14& item)
			{
				return item.FileOffset() == inModelOffset;
			});

			if ( !inModel )
			{
				throw std::logic_error("NightfireToXashModelConverter: Could not find Nightfire model at expected offset " +
									   std::to_string(inModelOffset));
			}

			ModelV10Xash& outModel = m_OutModelFile->Models[key];
			ConstructModel(inIndex, modelIndex, *inModel, outModel);
		}
	}

	void NightfireToXashModelConverter::ConstructModel(uint32_t inBodyGroupIndex, uint32_t inModelIndex, const AugmentedModelV14& in, ModelV10Xash& out)
	{
		// Construct one Xash Mesh per Nightfire ModelInfo.
		for ( uint32_t modelInfoIndex = 0; modelInfoIndex < ArraySize(in.modelInfoOffset); ++modelInfoIndex )
		{
			const uint32_t modelInfoOffset = in.modelInfoOffset[modelInfoIndex];

			if ( modelInfoOffset < 1 )
			{
				continue;
			}

			++out.meshes.count;

			TOwnedItemKey<AugmentedModelInfoV14> inModelInfoKey;
			inModelInfoKey.ownerIndex = inModelIndex;
			inModelInfoKey.itemIndex = modelInfoIndex;

			NightfireModelFile::ModelInfoCollection::const_iterator inModelInfoIt = m_InModelFile->ModelInfos.find(inModelInfoKey);

			if ( inModelInfoIt == m_InModelFile->ModelInfos.cend() )
			{
				throw std::logic_error("NightfireToXashModelConverter: Could not find Nightfire model info " +
									   std::to_string(modelInfoIndex) + " for model " + std::to_string(inModelIndex) +
									   " (offset " + std::to_string(modelInfoOffset) + ")");
			}

			MeshCollectionKeyV10Xash outMeshKey;
			outMeshKey.bodyGroupIndex = inBodyGroupIndex;
			outMeshKey.modelIndex = inModelIndex;
			outMeshKey.meshIndex = modelInfoIndex;

			MeshV10Xash& outMesh = m_OutModelFile->Meshes[outMeshKey];
			outMesh.skinref = inModelInfoIt->second.skinReference;

			// TODO: Continue from here.
		}
	}
}
