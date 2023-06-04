#include "curen_render_system.hpp"

using namespace Curen;

struct SimplePushConstant {
	glm::mat4 modelMatrix{1.0f};
	glm::mat4 normalMatrix{1.0f};
};

CurenRenderSystem::CurenRenderSystem(CurenDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout) :
	m_curenDevice {device}
{
	createPipelineLayout(globalSetLayout);
	createPipeline(renderPass);
}

CurenRenderSystem::~CurenRenderSystem()
{
	vkDestroyPipelineLayout(m_curenDevice.device(), m_pipelineLayout, nullptr);
}

void CurenRenderSystem::createPipelineLayout(VkDescriptorSetLayout globalSetLayout)
{

	VkPushConstantRange pushConstantRange{};
	pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
	pushConstantRange.offset = 0;
	pushConstantRange.size = sizeof(SimplePushConstant);

	std::vector<VkDescriptorSetLayout> descriptorSetLayout {globalSetLayout};

	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayout.size());
	pipelineLayoutInfo.pSetLayouts = descriptorSetLayout.data();
	pipelineLayoutInfo.pushConstantRangeCount = 1;
	pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;
	if (vkCreatePipelineLayout(m_curenDevice.device(), &pipelineLayoutInfo, nullptr, &m_pipelineLayout) !=
		VK_SUCCESS) {
		throw std::runtime_error("failed to create pipeline layout!");
	}
}

void CurenRenderSystem::createPipeline(VkRenderPass renderPass)
{
	PipelineConfigInfo pipelineConfigInfo{};
	auto pipelineConfig =
		CurenPipeline::defPipelineConfigInfo(pipelineConfigInfo);
	pipelineConfig.renderPass = renderPass;
	pipelineConfig.pipelineLayout = m_pipelineLayout;
	m_curenPipeline = std::make_unique<CurenPipeline>(
		m_curenDevice,
		"first_shader.vert.spv",
		"first_shader.frag.spv",
		pipelineConfig);
}



void CurenRenderSystem::renderObjects(FrameInfo& frameInfo)
{
	m_curenPipeline->bind(frameInfo.commandBuffer);

	vkCmdBindDescriptorSets(frameInfo.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
		m_pipelineLayout, 0, 1, &frameInfo.globalDescriptorSet, 0, nullptr);

	for (auto& kv : frameInfo.objects)
	{	
		CurenObject& obj = kv.second;
		SimplePushConstant push{};
		push.modelMatrix = obj.transformComponent.mat4();
		push.normalMatrix = obj.transformComponent.normalMatrix();
		
		vkCmdPushConstants(frameInfo.commandBuffer, m_pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(SimplePushConstant), &push);
		obj.model->bind(frameInfo.commandBuffer);
		obj.model->draw(frameInfo.commandBuffer);
	}
}

