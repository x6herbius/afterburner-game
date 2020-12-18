#pragma once

#include "ModelInfoV14.h"

namespace NFMDL
{
	class AugmentedModelInfoV14 : public ModelInfoV14
	{
	public:
		AugmentedModelInfoV14();
		AugmentedModelInfoV14(const ModelInfoV14& pod);
		AugmentedModelInfoV14& operator =(const ModelInfoV14& pod);

		uint32_t FileOffset() const;
		void SetFileOffset(uint32_t offset);

	private:
		uint32_t m_FileOffset = 0;
	};
}
