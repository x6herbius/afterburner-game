#pragma once

#include <string>

struct AppOptions
{
	std::string inputFile;
	std::string outputFile;
	bool dumpElementInfo = false;
};
