#pragma once

#include "vector_classes.h"

class CPlayerViewAngleVelocity
{
public:
	float ViewAngleVelocity() const;
	void Update(const Vector& viewAngles, float timestamp);
	void Reset(const Vector& viewAngles = Vector(), float baseTime = 0.0f);

private:
	float m_ViewAngleDelta = 0.0f;
	Vector m_LastViewAngles;
	Vector m_LastViewAnglesAsVec;
	float m_LastTimestamp = 0.0f;
};
