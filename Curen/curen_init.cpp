 #include "curen_init.hpp"

using namespace Curen;

struct SimplePushConstant {
	glm::mat2 transform{1.0f};
	glm::vec2 offset;
	alignas(16) glm::vec3 color;
};

Curen::CurenInit::CurenInit()
{
	loadObjects();
	createPipelineLayout();
	recreateSwapChain();
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

void Curen::CurenInit::createPipeline()
{
	PipelineConfigInfo pipelineConfigInfo{};
	auto pipelineConfig =
		CurenPipeline::defPipelineConfigInfo(pipelineConfigInfo);
	pipelineConfig.renderPass = m_curenSwapChain->getRenderPass();
	pipelineConfig.pipelineLayout = m_pipelineLayout;
	m_curenPipeline = std::make_unique<CurenPipeline>(
		m_curenDevice,
		"first_shader.vert.spv",
		"first_shader.frag.spv",
		pipelineConfig);
}

void Curen::CurenInit::createCommandBuffers()
{
	m_commandBuffers.resize(m_curenSwapChain->imageCount());

	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.pNext = NULL;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = m_curenDevice.getCommandPool();
	allocInfo.commandBufferCount = static_cast<uint32_t>(m_commandBuffers.size());

	if(vkAllocateCommandBuffers(m_curenDevice.device(), &allocInfo, m_commandBuffers.data()) != VK_SUCCESS) {
		throw std::runtime_error("Failed to allocate command buffer.");
	}

	
}

void Curen::CurenInit::freeCommandBuffers()
{
	vkFreeCommandBuffers(
		m_curenDevice.device(),
		m_curenDevice.getCommandPool(),
		static_cast<uint32_t>(m_commandBuffers.size()),
		m_commandBuffers.data());
	m_commandBuffers.clear();
}

void Curen::CurenInit::drawFrame()
{
	uint32_t imageIndex;

	auto result = m_curenSwapChain->acquireNextImage(&imageIndex);

	if (result == VK_ERROR_OUT_OF_DATE_KHR)
	{
		recreateSwapChain();
		return;
	}

	if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
		throw std::runtime_error("failed to acquire swap chain image");
	}
	recordCommandBuffer(imageIndex);
	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || m_curenWindow.wasWindowResized())
	{
		m_curenWindow.resetWindowResizedFlag();
		recreateSwapChain();
		return;
	}
	result = m_curenSwapChain->submitCommandBuffers(&m_commandBuffers[imageIndex], &imageIndex);
	if (result != VK_SUCCESS) {
		throw std::runtime_error("failed to present swap chain image!");
	}
}

void Curen::CurenInit::loadObjects()
{
	std::vector<CurenModel::Vertex> vertices {
		{{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
		{ {0.5f, 0.5f}, {0.0f, 1.0f, 0.0f} },
		{ {-0.5f, 0.5f} , {0.0f, .0f, 1.0f} }
	};
	auto model = std::make_shared<CurenModel>(m_curenDevice, vertices);
	auto triangle = CurenObject::createObject();
	triangle.model = model;
	triangle.color = { .1f, .8f, .1f };
	//triangle.transform2DComponent.translation.x = .2f;
	triangle.transform2DComponent.scale = { 1.0f, 1.0f };
	triangle.transform2DComponent.rotation = 0.25f * glm::two_pi<float>();
	m_curenObjects.push_back(std::move(triangle));
}

void Curen::CurenInit::recreateSwapChain()
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
		m_curenSwapChain = std::make_unique<CurenSwapChain>(m_curenDevice, extent, std::move(m_curenSwapChain));
		if (m_curenSwapChain->imageCount() != m_commandBuffers.size()) {
			freeCommandBuffers();
			createCommandBuffers();
		}
	}
	createPipeline();
}

void Curen::CurenInit::recordCommandBuffer(int imageIndex)
{
	VkCommandBufferBeginInfo beginInfo{};

	beginInfo.flags = 0;
	beginInfo.pNext = NULL;
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

	if (vkBeginCommandBuffer(m_commandBuffers.at(imageIndex), &beginInfo) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to begin command buffer.");
	}

	VkRenderPassBeginInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.pNext = NULL;
	renderPassInfo.renderPass = m_curenSwapChain->getRenderPass();
	renderPassInfo.framebuffer = m_curenSwapChain->getFrameBuffer(imageIndex);

	renderPassInfo.renderArea.offset = { 0, 0 };
	renderPassInfo.renderArea.extent = m_curenSwapChain->getSwapChainExtent();

	std::array<VkClearValue, 2> clearValues{};
	clearValues[0].color = { 0.1f, 0.1f, 0.1f, 1.0f };
	clearValues[1].depthStencil = { 1.0f, 0 };
	renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
	renderPassInfo.pClearValues = clearValues.data();

	vkCmdBeginRenderPass(m_commandBuffers.at(imageIndex), &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
	
	VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = static_cast<float>(m_curenSwapChain->getSwapChainExtent().width);
	viewport.height = static_cast<float>(m_curenSwapChain->getSwapChainExtent().height);
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	VkRect2D scissor{ {0, 0}, m_curenSwapChain->getSwapChainExtent() };
	vkCmdSetViewport(m_commandBuffers[imageIndex], 0, 1, &viewport);
	vkCmdSetScissor(m_commandBuffers[imageIndex], 0, 1, &scissor);

	renderObjects(m_commandBuffers.at(imageIndex));

	vkCmdEndRenderPass(m_commandBuffers.at(imageIndex));
	if (vkEndCommandBuffer(m_commandBuffers.at(imageIndex)) != VK_SUCCESS) {
		throw std::runtime_error("failed to record command buffer!");
	}
}

void CurenInit::renderObjects(VkCommandBuffer commandBuffer)
{
	m_curenPipeline->bind(commandBuffer);

	for (auto &obj : m_curenObjects)
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

