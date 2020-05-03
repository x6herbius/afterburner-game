#pragma once

#include "ItemSoundResources.h"
#include "WeaponSoundResources.h"
#include "SurfaceSoundResources.h"

namespace SoundResources
{
	extern CItemSoundResources ItemSounds;
	extern CWeaponSoundResources WeaponSounds;
	extern CSurfaceSoundResources SurfaceSounds;

	void Initialise();
	void Precache();
}