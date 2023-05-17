#include "curen_init.hpp"

using namespace Curen;

Curen::CurenInit::CurenInit()
{
	createPipelineLayout();
	createPipeline();
	createCommandBuffers();
}

Curen::CurenInit::~CurenInit()
{
	vkDestroyPipelineLayout(m_curenDevice.device(), m_pipelineLayout, nullptr);
}

void CurenInit::run() {
	while (!m_curenWindow.shouldClose()) {
		glfwPollEvents();
	}
}


void Curen::CurenInit::createPipelineLayout()
{
	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 0;
	pipelineLayoutInfo.pSetLayouts = nullptr;
	pipelineLayoutInfo.pushConstantRangeCount = 0;
	pipelineLayoutInfo.pPushConstantRanges = nullptr;
	if (vkCreatePipelineLayout(m_curenDevice.device(), &pipelineLayoutInfo, nullptr, &m_pipelineLayout) !=
		VK_SUCCESS) {
		throw std::runtime_error("failed to create pipeline layout!");
	}
}

void Curen::CurenInit::createPipeline()
{
	auto pipelineConfig =
		CurenPipeline::defPipelineConfigInfo(m_curenSwapChain.width(), m_curenSwapChain.height());
	pipelineConfig.renderPass = m_curenSwapChain.getRenderPass();
	pipelineConfig.pipelineLayout = m_pipelineLayout;
	m_curenPipeline = std::make_unique<CurenPipeline>(
		m_curenDevice,
		"first_shader.vert.spv",
		"first_shader.frag.spv",
		pipelineConfig);
}

void Curen::CurenInit::createCommandBuffers()
{
}

void Curen::CurenInit::drawFrame()
{
}
