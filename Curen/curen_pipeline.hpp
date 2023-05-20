#pragma once

#include "curen_device.hpp"
#include "curen_model.hpp"

#include <string>
#include <vector>

namespace Curen {

	struct PipelineConfigInfo {
		VkPipelineViewportStateCreateInfo viewportInfo;
		VkPipelineColorBlendStateCreateInfo colorBlendInfo;
		VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
		VkPipelineRasterizationStateCreateInfo rasterizationInfo;
		VkPipelineMultisampleStateCreateInfo multisampleInfo;
		VkPipelineColorBlendAttachmentState colorBlendAttachment;
		VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
		std::vector<VkDynamicState> dynamicStateEnables;
		VkPipelineDynamicStateCreateInfo dynamicStateInfo;
		VkPipelineLayout pipelineLayout = nullptr;
		VkRenderPass renderPass = nullptr;
		uint32_t subpass = 0;
	};

	class CurenPipeline {
	public:
		CurenPipeline(CurenDevice &device,
			const std::string& vertFilePath, const std::string& fragFilePath, const PipelineConfigInfo& configInfo);
		~CurenPipeline();

		CurenPipeline(const CurenPipeline&) = delete;
		CurenPipeline& operator = (const CurenPipeline&) = delete;

		void bind(VkCommandBuffer commandBuffer);
		static PipelineConfigInfo defPipelineConfigInfo(PipelineConfigInfo& pipelineConfigInfo);

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