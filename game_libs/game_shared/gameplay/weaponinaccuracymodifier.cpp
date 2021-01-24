#include "weaponinaccuracymodifier.h"
#include "miniutl.h"

namespace
{
	static inline float Clamp(float min, float val, float max)
	{
		return Max(Min(max, val), min);
	}

	// Clamps in range [0 1].
	static inline float ClampNormalised(float val)
	{
		return Clamp(0.0f, val, 1.0f);
	}
}

CWeaponInaccuracyModifier::CWeaponInaccuracyModifier()
{
	Reset();
}

void CWeaponInaccuracyModifier::Reset()
{
	ResetInaccuracy();

	m_InaccuracyCap = 1.0f;
	m_WeaponFireImpulse = 0.0f;
	m_WeaponFiredThisFrame = false;
	m_WeaponFireTime = 0.0f;
	m_WeaponFollowCoefficient = 0.5f;
	m_StandardFollowCoefficient = 0.95f;
	m_InputInaccuracyScale = 1.0f;
	m_FollowCoeffInterpFunc = FollowCoefficientInterpFunction();
}

void CWeaponInaccuracyModifier::ResetInaccuracy()
{
	m_Inaccuracy = 0.0f;
}

float CWeaponInaccuracyModifier::CurrentInaccuracy() const
{
	return m_Inaccuracy;
}

void CWeaponInaccuracyModifier::RecalculateInaccuracy(float trueInaccuracy, float currentTime)
{
	// Scale input as appropriate.
	trueInaccuracy *= m_InputInaccuracyScale;

	// Add impulse to current inaccuracy if the player's weapon was fired this frame.
	if ( m_WeaponFiredThisFrame )
	{
		m_Inaccuracy += m_WeaponFireImpulse;
		m_WeaponFireTime = currentTime;
		m_WeaponFiredThisFrame = false;
	}

	const float timeSinceWeaponFired = Max(0.0f, currentTime - m_WeaponFireTime);

	// Get interpolation value between both follow coefficients.
	// If interp == 1, the weapon's follow coefficient is used.
	// If interp == 0, the standard follow coefficient is used.
	// If interp is between 0 and 1, different proportions of each
	// coefficient are used.
	const float interp = m_FollowCoeffInterpFunc
		? ClampNormalised(m_FollowCoeffInterpFunc(timeSinceWeaponFired))
		: ClampNormalised(DefaultFollowCoefficientInterp(timeSinceWeaponFired));

	// Calculate the follow coefficient we will use.
	const float followCoefficient = (interp * m_WeaponFollowCoefficient) + ((1 - interp) * m_StandardFollowCoefficient);

	// Move current inaccuracy closer to the true inaccuracy curve.
	// How influential the true inaccuracy is depends on the follow coefficient.
	// If this is 1, the true inaccuracy curve is followed accurately.
	// If it is 0, the true inaccuracy curve has no influence.
	m_Inaccuracy += followCoefficient * (trueInaccuracy - m_Inaccuracy);

	// Clamp inaccuracy to valid range.
	m_Inaccuracy = Clamp(0.0f, m_Inaccuracy, m_InaccuracyCap);
}

float CWeaponInaccuracyModifier::InaccuracyCap() const
{
	return m_InaccuracyCap;
}

void CWeaponInaccuracyModifier::SetInaccuracyCap(float cap)
{
	m_InaccuracyCap = Max(cap, 0.0f);
}

float CWeaponInaccuracyModifier::InputInaccuracyScale() const
{
	return m_InputInaccuracyScale;
}

void CWeaponInaccuracyModifier::SetInputInaccuracyScale(float scale)
{
	m_InputInaccuracyScale = scale;
}

float CWeaponInaccuracyModifier::WeaponFireImpulse() const
{
	return m_WeaponFireImpulse;
}

void CWeaponInaccuracyModifier::SetWeaponFireImpulse(float impulse)
{
	m_WeaponFireImpulse = impulse;
}

bool CWeaponInaccuracyModifier::WeaponFiredThisFrame() const
{
	return m_WeaponFiredThisFrame;
}

void CWeaponInaccuracyModifier::SetWeaponFiredThisFrame()
{
	m_WeaponFiredThisFrame = true;
}

float CWeaponInaccuracyModifier::WeaponFollowCoefficient() const
{
	return m_WeaponFollowCoefficient;
}

void CWeaponInaccuracyModifier::SetWeaponFollowCoefficient(float coeff)
{
	m_WeaponFollowCoefficient = ClampNormalised(coeff);
}

float CWeaponInaccuracyModifier::StandardFollowCoefficient() const
{
	return m_StandardFollowCoefficient;
}

void CWeaponInaccuracyModifier::SetStandardFollowCoefficient(float coeff)
{
	m_StandardFollowCoefficient = ClampNormalised(coeff);
}

void CWeaponInaccuracyModifier::SetFollowCoefficientInterpFunction(const FollowCoefficientInterpFunction& func)
{
	m_FollowCoeffInterpFunc = func;
}

void CWeaponInaccuracyModifier::ClearFollowCoefficientInterpFunction()
{
	m_FollowCoeffInterpFunc = FollowCoefficientInterpFunction();
}

float CWeaponInaccuracyModifier::DefaultFollowCoefficientInterp(float timeSinceWeaponFired)
{
	return 1.0f - timeSinceWeaponFired;
}
