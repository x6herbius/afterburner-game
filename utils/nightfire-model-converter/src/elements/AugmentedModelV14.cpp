#include "AugmentedModelV14.h"

namespace NFMDL
{
	AugmentedModelV14::AugmentedModelV14() :
		ModelV14{}
	{
	}

	AugmentedModelV14::AugmentedModelV14(const ModelV14& pod) :
		ModelV14(pod)
	{
	}

	AugmentedModelV14& AugmentedModelV14::operator =(const ModelV14& pod)
	{
		*static_cast<ModelV14*>(this) = pod;
		return *this;
	}

	uint32_t AugmentedModelV14::FileOffset() const
	{
		return m_FileOffset;
	}

	void AugmentedModelV14::SetFileOffset(uint32_t offset)
	{
		m_FileOffset = offset;
	}
}
