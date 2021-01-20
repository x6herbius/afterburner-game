#include "playerviewanglevelocity.h"

extern "C"
{
#include "mathlib.h"
}

// TODO: This is icky. It'd be nice to have something that didn't use #ifdefs.
#ifdef CLIENT_DLL
#include "cl_dll.h"
#include "hud.h"
#include "cl_util.h"
#else
#include "standard_includes.h"
#define AngleVectors UTIL_MakeVectorsPrivate
#endif

namespace
{
	inline float AngleBetween(const Vector& a, const Vector& b)
	{
		float l1 = a.Length();
		float l2 = b.Length();

		if ( !l1 || !l2 )
		{
			return 0.0f;
		}

		float angle = acos(DotProduct(a, b) / (l1 * l2));
		angle = (angle * 180.0f) / M_PI;

		return angle;
	}
}

float CPlayerViewAngleVelocity::ViewAngleVelocity() const
{
	return m_ViewAngleDelta;
}

void CPlayerViewAngleVelocity::Update(const Vector& viewAngles, float timestamp)
{
	if ( timestamp <= m_LastTimestamp )
	{
		return;
	}

	if ( viewAngles == m_LastViewAngles )
	{
		m_ViewAngleDelta = 0.0f;
		return;
	}

	Vector currentFwd;
	Vector dummyRight;
	Vector dummyUp;
	AngleVectors(viewAngles, currentFwd, dummyRight, dummyUp);

	m_ViewAngleDelta = AngleBetween(currentFwd, m_LastViewAnglesAsVec) / (timestamp - m_LastTimestamp);

	m_LastViewAngles = viewAngles;
	m_LastViewAnglesAsVec = currentFwd;
	m_LastTimestamp = timestamp;
}

void CPlayerViewAngleVelocity::Reset(const Vector& viewAngles, float baseTime)
{
	Vector dummyRight;
	Vector dummyUp;
	AngleVectors(viewAngles, m_LastViewAnglesAsVec, dummyRight, dummyUp);

	m_LastViewAngles = viewAngles;
	m_ViewAngleDelta = 0.0f;
	m_LastTimestamp = baseTime;
}
