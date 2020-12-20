#pragma once

#include <cstdint>

namespace NFMDL
{
	class BaseElementUserData
	{
	public:
		inline uint32_t FileOffset() const
		{
			return m_FileOffset;
		}

		inline void SetFileOffset(uint32_t offset)
		{
			m_FileOffset = offset;
		}

	private:
		uint32_t m_FileOffset = 0;
	};
}
