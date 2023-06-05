#include "curen_point_light_system.hpp"

using namespace Curen;

CurenPointLightSystem::CurenPointLightSystem(CurenDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout) :
	m_curenDevice {device}
{
	createPipelineLayout(globalSetLayout);
	createPipeline(renderPass);
}

CurenPointLightSystem::~CurenPointLightSystem()
{
	vkDestroyPipelineLayout(m_curenDevice.device(), m_pipelineLayout, nullptr);
}

void CurenPointLightSystem::createPipelineLayout(VkDescriptorSetLayout globalSetLayout)
{

	//VkPushConstantRange pushConstantRange{};
	//pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
	//pushConstantRange.offset = 0;
	//pushConstantRange.size = sizeof(SimplePushConstant);

	std::vector<VkDescriptorSetLayout> descriptorSetLayout {globalSetLayout};

	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayout.size());
	pipelineLayoutInfo.pSetLayouts = descriptorSetLayout.data();
	pipelineLayoutInfo.pushConstantRangeCount = 0;
	pipelineLayoutInfo.pPushConstantRanges = nullptr;
	if (vkCreatePipelineLayout(m_curenDevice.device(), &pipelineLayoutInfo, nullptr, &m_pipelineLayout) !=
		VK_SUCCESS) {
		throw std::runtime_error("failed to create pipeline layout!");
	}
}

void CurenPointLightSystem::createPipeline(VkRenderPass renderPass)
{
	PipelineConfigInfo pipelineConfigInfo{};
	auto pipelineConfig =
		CurenPipeline::defPipelineConfigInfo(pipelineConfigInfo);
	pipelineConfig.renderPass = renderPass;
	pipelineConfig.pipelineLayout = m_pipelineLayout;
	m_curenPipeline = std::make_unique<CurenPipeline>(
		m_curenDevice,
		"point_light.vert.spv",
		"point_light.frag.spv",
		pipelineConfig);
}

void CurenPointLightSystem::render(FrameInfo& frameInfo)
{
	m_curenPipeline->bind(frameInfo.commandBuffer);

	vkCmdBindDescriptorSets(frameInfo.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
		m_pipelineLayout, 0, 1, &frameInfo.globalDescriptorSet, 0, nullptr);
	
	vkCmdDraw(frameInfo.commandBuffer, 6, 1, 0, 0);
}

