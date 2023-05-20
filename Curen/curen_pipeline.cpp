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

CurenPipeline::~CurenPipeline() {
	vkDestroyShaderModule(m_curenDevice.device(), m_vertShader, nullptr);
	vkDestroyShaderModule(m_curenDevice.device(), m_fragShader, nullptr);
	vkDestroyPipeline(m_curenDevice.device(), m_graphicsPipeline, nullptr);
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

	createShaderModule(vertCode, &m_vertShader);
	createShaderModule(fragCode, &m_fragShader);

	VkPipelineShaderStageCreateInfo shaderStages[2];
	shaderStages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	shaderStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
	shaderStages[0].module = m_vertShader;
	shaderStages[0].pName = "main";
	shaderStages[0].flags = 0;
	shaderStages[0].pNext = nullptr;
	shaderStages[0].pSpecializationInfo = nullptr;
	shaderStages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	shaderStages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	shaderStages[1].module = m_fragShader;
	shaderStages[1].pName = "main";
	shaderStages[1].flags = 0;
	shaderStages[1].pNext = nullptr;
	shaderStages[1].pSpecializationInfo = nullptr;

	auto bindingDescriptionsInfo = CurenModel::Vertex::getBindingDescriptions();
	auto attributeDescriptionsInfo = CurenModel::Vertex::getAttributeDescriptions();


	VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptionsInfo.size());
	vertexInputInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(bindingDescriptionsInfo.size());
	vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptionsInfo.data();
	vertexInputInfo.pVertexBindingDescriptions = bindingDescriptionsInfo.data();
	vertexInputInfo.flags = 0;
	vertexInputInfo.pNext = NULL;

	//VkPipelineColorBlendStateCreateInfo colorBlendInfo{};
	//colorBlendInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	//colorBlendInfo.logicOpEnable = VK_FALSE;
	//colorBlendInfo.logicOp = VK_LOGIC_OP_COPY;  // Optional
	//colorBlendInfo.attachmentCount = 1;
	//colorBlendInfo.flags = 0;
	//colorBlendInfo.pNext = NULL;
	//colorBlendInfo.pAttachments = &configInfo.colorBlendAttachment;
	//colorBlendInfo.blendConstants[0] = 0.0f;  // Optional
	//colorBlendInfo.blendConstants[1] = 0.0f;  // Optional
	//colorBlendInfo.blendConstants[2] = 0.0f;  // Optional
	//colorBlendInfo.blendConstants[3] = 0.0f;  // Optional

	VkGraphicsPipelineCreateInfo pipelineInfo{};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.stageCount = 2;
	pipelineInfo.pStages = shaderStages;
	pipelineInfo.pVertexInputState = &vertexInputInfo;
	pipelineInfo.pInputAssemblyState = &configInfo.inputAssemblyInfo;
	pipelineInfo.pViewportState = &configInfo.viewportInfo;
	pipelineInfo.pRasterizationState = &configInfo.rasterizationInfo;
	pipelineInfo.pMultisampleState = &configInfo.multisampleInfo;
	pipelineInfo.pColorBlendState = &configInfo.colorBlendInfo;
	pipelineInfo.pDepthStencilState = &configInfo.depthStencilInfo;
	pipelineInfo.pDynamicState = &configInfo.dynamicStateInfo;
	pipelineInfo.flags = 0;

	pipelineInfo.layout = configInfo.pipelineLayout;
	pipelineInfo.renderPass = configInfo.renderPass;
	pipelineInfo.subpass = configInfo.subpass;

	pipelineInfo.basePipelineIndex = -1;
	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

	if (vkCreateGraphicsPipelines(
		m_curenDevice.device(),
		VK_NULL_HANDLE,
		1,
		&pipelineInfo,
		nullptr,
		&m_graphicsPipeline) != VK_SUCCESS) {
		throw std::runtime_error("failed to create graphics pipeline");
	}
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

PipelineConfigInfo CurenPipeline::defPipelineConfigInfo(PipelineConfigInfo& pipelineConfigInfo) {
	

	pipelineConfigInfo.inputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	pipelineConfigInfo.inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	pipelineConfigInfo.inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;
	pipelineConfigInfo.inputAssemblyInfo.flags = 0;
	pipelineConfigInfo.inputAssemblyInfo.pNext = NULL;
	
	pipelineConfigInfo.viewportInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	pipelineConfigInfo.viewportInfo.flags = 0;
	pipelineConfigInfo.viewportInfo.pNext = NULL;
	pipelineConfigInfo.viewportInfo.pScissors = nullptr;
	pipelineConfigInfo.viewportInfo.pViewports = nullptr;
	pipelineConfigInfo.viewportInfo.scissorCount = 1;
	pipelineConfigInfo.viewportInfo.viewportCount = 1;

	pipelineConfigInfo.colorBlendInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	pipelineConfigInfo.colorBlendInfo.logicOpEnable = VK_FALSE;
	pipelineConfigInfo.colorBlendInfo.logicOp = VK_LOGIC_OP_COPY;  // Optional
	pipelineConfigInfo.colorBlendInfo.attachmentCount = 1;
	pipelineConfigInfo.colorBlendInfo.flags = 0;
	pipelineConfigInfo.colorBlendInfo.pNext = NULL;
	pipelineConfigInfo.colorBlendInfo.pAttachments = &pipelineConfigInfo.colorBlendAttachment;
	pipelineConfigInfo.colorBlendInfo.blendConstants[0] = 0.0f;  // Optional
	pipelineConfigInfo.colorBlendInfo.blendConstants[1] = 0.0f;  // Optional
	pipelineConfigInfo.colorBlendInfo.blendConstants[2] = 0.0f;  // Optional
	pipelineConfigInfo.colorBlendInfo.blendConstants[3] = 0.0f;  // Optional

	pipelineConfigInfo.rasterizationInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	pipelineConfigInfo.rasterizationInfo.depthClampEnable = VK_FALSE;
	pipelineConfigInfo.rasterizationInfo.rasterizerDiscardEnable = VK_FALSE;
	pipelineConfigInfo.rasterizationInfo.polygonMode = VK_POLYGON_MODE_FILL;
	pipelineConfigInfo.rasterizationInfo.lineWidth = 1.0f;
	pipelineConfigInfo.rasterizationInfo.cullMode = VK_CULL_MODE_NONE;
	pipelineConfigInfo.rasterizationInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
	pipelineConfigInfo.rasterizationInfo.depthBiasEnable = VK_FALSE;
	pipelineConfigInfo.rasterizationInfo.flags = 0;
	pipelineConfigInfo.rasterizationInfo.pNext = NULL;
	pipelineConfigInfo.rasterizationInfo.depthBiasConstantFactor = 0.0f;  // Optional
	pipelineConfigInfo.rasterizationInfo.depthBiasClamp = 0.0f;           // Optional
	pipelineConfigInfo.rasterizationInfo.depthBiasSlopeFactor = 0.0f;     // Optional

	pipelineConfigInfo.multisampleInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	pipelineConfigInfo.multisampleInfo.sampleShadingEnable = VK_FALSE;
	pipelineConfigInfo.multisampleInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	pipelineConfigInfo.multisampleInfo.flags = 0;
	pipelineConfigInfo.multisampleInfo.pNext = NULL;
	pipelineConfigInfo.multisampleInfo.minSampleShading = 1.0f;           // Optional
	pipelineConfigInfo.multisampleInfo.pSampleMask = nullptr;             // Optional
	pipelineConfigInfo.multisampleInfo.alphaToCoverageEnable = VK_FALSE;  // Optional
	pipelineConfigInfo.multisampleInfo.alphaToOneEnable = VK_FALSE;       // Optional

	pipelineConfigInfo.colorBlendAttachment.colorWriteMask =
		VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT |
		VK_COLOR_COMPONENT_A_BIT;
	pipelineConfigInfo.colorBlendAttachment.blendEnable = VK_FALSE;
	pipelineConfigInfo.colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;   // Optional
	pipelineConfigInfo.colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;  // Optional
	pipelineConfigInfo.colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;              // Optional
	pipelineConfigInfo.colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;   // Optional
	pipelineConfigInfo.colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;  // Optional
	pipelineConfigInfo.colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;              // Optional
	
	pipelineConfigInfo.depthStencilInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	pipelineConfigInfo.depthStencilInfo.depthTestEnable = VK_TRUE;
	pipelineConfigInfo.depthStencilInfo.depthWriteEnable = VK_TRUE;
	pipelineConfigInfo.depthStencilInfo.depthCompareOp = VK_COMPARE_OP_LESS;
	pipelineConfigInfo.depthStencilInfo.depthBoundsTestEnable = VK_FALSE;
	pipelineConfigInfo.depthStencilInfo.flags = 0;
	pipelineConfigInfo.depthStencilInfo.pNext = NULL;
	pipelineConfigInfo.depthStencilInfo.minDepthBounds = 0.0f;  // Optional
	pipelineConfigInfo.depthStencilInfo.maxDepthBounds = 1.0f;  // Optional
	pipelineConfigInfo.depthStencilInfo.stencilTestEnable = VK_FALSE;
	pipelineConfigInfo.depthStencilInfo.front = {};  // Optional
	pipelineConfigInfo.depthStencilInfo.back = {};   // Optional

	pipelineConfigInfo.dynamicStateEnables = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
	pipelineConfigInfo.dynamicStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	pipelineConfigInfo.dynamicStateInfo.pDynamicStates = pipelineConfigInfo.dynamicStateEnables.data();
	pipelineConfigInfo.dynamicStateInfo.dynamicStateCount = static_cast<uint32_t>(pipelineConfigInfo.dynamicStateEnables.size());
	pipelineConfigInfo.dynamicStateInfo.flags = 0;
	
	return pipelineConfigInfo;
}

void Curen::CurenPipeline::bind(VkCommandBuffer commandBuffer)
{
	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_graphicsPipeline);
}
