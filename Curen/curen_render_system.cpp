#include "curen_render_system.hpp"

using namespace Curen;

struct SimplePushConstant {
	glm::mat2 transform{1.0f};
	glm::vec2 offset;
	alignas(16) glm::vec3 color;
};

CurenRenderSystem::CurenRenderSystem(CurenDevice& device, VkRenderPass renderPass) :
	m_curenDevice {device}
{
	createPipelineLayout();
	createPipeline(renderPass);
}

CurenRenderSystem::~CurenRenderSystem()
{
	vkDestroyPipelineLayout(m_curenDevice.device(), m_pipelineLayout, nullptr);
}

void CurenRenderSystem::createPipelineLayout()
{

	VkPushConstantRange pushConstantRange{};
	pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
	pushConstantRange.offset = 0;
	pushConstantRange.size = sizeof(SimplePushConstant);

	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 0;
	pipelineLayoutInfo.pSetLayouts = nullptr;
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



void CurenRenderSystem::renderObjects(VkCommandBuffer commandBuffer, std::vector<CurenObject>& curenObjects)
{
	m_curenPipeline->bind(commandBuffer);

	for (auto& obj : curenObjects)
	{
		obj.transform2DComponent.rotation = glm::mod(obj.transform2DComponent.rotation + 0.05f, glm::two_pi<float>());
		SimplePushConstant push{};
		push.offset = obj.transform2DComponent.translation;
		push.color = obj.color;
		push.transform = obj.transform2DComponent.mat2();

		vkCmdPushConstants(commandBuffer, m_pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(SimplePushConstant), &push);
		obj.model->bind(commandBuffer);
		obj.model->draw(commandBuffer);
	}
}
