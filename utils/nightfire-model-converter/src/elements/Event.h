#pragma once

#include <cstdint>

namespace NFMDL
{
#pragma pack(push, 1)
	struct Event
	{
		int32_t frame;
		int32_t event;
		int32_t type;
		char options[64];
	};
#pragma pack(pop)
}
