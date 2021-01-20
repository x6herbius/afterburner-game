#pragma once

#include <cstring>
#include "standard_includes.h"
#include "weaponatts_base.h"

namespace WeaponAtts
{
	struct WACrosshair : public WABase
	{
		// Toggle for this crosshair mode. If false, no crosshair is drawn.
		bool hasCrosshair = true;
	};
}
