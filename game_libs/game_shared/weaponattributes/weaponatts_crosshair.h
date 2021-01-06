#pragma once

#include <cstring>
#include "standard_includes.h"
#include "weaponatts_base.h"

namespace WeaponAtts
{
	// Attributes describing crosshair behaviour.
	// - Radius is in the interval [0 1] from the centre of the screen,
	//   where 1 is half the length of the shortest dimension of the screen.
	struct WACrosshair : public WABase
	{
		bool hasCrosshair = true;

		// Inner radius: how far the innermost parts of the crosshair are
		// from the centre of the screen. 0 means a solid "+" shape.
		float innerRadius_Standing = 0.1f;

		// Outer radius: how far the outermost parts of the crosshair are
		// from the centre of the screen.
		float outerRadius_Standing = 0.2f;
	};
}
