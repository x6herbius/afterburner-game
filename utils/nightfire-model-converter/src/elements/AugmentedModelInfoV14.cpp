#include "AugmentedModelInfoV14.h"

namespace NFMDL
{
	AugmentedModelInfoV14::AugmentedModelInfoV14() :
		ModelInfoV14{}
	{
	}

	AugmentedModelInfoV14::AugmentedModelInfoV14(const ModelInfoV14& pod) :
		ModelInfoV14(pod)
	{
	}

	AugmentedModelInfoV14& AugmentedModelInfoV14::operator =(const ModelInfoV14& pod)
	{
		*static_cast<ModelInfoV14*>(this) = pod;
		return *this;
	}

	uint32_t AugmentedModelInfoV14::FileOffset() const
	{
		return m_FileOffset;
	}

	void AugmentedModelInfoV14::SetFileOffset(uint32_t offset)
	{
		m_FileOffset = offset;
	}
}
