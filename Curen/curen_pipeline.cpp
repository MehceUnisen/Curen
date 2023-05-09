#include "curen_pipeline.hpp"

#include <fstream>
#include <stdexcept>
#include <iostream>

using namespace Curen;

CurenPipeline::CurenPipeline(CurenDevice& device,const std::string& vertFilePath, 
	const std::string& fragFilePath, const PipelineConfigInfo& configInfo): m_curenDevice{ device }
{
	createGraphicsPipeline(vertFilePath, fragFilePath, configInfo);
}

std::vector<char> CurenPipeline::readFile(const std::string& filepath) {
	std::ifstream file(filepath, std::ios::ate | std::ios::binary);

	if (!file.is_open())
	{
		throw std::runtime_error("Failed to open the file : " + filepath);
	}

	std::size_t fileSize = static_cast<std::size_t>(file.tellg());
	std::vector<char> buffer(fileSize);

	file.seekg(0);
	file.read(buffer.data(), fileSize);
	file.close();

	return buffer;
}

void CurenPipeline::createGraphicsPipeline(const std::string& vertFilePath, const std::string& fragFilePath, const PipelineConfigInfo& configInfo) {
	std::vector<char> vertCode = readFile(vertFilePath);
	std::vector<char> fragCode = readFile(fragFilePath);

	std::cout << "vert file size = " << vertCode.size() << '\n';
	std::cout << "frag file size = " << fragCode.size() << '\n';
}

void CurenPipeline::createShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule) {
	VkShaderModuleCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = code.size();
	createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

	if (vkCreateShaderModule(m_curenDevice.device(), &createInfo, nullptr, shaderModule) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create shader module.");
	}
}

PipelineConfigInfo CurenPipeline::defPipelineConfigInfo(uint32_t width, uint32_t height) {
	PipelineConfigInfo configInfo;

	return configInfo;
}