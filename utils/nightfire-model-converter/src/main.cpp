#include <iostream>
#include <string>
#include <vector>
#include <sstream>

#include "thirdparty/cxxopts.hpp"
#include "AppOptions.h"
#include "appmodel/NightfireModelFile.h"
#include "appmodel/NightfireModelFileReader.h"
#include "elements/DumpElements.h"
#include "Utils.h"
#include "DumpMDLFileItems.h"

static constexpr const char* const ARG_HELP = "help";

static constexpr const char* const ARG_INPUT = "input";
static constexpr const char* const ARG_OUTPUT = "output";
static constexpr const char* const ARG_HEADER_ONLY = "header-only";

struct DumpOption
{
	const char* argName;
	bool AppOptions::* appOption;

	inline const std::string FullArgName() const
	{
		return std::string("dump-") + argName;
	}
};

static const DumpOption DumpOptionsList[] =
{
	{ "header", &AppOptions::dumpHeader },
	{ "bones", &AppOptions::dumpBones },
	{ "bone-controllers", &AppOptions::dumpBoneControllers },
	{ "sequences", &AppOptions::dumpSequences },
	{ "sequence-groups", &AppOptions::dumpSequenceGroups },
	{ "hitboxes", &AppOptions::dumpHitBoxes },
	{ "textures", &AppOptions::dumpTextures },
	{ "attachments", &AppOptions::dumpAttachments },
	{ "sound-groups", &AppOptions::dumpSoundGroups },
	{ "levels-of-detail", &AppOptions::dumpLevelsOfDetail },
	{ "body-groups", &AppOptions::dumpBodyGroups },
	{ "triangle-maps", &AppOptions::dumpTriangleMaps },
	{ "vertices", &AppOptions::dumpVertices },
};

bool ParseCommandLineOptions(int argc, const char** argv, AppOptions& options)
{
	cxxopts::Options launchOptions("nightfire-model-converter", "Converts a Nightfire v14 MDL file to an Afterburner-compatible file.");

	launchOptions.add_options()
		(ARG_HELP, "Print help")
		(ARG_INPUT, "Input file", cxxopts::value<std::string>())
		(ARG_OUTPUT, "Output file", cxxopts::value<std::string>())
		(ARG_HEADER_ONLY, "Read header only - ignore file contents")
	;

	cxxopts::OptionAdder dumpOptionsAdder = launchOptions.add_options("Element dumping");

	for ( uint32_t index = 0; index < ArraySize(DumpOptionsList); ++index )
	{
		const DumpOption& dumpOpt = DumpOptionsList[index];

		std::string itemName(dumpOpt.argName);
		ReplaceAllSubstrings(itemName, "-", " ");

		dumpOptionsAdder(dumpOpt.FullArgName(),
						 std::string("Dump information about MDL file ") + dumpOpt.argName + std::string(" to stdout"));
	}

	// Booooo, cxxopts developers...
	char** badMutableArguments = const_cast<char**>(argv);

	try
	{
		cxxopts::ParseResult result = launchOptions.parse(argc, badMutableArguments);

		if ( result.count(ARG_HELP) || result.count(ARG_INPUT) < 1 )
		{
			std::cerr << launchOptions.help() << std::endl;
			return false;
		}

		options.inputFile = result[ARG_INPUT].as<std::string>();

		if ( result.count(ARG_OUTPUT) > 0 )
		{
			options.outputFile = result[ARG_OUTPUT].as<std::string>();
		}

		options.readHeaderOnly = result[ARG_HEADER_ONLY].as<bool>();

		for ( uint32_t index = 0; index < ArraySize(DumpOptionsList); ++index )
		{
			const DumpOption& dumpOpt = DumpOptionsList[index];

			options.*(dumpOpt.appOption) = result[dumpOpt.FullArgName()].as<bool>();
		}
	}
	catch ( const cxxopts::OptionException& ex )
	{
		std::cerr << "Error parsing options. " << ex.what() << std::endl;
		return false;
	}

	return true;
}

static bool ConvertFile(const AppOptions& options)
{
	std::cout << "Reading " << options.inputFile << std::endl;

	std::shared_ptr<NFMDL::NightfireModelFile> modelFile = std::make_shared<NFMDL::NightfireModelFile>();
	NFMDL::NightfireModelFileReader reader(modelFile);

	try
	{
		reader.SetReadHeaderOnly(options.readHeaderOnly);
		reader.ReadFromFile(options.inputFile);
	}
	catch ( const std::exception& ex )
	{
		std::cerr << "Failed to read input file " << options.inputFile << " Error: " << ex.what() << std::endl;
		return false;
	}

	std::cout << "Read successfully." << std::endl;

	DumpMDLFileItems(options, *modelFile);
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
