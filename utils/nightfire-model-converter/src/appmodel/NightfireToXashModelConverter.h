#pragma once

#include <memory>
#include <string>
#include "NightfireModelFile.h"
#include "XashModelFile.h"

namespace NFMDL
{
	class NightfireToXashModelConverter
	{
	public:
		std::string GetConversionError() const;

		void SetInputFile(const std::shared_ptr<const NightfireModelFile>& inFile);
		void SetOutputFile(const std::shared_ptr<XashModelFile>& outFile);
		bool Convert();

	private:
		void ConvertInternal();
		void ConstructHLBodyGroups();
		void ConstructBodyGroup(uint32_t inIndex, const BodyGroup& in, BodyGroup& out);
		void ConstructModel(uint32_t inBodyGroupIndex, uint32_t inModelIndex, const AugmentedModelV14& in, ModelV10Xash& out);

		std::shared_ptr<const NightfireModelFile> m_InModelFile;
		std::shared_ptr<XashModelFile> m_OutModelFile;
		std::string m_ConversionError;
	};
}
