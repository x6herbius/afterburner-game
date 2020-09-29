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

		std::shared_ptr<const NightfireModelFile> m_InModelFile;
		std::shared_ptr<XashModelFile> m_OutModelFile;
		std::string m_ConversionError;
	};
}
