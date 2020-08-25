#pragma once

#include <cstdint>
#include <string>

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

	enum LevelOfDetailFlags
	{
		LOD_None = 0,
		LOD_TimesOne = (1 << 0),
		LOD_TimesTwo = (1 << 1),
		LOD_TimesFive = (1 << 2),
		LOD_PlusOne = (1 << 8),
		LOD_PlusTwo = (1 << 9),
		LOD_PlusFour = (1 << 10)
	};

	// Excludes the loop flag:
	static constexpr uint32_t MOTION_FLAGS_DOF_MASK = 0x7FF;

	static constexpr uint32_t MOUTH_CONTROLLER_INDEX = 4;

#pragma pack(push, 1)
	struct HeaderIdentifier
	{
		int8_t ident[4];

		inline HeaderIdentifier() :
			ident{0, 0, 0, 0}
		{
		}

		inline HeaderIdentifier(int8_t a, int8_t b, int8_t c, int8_t d) :
			ident{a, b, c, d}
		{
		}

		inline HeaderIdentifier(const HeaderIdentifier& other) :
			ident{other.ident[0], other.ident[1], other.ident[2], other.ident[3]}
		{
		}

		inline explicit HeaderIdentifier(const char* string)
		{
			for ( uint32_t index = 0; index < 4; ++index )
			{
				if ( string && *string )
				{
					ident[index] = *string;
					++string;
				}
				else
				{
					ident[index] = 0;
				}
			}
		}

		inline bool operator ==(const HeaderIdentifier& other) const
		{
			return ident[0] == other.ident[0] &&
				   ident[1] == other.ident[1] &&
				   ident[2] == other.ident[2] &&
				   ident[3] == other.ident[3];
		}

		inline bool operator != (const HeaderIdentifier& other) const
		{
			return !(*this == other);
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
		uint32_t count;
		uint32_t offset;
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

	static constexpr const char* const MDL_IDENTIFIER_HALFLIFE = "IDST";
	static constexpr const char* const MDL_IDENTIFIER_NIGHTFIRE = "MDLZ";
}
