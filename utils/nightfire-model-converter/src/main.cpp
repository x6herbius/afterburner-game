#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <fstream>

#include "thirdparty/cxxopts.hpp"
#include "AppOptions.h"
#include "appmodel/NightfireModelFile.h"
#include "appmodel/NightfireModelFileReader.h"
#include "appmodel/XashModelFile.h"
#include "appmodel/XashModelFileWriter.h"
#include "appmodel/XashModelFileHeaderReader.h"
#include "appmodel/NightfireToXashModelConverter.h"
#include "appmodel/FileIdentifier.h"
#include "elements/DumpElements.h"
#include "elements/HeaderV10Xash.h"
#include "Utils.h"
#include "DumpMDLFileItems.h"

using NFModelPtr = std::shared_ptr<NFMDL::NightfireModelFile>;
using XashModelPtr = std::shared_ptr<NFMDL::XashModelFile>;

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
	{ "sounds", &AppOptions::dumpSounds },
	{ "levels-of-detail", &AppOptions::dumpLevelsOfDetail },
	{ "body-groups", &AppOptions::dumpBodyGroups },
	{ "triangle-maps", &AppOptions::dumpTriangleMaps },
	{ "vertices", &AppOptions::dumpVertices },
	{ "normals", &AppOptions::dumpNormals },
	{ "texture-coords", &AppOptions::dumpTextureCoOrdinates },
	{ "vertex-blends", &AppOptions::dumpVertexBlends },
	{ "vertex-blend-scales", &AppOptions::dumpVertexBlendScales },
	{ "bone-fixups", &AppOptions::dumpBoneFixUps },
	{ "skins", &AppOptions::dumpSkins },
	{ "models", &AppOptions::dumpModels },
	{ "model-infos", &AppOptions::dumpModelInfos },
	{ "meshes", &AppOptions::dumpMeshes },
	{ "animation-data", &AppOptions::dumpAnimationData },
	{ "events", &AppOptions::dumpEvents },
	{ "foot-pivots", &AppOptions::dumpFootPivots },
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
						 std::string("Dump information about MDL file ") + itemName + std::string(" to stdout"));
	}

	try
	{
		// Booooo, cxxopts developers...
		char** badMutableArguments = const_cast<char**>(argv);
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

static NFModelPtr ReadNightfireModelFile(const AppOptions& options)
{
	std::cout << "Reading " << options.inputFile << std::endl;

	NFModelPtr inModelFile = std::make_shared<NFMDL::NightfireModelFile>();

	try
	{
		NFMDL::NightfireModelFileReader reader(inModelFile);
		reader.SetReadHeaderOnly(options.readHeaderOnly);
		reader.ReadFromFile(options.inputFile);

		std::cout << "Read successfully." << std::endl;
	}
	catch ( const std::exception& ex )
	{
		// TODO: re-throw this as an exception?
		std::cerr << "Failed to read input file " << options.inputFile << " Error: " << ex.what() << std::endl;
		inModelFile.reset();
	}

	return inModelFile;
}

static bool ConvertAndWriteModelFile(const AppOptions& options, const NFModelPtr& inModelFile)
{
	XashModelPtr outModelFile = std::make_shared<NFMDL::XashModelFile>();

	NFMDL::NightfireToXashModelConverter converter;
	converter.SetInputFile(inModelFile);
	converter.SetOutputFile(outModelFile);

	std::cout << "Converting input file..." << std::endl;

	if ( !converter.Convert() )
	{
		// TODO: Have converter throw exceptions instead?
		std::cerr << "Failed to convert Nightfire model file to Xash model file. " << converter.GetConversionError() << std::endl;
		return false;
	}

	NFMDL::XashModelFileWriter writer(outModelFile);

	std::shared_ptr<std::ofstream> outFile = std::make_shared<std::ofstream>(options.outputFile);
	writer.SetOutputStream(outFile);

	std::cout << "Writing: " << options.outputFile << std::endl;
	writer.Write();
	std::cout << "Writing complete." << std::endl;

	return true;
}

static void ReadXashHeader(const AppOptions& options)
{
	std::cout << "Input file was a Xash model file, dumping header for information." << std::endl;

	NFMDL::HeaderV10Xash xashHeader{};
	NFMDL::XashModelFileHeaderReader reader(xashHeader);
	reader.ReadFromFile(options.inputFile);

	std::cout << xashHeader << std::endl;
}

int main(int argc, const char** argv)
{
	AppOptions options;

	try
	{
		if ( !ParseCommandLineOptions(argc, argv, options) )
		{
			return 1;
		}

		NFMDL::FileIdentifier::Type fileType = NFMDL::FileIdentifier::IdentifyFile(options.inputFile);

		if ( fileType == NFMDL::FileIdentifier::Type::Xash )
		{
			ReadXashHeader(options);
			return 0;
		}

		std::shared_ptr<NFMDL::NightfireModelFile> inModelFile = ReadNightfireModelFile(options);

		if ( !inModelFile )
		{
			return 1;
		}

		DumpMDLFileItems(options, *inModelFile);

		if ( !ConvertAndWriteModelFile(options, inModelFile) )
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
