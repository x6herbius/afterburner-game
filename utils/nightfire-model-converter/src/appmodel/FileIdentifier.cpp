#include <fstream>
#include "appmodel/FileIdentifier.h"
#include "elements/HeaderV10Xash.h"
#include "elements/HeaderV10XashFormatTraits.h"
#include "elements/HeaderV14.h"
#include "elements/HeaderV14FormatTraits.h"
#include "Utils.h"

namespace NFMDL
{
	namespace FileIdentifier
	{
		Type IdentifyFile(const std::string& fileName)
		{
			std::ifstream inStream(fileName, std::ios_base::in | std::ios_base::binary);

			if ( !inStream.good() )
			{
				return Type::Unknown;
			}

			char ident[4];
			inStream.read(ident, ArraySize(ident));

			if ( !inStream.good() )
			{
				return Unknown;
			}

			for ( size_t index = 0; index < ArraySize(ident); ++index )
			{
				if ( FormatTraits<HeaderV10Xash>::TARGET_IDENTIFIER[index] == '\0' ||
					 ident[index] != FormatTraits<HeaderV10Xash>::TARGET_IDENTIFIER[index] )
				{
					break;
				}

				if ( index == ArraySize(ident) - 1 )
				{
					return Type::Xash;
				}
			}

			for ( size_t index = 0; index < ArraySize(ident); ++index )
			{
				if ( FormatTraits<HeaderV14>::TARGET_IDENTIFIER[index] == '\0' ||
					 ident[index] != FormatTraits<HeaderV14>::TARGET_IDENTIFIER[index] )
				{
					break;
				}

				if ( index == ArraySize(ident) - 1 )
				{
					return Type::Nightfire;
				}
			}

			return Type::Unknown;
		}
	}
}
