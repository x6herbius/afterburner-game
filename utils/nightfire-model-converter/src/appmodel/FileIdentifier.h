#pragma once

#include <string>

namespace NFMDL
{
	namespace FileIdentifier
	{
		enum Type
		{
			Unknown,
			Nightfire,
			Xash
		};

		Type IdentifyFile(const std::string& fileName);
	}
}
