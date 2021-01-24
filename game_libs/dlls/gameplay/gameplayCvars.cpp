#include "gameplay/gameplayCvars.h"
#include "extdll.h"
#include "enginecallback.h"

cvar_t mp_weaponspread_maxmovementvel = { "mp_weaponspread_maxmovementvel", "280", FCVAR_SERVER };
cvar_t mp_weaponspread_movementbias = { "mp_weaponspread_movementbias", "1", FCVAR_SERVER };
cvar_t mp_weaponspread_maxangvel = { "mp_weaponspread_maxangvel", "720", FCVAR_SERVER };
cvar_t mp_weaponspread_angvelbias = { "mp_weaponspread_angvelbias", "1", FCVAR_SERVER };

void RegisterGameplayCvars()
{
	CVAR_REGISTER(&mp_weaponspread_maxmovementvel);
	CVAR_REGISTER(&mp_weaponspread_movementbias);
	CVAR_REGISTER(&mp_weaponspread_maxangvel);
	CVAR_REGISTER(&mp_weaponspread_angvelbias);
}
