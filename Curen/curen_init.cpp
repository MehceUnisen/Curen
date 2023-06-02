#include "curen_init.hpp"
#include "curen_camera.hpp"
#include "keyboard_manager.hpp"

using namespace Curen;

namespace Curen {
    struct GlobalUbo {
        glm::mat4 projectionView{1.f};
        glm::vec3 lightDirection = glm::normalize(glm::vec3(1.f, -3.f, -1.f));
    };
}
CurenInit::CurenInit()
{
	loadObjects();
    
	
}

Curen::CurenInit::~CurenInit()
{
	
}

void CurenInit::run() {

    CurenBuffer globalUboBuffer{
        m_curenDevice,
        sizeof(GlobalUbo),
        CurenSwapChain::MAX_FRAMES_IN_FLIGHT,
        VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
        m_curenDevice.properties.limits.minUniformBufferOffsetAlignment
    };

    globalUboBuffer.map();

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

            int frameIndex = m_curenRenderer.getFrameIndex();
            FrameInfo frameInfo{ frameIndex, frameTime, commandBuffer, camera };

            GlobalUbo globalUbo{};
            globalUbo.projectionView = camera.getProjection() * camera.getView();
            globalUboBuffer.writeToIndex(&globalUbo, frameIndex);
            globalUboBuffer.flush(frameIndex);

            m_curenRenderer.beginSwapChainRenderPass(commandBuffer);
			renderSystem.renderObjects(frameInfo, m_curenObjects);
			m_curenRenderer.endSwapChainRenderPass(commandBuffer);
			m_curenRenderer.endFrame();
		}

	}
	vkDeviceWaitIdle(m_curenDevice.device());
}

void Curen::CurenInit::loadObjects()
{
    std::shared_ptr<CurenModel> curenModel = CurenModel::createModelFromFile(m_curenDevice, "../Models/flat_vase.obj");
    auto cube = CurenObject::createObject();
    cube.model = curenModel;
    cube.transformComponent.translation = glm::vec3(1.f, 0.5f, 2.5f);
    cube.transformComponent.scale = { 3.f, 1.5f, 3.f };
    m_curenObjects.push_back(std::move(cube));

    curenModel = CurenModel::createModelFromFile(m_curenDevice, "../Models/smooth_vase.obj");
    auto x = CurenObject::createObject();
    x.model = curenModel;
    x.transformComponent.translation = glm::vec3(-1.f, 0.5f, 2.5f);
    x.transformComponent.scale = { 3.f, 1.5f, 3.f };
    m_curenObjects.push_back(std::move(x));
}
 