#include "curen_init.hpp"

using namespace Curen;

namespace Curen {
    struct GlobalUbo {
        glm::mat4 projectionView{1.f};
        glm::vec4 ambientLightColor{1.f, 1.f, 1.f, .02f};
        glm::vec3 lightPosition{-1.f};
        alignas(16) glm::vec4 lightColor{1.f};
    };
}
CurenInit::CurenInit()
{
	m_globalDescriptorPool = CurenDescriptorPool::Builder(m_curenDevice)
        .setMaxSets(CurenSwapChain::MAX_FRAMES_IN_FLIGHT)
        .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, CurenSwapChain::MAX_FRAMES_IN_FLIGHT)
        .build();
    loadObjects();
}

Curen::CurenInit::~CurenInit()
{
	
}

void CurenInit::run() {

    std::vector<std::unique_ptr<CurenBuffer>> uboBuffers(CurenSwapChain::MAX_FRAMES_IN_FLIGHT);
    for (int i = 0; i < uboBuffers.size(); i++)
    {
        uboBuffers.at(i) = std::make_unique<CurenBuffer>(
            m_curenDevice,
            sizeof(GlobalUbo),
            1,
            VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
        uboBuffers.at(i)->map();
    }
    
    auto globalSetLayout = CurenDescriptorSetLayout::Builder(m_curenDevice)
        .addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT)
        .build();

    std::vector<VkDescriptorSet> globalDescriptorSets(CurenSwapChain::MAX_FRAMES_IN_FLIGHT);
    for (int i = 0; i < globalDescriptorSets.size(); i++)
    {
        auto bufferInfo = uboBuffers.at(i)->descriptorInfo();
        CurenDescriptorWriter(*globalSetLayout, *m_globalDescriptorPool)
            .writeBuffer(0, &bufferInfo)
            .build(globalDescriptorSets.at(i));
    }

	CurenRenderSystem renderSystem {m_curenDevice, m_curenRenderer.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout()};

    CurenCamera camera{};
    camera.setViewTarget(glm::vec3(-1.f, -2.f, -2.5f), glm::vec3(0.f, 0.f, 2.5f));
    
    CurenObject viewerObject = CurenObject::createObject();
    viewerObject.transformComponent.translation.z = -2.5f;
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
        camera.setPerspectiveProjection(glm::radians(50.f), aspect, 0.1f, 100.f);
		
        if (auto commandBuffer = m_curenRenderer.beginFrame()) {

            int frameIndex = m_curenRenderer.getFrameIndex();
            FrameInfo frameInfo{ frameIndex, frameTime, commandBuffer, camera, globalDescriptorSets.at(frameIndex)};

            GlobalUbo globalUbo{};
            globalUbo.projectionView = camera.getProjection() * camera.getView();
            uboBuffers.at(frameIndex)->writeToBuffer(&globalUbo);
            uboBuffers.at(frameIndex)->flush();

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
    cube.transformComponent.translation = glm::vec3(1.f, 0.5f, .0f);
    cube.transformComponent.scale = { 3.f, 1.5f, 3.f };
    m_curenObjects.push_back(std::move(cube));

    curenModel = CurenModel::createModelFromFile(m_curenDevice, "../Models/smooth_vase.obj");
    auto x = CurenObject::createObject();
    x.model = curenModel;
    x.transformComponent.translation = glm::vec3(-1.f, 0.5f, .0f);
    x.transformComponent.scale = { 3.f, 1.5f, 3.f };
    m_curenObjects.push_back(std::move(x));
    
    curenModel = CurenModel::createModelFromFile(m_curenDevice, "../Models/quad.obj");
    auto y = CurenObject::createObject();
    y.model = curenModel;
    y.transformComponent.translation = glm::vec3(0.f, 0.5f, .0f);
    y.transformComponent.scale = { 3.f, 1.f, 3.f };
    m_curenObjects.push_back(std::move(y));

}
 