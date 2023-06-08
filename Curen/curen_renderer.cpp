#include "curen_renderer.hpp"

using namespace Curen;

CurenRenderer::CurenRenderer(CurenWindow& curenWindow, CurenDevice& curenDevice) :
	m_curenWindow{ curenWindow }, m_curenDevice{ curenDevice }
{
	recreateSwapChain();
	createCommandBuffers();
}

CurenRenderer::~CurenRenderer()
{
	freeCommandBuffers();
}

VkCommandBuffer CurenRenderer::beginFrame()
{
	assert(!m_isFrameStarted && "Can't call beginFrame() while frame is in progress");

	auto result = m_curenSwapChain->acquireNextImage(&m_currentImageIndex);

	if (result == VK_ERROR_OUT_OF_DATE_KHR)
	{
		recreateSwapChain();
		return nullptr;
	}

	if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
		throw std::runtime_error("failed to acquire swap chain image");
	}
	m_isFrameStarted = true;

	VkCommandBuffer commandBuffer = getCurrentCommandBuffer();

	VkCommandBufferBeginInfo beginInfo{};

	beginInfo.flags = 0;
	beginInfo.pNext = NULL;
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

	if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to begin command buffer.");
	}

	return commandBuffer;
}

void CurenRenderer::endFrame()
{
	assert(m_isFrameStarted && "Can't call endFrame() when frame is in progress");
	VkCommandBuffer commandBuffer = getCurrentCommandBuffer();

	if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
		throw std::runtime_error("failed to record command buffer!");
	}
	auto result = m_curenSwapChain->submitCommandBuffers(&commandBuffer, &m_currentImageIndex);
	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || m_curenWindow.wasWindowResized())
	{
		m_curenWindow.resetWindowResizedFlag();
		recreateSwapChain();
	}
	else if (result != VK_SUCCESS) {
		throw std::runtime_error("failed to present swap chain image!");
	}

	m_isFrameStarted = false;
	m_currentFrameIndex = (m_currentFrameIndex + 1) % CurenSwapChain::MAX_FRAMES_IN_FLIGHT;
}

void CurenRenderer::beginSwapChainRenderPass(VkCommandBuffer commandBuffer)
{
	assert(m_isFrameStarted && "Can't call beginSwapChainRenderPass() if frame is not in progress");
	assert((commandBuffer = getCurrentCommandBuffer()) && "Can't begin render pass on command buffer from a different frame");
	
	VkCommandBufferBeginInfo beginInfo{};

	beginInfo.flags = 0;
	beginInfo.pNext = NULL;
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

	if (vkBeginCommandBuffer(m_commandBuffers.at(m_currentFrameIndex), &beginInfo) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to begin command buffer.");
	}

	VkRenderPassBeginInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.pNext = NULL;
	renderPassInfo.renderPass = m_curenSwapChain->getRenderPass();
	renderPassInfo.framebuffer = m_curenSwapChain->getFrameBuffer(m_currentImageIndex);

	renderPassInfo.renderArea.offset = { 0, 0 };
	renderPassInfo.renderArea.extent = m_curenSwapChain->getSwapChainExtent();

	std::array<VkClearValue, 2> clearValues{};
	clearValues[0].color = { 0.1f, 0.1f, 0.1f, 1.0f };
	clearValues[1].depthStencil = { 1.0f, 0 };
	renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
	renderPassInfo.pClearValues = clearValues.data();

	vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

	VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = static_cast<float>(m_curenSwapChain->getSwapChainExtent().width);
	viewport.height = static_cast<float>(m_curenSwapChain->getSwapChainExtent().height);
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	VkRect2D scissor{ {0, 0}, m_curenSwapChain->getSwapChainExtent() };
	vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
	vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

}

void CurenRenderer::endSwapChainRenderPass(VkCommandBuffer commandBuffer)
{
	assert(m_isFrameStarted && "Can't call endSwapChainRenderPass() if frame is not in progress");
	assert((commandBuffer = getCurrentCommandBuffer()) && "Can't end render pass on command buffer from a different frame");
	
	vkCmdEndRenderPass(commandBuffer);
}

void CurenRenderer::createCommandBuffers()
{
	m_commandBuffers.resize(CurenSwapChain::MAX_FRAMES_IN_FLIGHT);

	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.pNext = NULL;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = m_curenDevice.getCommandPool();
	allocInfo.commandBufferCount = static_cast<uint32_t>(m_commandBuffers.size());

	if (vkAllocateCommandBuffers(m_curenDevice.device(), &allocInfo, m_commandBuffers.data()) != VK_SUCCESS) {
		throw std::runtime_error("Failed to allocate command buffer.");
	}


}

void CurenRenderer::freeCommandBuffers()
{
	vkFreeCommandBuffers(
		m_curenDevice.device(),
		m_curenDevice.getCommandPool(),
		static_cast<uint32_t>(m_commandBuffers.size()),
		m_commandBuffers.data());
	
	m_commandBuffers.clear();
}


void CurenRenderer::recreateSwapChain()
{
	auto extent = m_curenWindow.getExtent();
	while (extent.height == 0 || extent.width == 0) {
		extent = m_curenWindow.getExtent();
		glfwPollEvents();
	}

	vkDeviceWaitIdle(m_curenDevice.device());
	if (m_curenSwapChain == nullptr) {
		m_curenSwapChain = std::make_unique<CurenSwapChain>(m_curenDevice, extent);
	}
	else {
		std::shared_ptr<CurenSwapChain> oldSwapChain = std::move(m_curenSwapChain);
		m_curenSwapChain = std::make_unique<CurenSwapChain>(m_curenDevice, extent, oldSwapChain);
		
		if (!oldSwapChain->compareSwapChainFormats(*m_curenSwapChain.get()))
		{
			throw std::runtime_error("Swap chain image or depth format has changed");
		}
		
	}
	//createPipeline();
}
