#include "curen_init.hpp"

using namespace Curen;

Curen::CurenInit::CurenInit()
{
	loadModels();
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
		drawFrame();
	}
	vkDeviceWaitIdle(m_curenDevice.device());
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
	m_commandBuffers.resize(m_curenSwapChain.imageCount());

	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.pNext = NULL;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = m_curenDevice.getCommandPool();
	allocInfo.commandBufferCount = static_cast<uint32_t>(m_commandBuffers.size());

	if(vkAllocateCommandBuffers(m_curenDevice.device(), &allocInfo, m_commandBuffers.data()) != VK_SUCCESS) {
		throw std::runtime_error("Failed to allocate command buffer.");
	}

	for (int i = 0; i < m_commandBuffers.size(); i++)
	{
		VkCommandBufferBeginInfo beginInfo{};

		beginInfo.flags = 0;
		beginInfo.pNext = NULL;
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		if (vkBeginCommandBuffer(m_commandBuffers.at(i), &beginInfo) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to begin command buffer.");
		}

		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.pNext = NULL;
		renderPassInfo.renderPass = m_curenSwapChain.getRenderPass();
		renderPassInfo.framebuffer = m_curenSwapChain.getFrameBuffer(i);

		renderPassInfo.renderArea.offset = { 0, 0};
		renderPassInfo.renderArea.extent = m_curenSwapChain.getSwapChainExtent();

		std::array<VkClearValue, 2> clearValues{};
		clearValues[0].color = { 0.1f, 0.1f, 0.1f, 1.0f };
		clearValues[1].depthStencil = { 1.0f, 0 };
		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(m_commandBuffers.at(i), &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		m_curenPipeline->bind(m_commandBuffers.at(i));
		m_curenModel->bind(m_commandBuffers.at(i));
		m_curenModel->draw(m_commandBuffers.at(i));
		vkCmdEndRenderPass(m_commandBuffers.at(i));
		if (vkEndCommandBuffer(m_commandBuffers.at(i)) != VK_SUCCESS) {
			throw std::runtime_error("failed to record command buffer!");
		}
	}
}

void Curen::CurenInit::drawFrame()
{
	uint32_t imageIndex;
	auto result = m_curenSwapChain.acquireNextImage(&imageIndex);
	if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
		throw std::runtime_error("failed to acquire swap chain image");
	}

	result = m_curenSwapChain.submitCommandBuffers(&m_commandBuffers[imageIndex], &imageIndex);
	if (result != VK_SUCCESS) {
		throw std::runtime_error("failed to present swap chain image!");
	}
}

void Curen::CurenInit::loadModels()
{
	std::vector<CurenModel::Vertex> vertices {
		{{0.0f, -0.5f}},
		{ {0.5f, 0.5f} },
		{ {-0.5f, 0.5f} }
	};
	m_curenModel = std::make_unique<CurenModel>(m_curenDevice, vertices);
}

