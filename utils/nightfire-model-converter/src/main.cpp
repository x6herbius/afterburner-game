#include <iostream>
#include <string>
#include <vector>
#include <sstream>

#include "thirdparty/cxxopts.hpp"
#include "AppOptions.h"
#include "appmodel/NightfireModelFile.h"
#include "appmodel/NightfireModelFileReader.h"
#include "elements/DumpElements.h"

static constexpr const char* const ARG_HELP = "help";

static constexpr const char* const ARG_INPUT = "input";
static constexpr const char* const ARG_OUTPUT = "output";
static constexpr const char* const ARG_DUMP = "dump";

bool ParseCommandLineOptions(int argc, const char** argv, AppOptions& options)
{
	cxxopts::Options launchOptions("nightfire-model-converter", "Converts a Nightfire v14 MDL file to an Afterburner-compatible file.");

	launchOptions.add_options()
		(ARG_HELP, "Print help")
		(ARG_INPUT, "Input file", cxxopts::value<std::string>())
		(ARG_OUTPUT, "Output file", cxxopts::value<std::string>())
		(ARG_DUMP, "Dump information about MDL file elements to stdout")
	;

	// Booooo, cxxopts developers...
	char** badMutableArguments = const_cast<char**>(argv);

	try
	{
		cxxopts::ParseResult result = launchOptions.parse(argc, badMutableArguments);

		if ( result.count(ARG_HELP) || result.count(ARG_INPUT) < 1 || result.count(ARG_OUTPUT) < 1 )
		{
			std::cerr << launchOptions.help() << std::endl;
			return false;
		}

		options.inputFile = result[ARG_INPUT].as<std::string>();
		options.outputFile = result[ARG_OUTPUT].as<std::string>();
		options.dumpElementInfo = result[ARG_DUMP].as<bool>();
	}
	catch ( const cxxopts::OptionException& ex )
	{
		std::cerr << "Error parsing options. " << ex.what() << std::endl;
		return false;
	}

	return true;
}

static void DumpInfo(const std::string& filePath, const NFMDL::NightfireModelFile& modelFile)
{
	std::stringstream stream;
	stream << modelFile.Header;

	std::cout << "Dump of " << filePath << ":" << std::endl;
	std::cout << stream.str() << std::endl;
}

static bool ConvertFile(const AppOptions& options)
{
	std::cout << "Converting " << options.inputFile << " to " << options.outputFile << std::endl;

	std::shared_ptr<NFMDL::NightfireModelFile> modelFile = std::make_shared<NFMDL::NightfireModelFile>();
	NFMDL::NightfireModelFileReader reader(modelFile);

	try
	{
		reader.ReadFromFile(options.inputFile);
	}
	catch ( const std::exception& ex )
	{
		std::cerr << "Failed to read input file " << options.inputFile << " Error: " << ex.what() << std::endl;
		return false;
	}

	if ( options.dumpElementInfo )
	{
		DumpInfo(options.inputFile, *modelFile);
	}

	return true;
}

int main(int argc, const char** argv)
{
	AppOptions options;

	try
	{
		if ( !ParseCommandLineOptions(argc, argv, options) || !ConvertFile(options) )
		{
			return 1;
		}
	}
	catch ( const std::exception& ex )
	{
		std::cerr << "An error occurred during the conversion process. " << ex.what() << std::endl;
		return 1;
	}
	catch ( ... )
	{
		std::cerr << "An unhandled error occurred during execution." << std::endl;
		return 1;
	}

	return 0;
}
