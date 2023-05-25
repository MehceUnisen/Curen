#include "curen_init.hpp"
#include "curen_camera.hpp"

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

    CurenCamera camera{};

	while (!m_curenWindow.shouldClose()) {
		glfwPollEvents();
        float aspect = m_curenRenderer.getAspectRatio();
        //camera.setOrthographicProjection(-aspect, aspect, -1, 1, -1, 1);
        camera.setPerspectiveProjection(glm::radians(50.f), aspect, 0.1f, 10.f);
		if (auto commandBuffer = m_curenRenderer.beginFrame()) {
			m_curenRenderer.beginSwapChainRenderPass(commandBuffer);
			renderSystem.renderObjects(commandBuffer, m_curenObjects, camera);
			m_curenRenderer.endSwapChainRenderPass(commandBuffer);
			m_curenRenderer.endFrame();
		}

	}
	vkDeviceWaitIdle(m_curenDevice.device());
}

std::unique_ptr<CurenModel> createCubeModel(CurenDevice& device, glm::vec3 offset) {
    std::vector<CurenModel::Vertex> vertices{// left face (white)
        {{-.5f, -.5f, -.5f}, { .9f, .9f, .9f }},
        { {-.5f, .5f, .5f}, {.9f, .9f, .9f} },
        { {-.5f, -.5f, .5f}, {.9f, .9f, .9f} },
        { {-.5f, -.5f, -.5f}, {.9f, .9f, .9f} },
        { {-.5f, .5f, -.5f}, {.9f, .9f, .9f} },
        { {-.5f, .5f, .5f}, {.9f, .9f, .9f} },

            // right face (yellow)
        { {.5f, -.5f, -.5f}, {.8f, .8f, .1f} },
        { {.5f, .5f, .5f}, {.8f, .8f, .1f} },
        { {.5f, -.5f, .5f}, {.8f, .8f, .1f} },
        { {.5f, -.5f, -.5f}, {.8f, .8f, .1f} },
        { {.5f, .5f, -.5f}, {.8f, .8f, .1f} },
        { {.5f, .5f, .5f}, {.8f, .8f, .1f} },

            // top face (orange, remember y axis points down)
        { {-.5f, -.5f, -.5f}, {.9f, .6f, .1f} },
        { {.5f, -.5f, .5f}, {.9f, .6f, .1f} },
        { {-.5f, -.5f, .5f}, {.9f, .6f, .1f} },
        { {-.5f, -.5f, -.5f}, {.9f, .6f, .1f} },
        { {.5f, -.5f, -.5f}, {.9f, .6f, .1f} },
        { {.5f, -.5f, .5f}, {.9f, .6f, .1f} },

            // bottom face (red)
        { {-.5f, .5f, -.5f}, {.8f, .1f, .1f} },
        { {.5f, .5f, .5f}, {.8f, .1f, .1f} },
        { {-.5f, .5f, .5f}, {.8f, .1f, .1f} },
        { {-.5f, .5f, -.5f}, {.8f, .1f, .1f} },
        { {.5f, .5f, -.5f}, {.8f, .1f, .1f} },
        { {.5f, .5f, .5f}, {.8f, .1f, .1f} },

            // nose face (blue)
        { {-.5f, -.5f, 0.5f}, {.1f, .1f, .8f} },
        { {.5f, .5f, 0.5f}, {.1f, .1f, .8f} },
        { {-.5f, .5f, 0.5f}, {.1f, .1f, .8f} },
        { {-.5f, -.5f, 0.5f}, {.1f, .1f, .8f} },
        { {.5f, -.5f, 0.5f}, {.1f, .1f, .8f} },
        { {.5f, .5f, 0.5f}, {.1f, .1f, .8f} },

            // tail face (green)
        { {-.5f, -.5f, -0.5f}, {.1f, .8f, .1f} },
        { {.5f, .5f, -0.5f}, {.1f, .8f, .1f} },
        { {-.5f, .5f, -0.5f}, {.1f, .8f, .1f} },
        { {-.5f, -.5f, -0.5f}, {.1f, .8f, .1f} },
        { {.5f, -.5f, -0.5f}, {.1f, .8f, .1f} },
        { {.5f, .5f, -0.5f}, {.1f, .8f, .1f} }, };

    for (auto& v : vertices) {
        v.position += offset;
    }
    return std::make_unique<CurenModel>(device, vertices);
}


void Curen::CurenInit::loadObjects()
{
    std::shared_ptr<CurenModel> curenModel = createCubeModel(m_curenDevice, { .0f, .0f, .0f });
    auto cube = CurenObject::createObject();
    cube.model = curenModel;
    cube.transformComponent.translation = { .0f, .0f, 1.5f };
    cube.transformComponent.scale = { 0.5f, 0.5f , 0.5f };
    m_curenObjects.push_back(std::move(cube));
}
 