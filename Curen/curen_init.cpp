#include "curen_init.hpp"
#include "curen_camera.hpp"
#include "keyboard_manager.hpp"

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
    camera.setViewTarget(glm::vec3(-1.f, -2.f, -2.f), glm::vec3(0.f, 0.f, 2.5f));

    CurenObject viewerObject = CurenObject::createObject();
    KeyboardManager cameraController{};

    auto currentTime = std::chrono::high_resolution_clock::now();

	while (!m_curenWindow.shouldClose()) {
		glfwPollEvents();

        auto newTime = std::chrono::high_resolution_clock::now();
        float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
        currentTime = newTime;
        
        cameraController.moveInPlaneXZ(m_curenWindow.getWindow(), frameTime, viewerObject);
        camera.setViewYXZ(viewerObject.transformComponent.translation, viewerObject.transformComponent.rotation);
        
        float aspect = m_curenRenderer.getAspectRatio();
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
    CurenModel::Builder builder{};
    builder.vertices = {// left face (white)
        // left face (white)
      {{-.5f, -.5f, -.5f}, {.9f, .9f, .9f}},
      {{-.5f, .5f, .5f}, {.9f, .9f, .9f}},
      {{-.5f, -.5f, .5f}, {.9f, .9f, .9f}},
      {{-.5f, .5f, -.5f}, {.9f, .9f, .9f}},

      // right face (yellow)
      {{.5f, -.5f, -.5f}, {.8f, .8f, .1f}},
      {{.5f, .5f, .5f}, {.8f, .8f, .1f}},
      {{.5f, -.5f, .5f}, {.8f, .8f, .1f}},
      {{.5f, .5f, -.5f}, {.8f, .8f, .1f}},

      // top face (orange, remember y axis points down)
      {{-.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
      {{.5f, -.5f, .5f}, {.9f, .6f, .1f}},
      {{-.5f, -.5f, .5f}, {.9f, .6f, .1f}},
      {{.5f, -.5f, -.5f}, {.9f, .6f, .1f}},

      // bottom face (red)
      {{-.5f, .5f, -.5f}, {.8f, .1f, .1f}},
      {{.5f, .5f, .5f}, {.8f, .1f, .1f}},
      {{-.5f, .5f, .5f}, {.8f, .1f, .1f}},
      {{.5f, .5f, -.5f}, {.8f, .1f, .1f}},

      // nose face (blue)
      {{-.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
      {{.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
      {{-.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
      {{.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},

      // tail face (green)
      {{-.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
      {{.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
      {{-.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
      {{.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
    };

    builder.indices = { 0,  1,  2,  0,  3,  1,  4,  5,  6,  4,  7,  5,  8,  9,  10, 8,  11, 9,
                          12, 13, 14, 12, 15, 13, 16, 17, 18, 16, 19, 17, 20, 21, 22, 20, 23, 21 };

    for (auto& v : builder.vertices) {
        v.position += offset;
    }
    return std::make_unique<CurenModel>(device, builder);
}


void Curen::CurenInit::loadObjects()
{
    std::shared_ptr<CurenModel> curenModel = createCubeModel(m_curenDevice, { .0f, .0f, .0f });
    auto cube = CurenObject::createObject();
    cube.model = curenModel;
    cube.transformComponent.translation = glm::vec3(0.f, 0.f, 2.5f);
    cube.transformComponent.scale = { 0.5f, 0.5f , 0.5f };
    m_curenObjects.push_back(std::move(cube));
}
 