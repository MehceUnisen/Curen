#pragma once

#include "curen_device.hpp"

#include <string>
#include <vector>

namespace Curen {

	struct PipelineConfigInfo {

	};

	class CurenPipeline {
	public:
		CurenPipeline(CurenDevice &device,
			const std::string& vertFilePath, const std::string& fragFilePath, const PipelineConfigInfo& configInfo);
		~CurenPipeline() {}

		CurenPipeline(const CurenPipeline&) = delete;
		void operator = (const CurenPipeline&) = delete;

		static PipelineConfigInfo defPipelineConfigInfo(uint32_t width, uint32_t height);

	private:
		static std::vector<char> readFile(const std::string& filepath);
		void createGraphicsPipeline(const std::string& vertFilePath, const std::string& fragFilePath, const PipelineConfigInfo& configInfo);
		void createShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule);


		CurenDevice& m_curenDevice;
		VkPipeline m_graphicsPipeline;
		VkShaderModule m_vertShader;
		VkShaderModule m_fragShader;
	};
}