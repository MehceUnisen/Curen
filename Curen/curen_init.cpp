 #include "curen_init.hpp"

using namespace Curen;



Curen::CurenInit::CurenInit()
{
	loadObjects();
	
}

Curen::CurenInit::~CurenInit()
{
	
}

void CurenInit::run() {
	CurenRenderSystem renderSystem {m_curenDevice, m_curenRenderer.getSwapChainRenderPass()};
	while (!m_curenWindow.shouldClose()) {
		glfwPollEvents();
		
		if (auto commandBuffer = m_curenRenderer.beginFrame()) {
			m_curenRenderer.beginSwapChainRenderPass(commandBuffer);
			renderSystem.renderObjects(commandBuffer, m_curenObjects);
			m_curenRenderer.endSwapChainRenderPass(commandBuffer);
			m_curenRenderer.endFrame();
		}

	}
	vkDeviceWaitIdle(m_curenDevice.device());
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
	triangle.transform2DComponent.rotation = 0.05f * glm::two_pi<float>();
	m_curenObjects.push_back(std::move(triangle));
}
