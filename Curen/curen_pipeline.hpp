#pragma once

#include <string>
#include <vector>

namespace Curen {
	class CurenPipeline {
	public:
		CurenPipeline(const std::string& vertFilePath, const std::string& fragFilePath);

	private:
		static std::vector<char> readFile(const std::string& filepath);
		void createGraphicsPipeline(const std::string& vertFilePath, const std::string& fragFilePath);
	};
}