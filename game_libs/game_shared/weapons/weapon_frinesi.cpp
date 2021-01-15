#include "weapon_frinesi.h"
#include "weaponinfo.h"
#include "skill.h"
#include "gamerules.h"
#include "weapon_pref_weights.h"
#include "weapon_frinesi_atts.h"

#ifndef CLIENT_DLL
#include "bot.h"
#endif

enum FrinesiReloadState_e
{
	RELOAD_IDLE = 0,
	RELOAD_LOAD_SHELL,
	RELOAD_INCREMENT_CLIP
};

static constexpr int RELOAD_MASK = 0x3;
static constexpr int RELOAD_FLAG_INTERRUPTED = (1 << 2);
static constexpr int RELOAD_FLAG_LOADED_ONCE = (1 << 3);
static constexpr int NextReloadState(int orig, int next)
{
	return (orig & ~(RELOAD_MASK)) | next;
}

static constexpr const char* FRINESI_COCK_SOUND = "weapons/weapon_frinesi/frinesi_cock.wav";
static constexpr float FRINESI_PUMP_DELAY = 0.42f;
static constexpr float PUMP_SOUND_OFFSET = 0.08f;
static constexpr float RELOAD_SOUND_OFFSET = 0.1f;

LINK_ENTITY_TO_CLASS(weapon_frinesi, CWeaponFrinesi)

#ifdef AFTERBURNER_GAMEPLAY_PLACEHOLDERS
// To make weapons less sparse for testing, map some other known weapons to this one.
LINK_ENTITY_TO_CLASS(weapon_commando, CWeaponFrinesi)
LINK_ENTITY_TO_CLASS(weapon_minigun, CWeaponFrinesi)
#endif

CWeaponFrinesi::CWeaponFrinesi()
	: CGenericHitscanWeapon(),
	  m_flReloadStartDuration(0.0f),
	  m_flReloadDuration(0.0f),
	  m_flPumpDuration(0.0f),
	  m_flNextPumpTime(0.0f),
	  m_flNextReloadSoundTime(0.0f)
{
	m_pPrimaryAttackMode = GetAttackModeFromAttributes<WeaponAtts::WAHitscanAttack>(ATTACKMODE_AUTO);
	m_pSecondaryAttackMode = GetAttackModeFromAttributes<WeaponAtts::WAHitscanAttack>(ATTACKMODE_PUMP);
}

void CWeaponFrinesi::Precache()
{
	CGenericHitscanWeapon::Precache();
	PRECACHE_SOUND(FRINESI_COCK_SOUND);

	// Cache the durations for our reload animations, so we can use them later.
	m_flReloadStartDuration = ViewModelAnimationDuration(FRINESI_START_RELOAD);
	m_flReloadDuration = ViewModelAnimationDuration(FRINESI_RELOAD);
	m_flPumpDuration = ViewModelAnimationDuration(FRINESI_PUMP);
}

void CWeaponFrinesi::PrimaryAttack()
{
	if ( FlagReloadInterrupt() )
	{
		return;
	}

	InvokeWithAttackMode(WeaponAttackType::Primary, m_pPrimaryAttackMode);
}

void CWeaponFrinesi::SecondaryAttack()
{
	if ( FlagReloadInterrupt() )
	{
		return;
	}

	if ( InvokeWithAttackMode(WeaponAttackType::Secondary, m_pSecondaryAttackMode) )
	{
		m_flNextPumpTime = gpGlobals->time + FRINESI_PUMP_DELAY;
	}
}

void CWeaponFrinesi::Holster(int skipLocal)
{
	CGenericHitscanWeapon::Holster(skipLocal);
	DelayPendingActions(0.1f, true);
}

void CWeaponFrinesi::WeaponTick()
{
	// If we're reloading and have already flagged an interrupt, clear the input buttons.
	// This is to prevent the player holding down attack buttons and preventing
	// the weapon going into an idle state.
	if ( (m_fInSpecialReload & RELOAD_MASK) && (m_fInSpecialReload & RELOAD_FLAG_INTERRUPTED) )
	{
		m_pPlayer->pev->button &= ~(IN_ATTACK | IN_ATTACK2);
	}

	if ( m_flNextPumpTime != 0.0f && m_flNextPumpTime < gpGlobals->time )
	{
		PlayPumpSound();
		m_flNextPumpTime = 0.0f;
	}

	if ( m_flNextReloadSoundTime != 0.0f && m_flNextReloadSoundTime < gpGlobals->time )
	{
		PlaySound(WeaponAttributes().ViewModel.ReloadSounds, CHAN_ITEM);
		m_flNextReloadSoundTime = 0.0f;
	}
}

bool CWeaponFrinesi::FlagReloadInterrupt()
{
	// If we're reloading, allow an interruption.
	if ( (m_fInSpecialReload & RELOAD_MASK) != RELOAD_IDLE )
	{
		m_fInSpecialReload |= RELOAD_FLAG_INTERRUPTED;
		return true;
	}

	return false;
};

int CWeaponFrinesi::HandleSpecialReload(int currentState)
{
	switch (currentState & RELOAD_MASK)
	{
		case RELOAD_IDLE:
		{
			m_flNextReloadSoundTime = 0.0f;
			m_flNextPumpTime = 0.0f;
			SendWeaponAnim(FRINESI_START_RELOAD);

			// Set both our next firing times to be now.
			// This allows the player to flag a reload interrupt by firing
			// at any point from now on. It'll only be honoured at the
			// appropriate time, though.
			DelayFiring(0, true);

			// Go into load shell state after intro animation has finished.
			SetNextIdleTime(m_flReloadStartDuration, true);
			return NextReloadState(currentState, RELOAD_LOAD_SHELL);
		}

		case RELOAD_LOAD_SHELL:
		{
			// If we haven't finished whatever animation is currently playing, don't change state.
			if( m_flTimeWeaponIdle > UTIL_WeaponTimeBase() )
			{
				return NextReloadState(currentState, RELOAD_LOAD_SHELL);
			}

			if ( ((currentState & RELOAD_FLAG_INTERRUPTED) && (currentState & RELOAD_FLAG_LOADED_ONCE)) ||
				 m_iClip >= WeaponAttributes().Ammo.MaxClip ||
				 m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] < 1 )
			{
				// Reloading has finished. Do a pump and delay any further activity until it's finished.
				m_flNextReloadSoundTime = 0.0f;
				m_flNextPumpTime = gpGlobals->time + PUMP_SOUND_OFFSET;
				SendWeaponAnim(FRINESI_PUMP);
				DelayPendingActions(m_flPumpDuration, true);

				return NextReloadState(0, RELOAD_IDLE);
			}

			m_flNextReloadSoundTime = gpGlobals->time + RELOAD_SOUND_OFFSET;
			SendWeaponAnim(FRINESI_RELOAD);

			// Go into the increment clip state once this animation has finished.
			SetNextIdleTime(m_flReloadDuration, true);
			return NextReloadState(currentState, RELOAD_INCREMENT_CLIP);
		}

		case RELOAD_INCREMENT_CLIP:
		{
			if ( m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] > 0 )
			{
				++m_iClip;
				--m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType];
			}

			return NextReloadState(currentState, RELOAD_LOAD_SHELL | RELOAD_FLAG_LOADED_ONCE);
		}

		default:
		{
			return NextReloadState(0, RELOAD_IDLE);
		}
	};
}

void CWeaponFrinesi::PlayPumpSound()
{
	EMIT_SOUND_DYN(ENT(m_pPlayer->pev),
					   CHAN_ITEM,
					   FRINESI_COCK_SOUND,
					   1.0f,
					   ATTN_NORM,
					   0,
					   UTIL_SharedRandomLong(m_pPlayer->random_seed, 98, 101));
}

bool CWeaponFrinesi::ReadPredictionData(const weapon_data_t* from)
{
	if ( !CGenericHitscanWeapon::ReadPredictionData(from) )
	{
		return false;
	}

	m_flNextPumpTime = from->fuser1;
	return true;
}

bool CWeaponFrinesi::WritePredictionData(weapon_data_t* to)
{
	if ( !CGenericHitscanWeapon::WritePredictionData(to) )
	{
		return false;
	}

	to->fuser1 = m_flNextPumpTime;
	return true;
}

const WeaponAtts::WACollection& CWeaponFrinesi::WeaponAttributes() const
{
	return WeaponAtts::StaticWeaponAttributes<CWeaponFrinesi>();
}

#ifndef CLIENT_DLL
TYPEDESCRIPTION	CWeaponFrinesi::m_SaveData[] =
{
	DEFINE_FIELD(CWeaponFrinesi, m_flNextPumpTime, FIELD_FLOAT)
};

IMPLEMENT_SAVERESTORE(CWeaponFrinesi, CGenericHitscanWeapon)

float CWeaponFrinesi::Bot_CalcDesireToUse(CBaseBot& bot, CBaseEntity& enemy, float distanceToEnemy) const
{
	return static_cast<float>(WeaponAttributes().Core.SwitchWeight) / static_cast<float>(WeaponPref_Max);
}

void CWeaponFrinesi::Bot_SetFightStyle(CBaseBotFightStyle& fightStyle) const
{
	static constexpr float SECONDARY_FIRE_PROXIMITY = 700.0f;

	fightStyle.RandomizeAimAtHead(60);
	int chanceOfSecondaryFire = 20;

	// If we're able to determine that the enemy is near enough,
	// increase the chance of using secondary fire.

	CBaseBot* bot = fightStyle.GetOwner();
	CBaseEntity* enemy = bot->GetEnemy();

	if ( enemy && (enemy->pev->origin - bot->pev->origin).Length() <= SECONDARY_FIRE_PROXIMITY )
	{
		chanceOfSecondaryFire = 90;
	}

	fightStyle.RandomizeSecondaryFire(chanceOfSecondaryFire);
	fightStyle.SetNextShootTime(fightStyle.GetSecondaryFire() ? m_flNextSecondaryAttack : m_flNextPrimaryAttack, 0.0f, 1.0f);
}
#endif

namespace WeaponAtts
{
	template<>
	const WACollection& StaticWeaponAttributes<CWeaponFrinesi>()
	{
		return ::StaticWeaponAttributes;
	}
}

class CAmmoFrinesi : public CGenericAmmo
{
public:
	CAmmoFrinesi()
		: CGenericAmmo("models/weapon_frinesi/w_ammo_shotgun.mdl", Ammo_Frinesi)
	{
	}
};

LINK_ENTITY_TO_CLASS(ammo_frinesi, CAmmoFrinesi)
LINK_ENTITY_TO_CLASS(ammo_shotgun, CAmmoFrinesi)	// For Nightfire compatibility

#ifdef AFTERBURNER_GAMEPLAY_PLACEHOLDERS
// To make weapons less sparse for testing, map some other known ammo to this one.
LINK_ENTITY_TO_CLASS(ammo_commando, CAmmoFrinesi)
LINK_ENTITY_TO_CLASS(ammo_mini, CAmmoFrinesi)
#endif
