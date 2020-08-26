#pragma once

#include <string>

struct AppOptions
{
	std::string inputFile;
	std::string outputFile;
	bool readHeaderOnly = false;

	bool dumpHeader = false;
	bool dumpBones = false;
	bool dumpBoneControllers = false;
	bool dumpSequences = false;
	bool dumpSequenceGroups = false;
	bool dumpHitBoxes = false;
	bool dumpTextures = false;
	bool dumpAttachments = false;
	bool dumpSoundGroups = false;
	bool dumpLevelsOfDetail = false;
	bool dumpBodyGroups = false;
	bool dumpTriangleMaps = false;
	bool dumpVertices = false;
	bool dumpNormals = false;
	bool dumpTextureCoOrdinates = false;
	bool dumpVertexBlendScales = false;
	bool dumpVertexBlends = false;
};
