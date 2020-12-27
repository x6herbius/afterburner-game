#include <cstring>
#include <cassert>
#include <stdexcept>
#include <iostream>
#include "NightfireToXashModelConverter.h"
#include "Utils.h"
#include "elements/Conversions.h"
#include "elements/ModelV14.h"
#include "elements/ModelV10Xash.h"

namespace NFMDL
{
	template<typename T, typename U, typename K>
	static inline void CopyContainer(const ElementContainer<T, U, K>& src,
									 ElementContainer<T, U, K>& dest,
									 const char* elementName)
	{
		if ( elementName && *elementName )
		{
			std::cout << "Copying " << elementName << " (" << src.Count() << " elements)" << std::endl;
		}

		src.Clone(dest);
	}

	// Note that this does not copy user data or keys.
	template<typename T1, typename U1, typename K1, typename T2, typename U2, typename K2>
	static inline void CopyContainerElements(const ElementContainer<T1, U1, K1>& src,
											 ElementContainer<T2, U2, K2>& dest,
											 const char* elementName)
	{
		using SourceContainerType = ElementContainer<T1, U1, K1>;
		using DestContainerType = ElementContainer<T2, U2, K2>;

		if ( elementName && *elementName )
		{
			std::cout << "Copying and converting " << elementName << " (" << src.Count() << " elements)" << std::endl;
		}

		dest.AllocateDefault(src.Count());

		for ( const DestContainerType::IteratorData& it : dest )
		{
			const SourceContainerType::ElementType* source = src.ElementAt(it.index);
			assert(source);

			Convert(*source, *it.element);
		}
	}

	// Note that this does not copy user data or keys.
	template<typename T, typename U1, typename K1, typename U2, typename K2>
	static inline void CopyContainerElements(const ElementContainer<T, U1, K1>& src,
											 ElementContainer<T, U2, K2>& dest,
											 const char* elementName)
	{
		using SourceContainerType = ElementContainer<T, U1, K1>;
		using DestContainerType = ElementContainer<T, U2, K2>;

		if ( elementName && *elementName )
		{
			std::cout << "Copying " << elementName << " (" << src.Count() << " elements)" << std::endl;
		}

		dest.AllocateDefault(src.Count());

		for ( const DestContainerType::IteratorData& it : dest )
		{
			const SourceContainerType::ElementType* source = src.ElementAt(it.index);
			assert(source);

			*it.element = *source;
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

	bool NightfireToXashModelConverter::Verbose() const
	{
		return m_Verbose;
	}

	void NightfireToXashModelConverter::SetVerbose(bool verbose)
	{
		m_Verbose = verbose;
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
			success = true;
		}
		catch ( const std::exception& ex )
		{
			m_ConversionError = ex.what();
		}

		return success;
	}

	void NightfireToXashModelConverter::ConvertInternal()
	{
		if ( m_Verbose )
		{
			std::cout << "Converting header" << std::endl;
		}

		NFMDL::Convert(m_InModelFile->Header, m_OutModelFile->Header);

		CopyContainer(m_InModelFile->Bones, m_OutModelFile->Bones, m_Verbose ? "bones" : "");
		CopyContainer(m_InModelFile->BoneControllers, m_OutModelFile->BoneControllers, m_Verbose ? "bone controllers" : "");
		CopyContainer(m_InModelFile->HitBoxes, m_OutModelFile->HitBoxes, m_Verbose ? "hitboxes" : "");
		CopyContainerElements(m_InModelFile->Sequences, m_OutModelFile->Sequences, m_Verbose ? "sequences" : "");
		CopyContainer(m_InModelFile->SequenceGroups, m_OutModelFile->SequenceGroups, m_Verbose ? "sequence groups" : "");
		CopyContainer(m_InModelFile->Attachments, m_OutModelFile->Attachments, m_Verbose ? "attachments" : "");

		CopyContainer(m_InModelFile->Skins, m_OutModelFile->Skins, m_Verbose ? "skins" : "");
		CopyContainer(m_InModelFile->Events, m_OutModelFile->Events, m_Verbose ? "events" : "");
		CopyContainer(m_InModelFile->FootPivots, m_OutModelFile->FootPivots, m_Verbose ? "foot pivots" : "");
		CopyContainer(m_InModelFile->AnimationData, m_OutModelFile->AnimationData, m_Verbose ? "animation data" : "");

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

		CopyContainerElements(m_InModelFile->BodyGroups, m_OutModelFile->BodyGroups, m_Verbose ? "body groups" : "");

		for ( auto it : m_OutModelFile->BodyGroups )
		{
			// Zero out model offsets for now.
			it.element->modelOffset = 0;
		}

		ConstructModelsFromBodyGroups();
	}

	void NightfireToXashModelConverter::ConstructModelsFromBodyGroups()
	{
		for ( auto it : m_OutModelFile->BodyGroups )
		{
			const size_t outModelIndexBase = m_OutModelFile->Models.Count();

			const BodyGroupUserDataV14* inUserData = m_InModelFile->BodyGroups.UserDataAt(it.index);
			it.userData->SetChildModelsBeginIndex(outModelIndexBase);
			it.userData->SetChildModelsCount(inUserData->ChildModelsCount());

			if ( m_Verbose )
			{
				std::cout
					<< "Allocating "
					<< it.userData->ChildModelsCount()
					<< " models for body group "
					<< it.index
					<< std::endl;
			}

			// Create one ModelV10Xash per ModelV14.
			for ( size_t modelIndex = 0;
				  modelIndex < it.userData->ChildModelsCount();
				  ++modelIndex )
			{
				const size_t outModelIndex = m_OutModelFile->Models.Count();
				m_OutModelFile->Models.AppendDefault();

				TOwnedItemKey<ModelV10Xash> outModelKey;
				outModelKey.ownerIndex = it.index;
				outModelKey.itemIndex = outModelIndex - outModelIndexBase;

				m_OutModelFile->Models.AssignMapping(outModelKey, outModelIndex);

				ConstructMeshesFromModel(outModelKey);
			}
		}
	}

	void NightfireToXashModelConverter::ConstructMeshesFromModel(const TOwnedItemKey<ModelV10Xash>& modelKey)
	{
		TOwnedItemKey<ModelV14> inModelKey;
		inModelKey.ownerIndex = modelKey.ownerIndex;
		inModelKey.itemIndex = modelKey.itemIndex;

		const ModelUserDataV14* inModelUserData = m_InModelFile->Models.UserDataAt(inModelKey);
		assert(inModelUserData);

		const size_t outMeshBaseIndex = m_OutModelFile->Meshes.Count();

		ModelUserDataV10Xash* outModelUserData = m_OutModelFile->Models.UserDataAt(modelKey);
		assert(outModelUserData);

		outModelUserData->SetChildMeshesBeginIndex(outMeshBaseIndex);
		outModelUserData->SetChildMeshesCount(inModelUserData->ChildModelInfosCount());

		if ( m_Verbose )
			{
				std::cout
					<< "Allocating "
					<< outModelUserData->ChildMeshesCount()
					<< " meshes for body group "
					<< modelKey.ownerIndex
					<< ", model "
					<< modelKey.itemIndex
					<< std::endl;
			}


		// Create one MeshV10Xash per ModelInfoV14
		for ( size_t modelInfoIndex = 0;
			  modelInfoIndex < outModelUserData->ChildMeshesCount();
			  ++modelInfoIndex )
		{
			const size_t outMeshIndex = m_OutModelFile->Meshes.Count();
			m_OutModelFile->Meshes.AppendDefault();

			MeshCollectionKeyV10Xash outMeshKey;
			outMeshKey.bodyGroupIndex = modelKey.ownerIndex;
			outMeshKey.modelIndex = modelKey.itemIndex;
			outMeshKey.meshIndex = outMeshIndex - outMeshBaseIndex;

			m_OutModelFile->Meshes.AssignMapping(outMeshKey, outMeshIndex);
		}
	}
}
