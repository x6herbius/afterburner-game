#pragma once

#include <cstdint>
#include "ElementTraits.h"

namespace NFMDL
{
#pragma pack(push, 1)
	struct Event
	{
		int32_t triggerFrame;
		int32_t eventID;
		int32_t type;
		char options[64];
	};
#pragma pack(pop)

	template<>
	struct ElementTraits<Event>
	{
		static constexpr const char* ELEMENT_NAME = "Event";
	};
}
