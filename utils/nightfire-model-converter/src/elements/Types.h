#pragma once

#include <cstdint>

namespace NFMDL
{
	enum MotionFlag
	{
		Motion_X = (1 << 0),
		Motion_Y = (1 << 1),
		Motion_Z = (1 << 2),
		Motion_XR = (1 << 3),
		Motion_YR = (1 << 4),
		Motion_ZR = (1 << 5),
		Motion_LX = (1 << 6),
		Motion_LY = (1 << 7),
		Motion_LZ = (1 << 8),
		Motion_AX = (1 << 9),
		Motion_AY = (1 << 10),
		Motion_AZ = (1 << 11),
		Motion_AXR = (1 << 12),
		Motion_AYR = (1 << 13),
		Motion_AZR = (1 << 14),
		Motion_Loop = (1 << 15)
	};

	enum SequenceFlag
	{
		Sequence_Looping = (1 << 0)
	};

	// Excludes the loop flag:
	static constexpr uint32_t MOTION_FLAGS_DOF_MASK = 0x7FF;

	static constexpr uint32_t MOUTH_CONTROLLER_INDEX = 4;

#pragma pack(push, 1)
	struct HeaderIdentifier
	{
		int8_t ident[4];

		inline constexpr HeaderIdentifier(int8_t a, int8_t b, int8_t c, int8_t d) :
			ident{a, b, c, d}
		{
		}

		inline constexpr HeaderIdentifier(const HeaderIdentifier& other) :
			ident{other.ident[0], other.ident[1], other.ident[2], other.ident[3]}
		{
		}

		inline constexpr HeaderIdentifier& operator =(const HeaderIdentifier& other)
		{
			ident[0] = other.ident[0];
			ident[1] = other.ident[1];
			ident[2] = other.ident[2];
			ident[3] = other.ident[3];
		}
	};

	struct Vec3F
	{
		float v[3];

		inline float& operator [](int index)
		{
			return v[index];
		}

		inline const float& operator [](int index) const
		{
			return v[index];
		}
	};

	struct CountOffsetPair
	{
		uint32_t offset;
		uint32_t count;
	};

	struct BoundingBox
	{
		Vec3F min;
		Vec3F max;
	};
#pragma pack(pop)

	template<typename T>
	struct FormatTraits
	{
	};

	static constexpr HeaderIdentifier MDL_IDENTIFIER('I', 'D', 'S', 'T');
}
