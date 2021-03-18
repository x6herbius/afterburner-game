#pragma once

#include <string>

struct AppOptions
{
	std::string inputFile;
	std::string outputFile;
	bool readHeaderOnly = false;
	bool verbose = false;

	bool dumpAll = false;
	bool dumpHeader = false;
	bool dumpBones = false;
	bool dumpBoneControllers = false;
	bool dumpSequences = false;
	bool dumpSequenceGroups = false;
	bool dumpHitBoxes = false;
	bool dumpTextures = false;
	bool dumpAttachments = false;
	bool dumpSoundGroups = false;
	bool dumpSounds = false;
	bool dumpLevelsOfDetail = false;
	bool dumpBodyGroups = false;
	bool dumpTriangleMaps = false;
	bool dumpVertices = false;
	bool dumpNormals = false;
	bool dumpTextureCoOrdinates = false;
	bool dumpVertexBlendScales = false;
	bool dumpVertexBlends = false;
	bool dumpBoneFixUps = false;
	bool dumpSkins = false;
	bool dumpModels = false;
	bool dumpModelInfos = false;
	bool dumpMeshes = false;
	bool dumpAnimationData = false;
	bool dumpEvents = false;
	bool dumpFootPivots = false;

	bool dumpingAnyElements = false;
};
