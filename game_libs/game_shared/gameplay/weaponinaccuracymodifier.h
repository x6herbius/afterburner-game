#pragma once

#include <functional>

class CWeaponInaccuracyModifier
{
public:
	using FollowCoefficientInterpFunction = std::function<float(float)>;

	CWeaponInaccuracyModifier();

	// Resets all properties and internal values.
	void Reset();

	// Resets inaccuracy and accumulated time, but leaves other properties unaffected.
	void ResetInaccuracy();

	// Returns the current inaccuracy value, as determined by the last call to RecalculateInaccuracy().
	float CurrentInaccuracy() const;

	// Recalculates the inaccuracy value based on the true value for this frame,
	// and the amount of time that has passed since the last call.
	// The true value should be calculated based on the player's movement and
	// rotation speed. The computed inaccuracy value will track this value across
	// frames, based on the standard follow coefficient, and a weapon being fired
	// will apply an impulse that temporarily increases the inaccuracy.
	void RecalculateInaccuracy(float trueInaccuracy, float currentTime);

	// The maximum value that the inaccuracy is allowed to reach.
	// If it exceeds this value, it will be clamped.
	float InaccuracyCap() const;
	void SetInaccuracyCap(float cap);

	// When inaccuracy is recalculated, the true inaccuracy value is scaled by this
	// amount before it is used. The higher the value, the more the input affects
	// the calculated inaccuracy.
	float InputInaccuracyScale() const;
	void SetInputInaccuracyScale(float scale);

	// The absolute value added to the inaccuracy on the frame when a weapon is fired.
	float WeaponFireImpulse() const;
	void SetWeaponFireImpulse(float impulse);

	// If set, the weapon is considered to have been fired when RecalculateInaccuracy()
	// is next called.
	bool WeaponFiredThisFrame() const;
	void SetWeaponFiredThisFrame();

	// Follow coefficient that is used if the weapon has recently been fired.
	// This should be less than the standard coefficient, as if the weapon has recently
	// been fired, the calculated accuracy curve should remain above the true curve
	// for a short period of time.
	float WeaponFollowCoefficient() const;
	void SetWeaponFollowCoefficient(float coeff);

	// Follow coefficient that is used if the weapon has not recently been fired.
	// This should be closer to 1 than the weapon follow coefficient, as the longer
	// the time since the weapon last fired, the more closely the calculated
	// inaccuracy curve should track the true curve.
	float StandardFollowCoefficient() const;
	void SetStandardFollowCoefficient(float coeff);

	// Function that returns a value in the interval [0 1] based on how long it has
	// been since the weapon was fired. The argument to the function is the time in
	// seconds since the weapon was fired, and will always be >= 0.
	// If 1 is returned, the weapon follow coefficient is used to track the true
	// inaccuracy curve; if 0 is returned, the standard follow coefficient is used.
	// Any value in-between implies a linear interpolation between these two coefficients.
	// If this callback is left blank, the default interp function ramps linearly from
	// 1 to 0 over a period of 1s after the weapon is fired.
	void SetFollowCoefficientInterpFunction(const FollowCoefficientInterpFunction& func);
	void ClearFollowCoefficientInterpFunction();

private:
	float DefaultFollowCoefficientInterp(float timeSinceWeaponFired);

	float m_Inaccuracy = 0.0f;
	float m_InaccuracyCap = 1.0f;
	float m_WeaponFireImpulse = 0.0f;
	bool m_WeaponFiredThisFrame = false;
	float m_WeaponFireTime = 0.0f;
	float m_WeaponFollowCoefficient = 0.5f;
	float m_StandardFollowCoefficient = 0.95f;
	float m_InputInaccuracyScale = 1.0f;
	FollowCoefficientInterpFunction m_FollowCoeffInterpFunc;
};
