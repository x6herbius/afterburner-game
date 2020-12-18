#pragma once

#include "ModelV14.h"

namespace NFMDL
{
	class AugmentedModelV14 : public ModelV14
	{
	public:
		AugmentedModelV14();
		AugmentedModelV14(const ModelV14& pod);
		AugmentedModelV14& operator =(const ModelV14& pod);

		uint32_t FileOffset() const;
		void SetFileOffset(uint32_t offset);

	private:
		uint32_t m_FileOffset = 0;
	};
}
