#pragma once

#include "curen_camera.hpp"
#include "curen_pipeline.hpp"
#include "curen_device.hpp"
#include "curen_object.hpp"
#include "curen_frame_info.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <memory>
#include <vector>
#include <stdexcept>
#include <array>
#include <iostream>

namespace Curen {



	class CurenPointLightSystem {
	public:

		CurenPointLightSystem(CurenDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout globalDescriptorSet);
		~CurenPointLightSystem();

		CurenPointLightSystem(const CurenPointLightSystem&) = delete;
		CurenPointLightSystem& operator = (const CurenPointLightSystem&) = delete;
		
		void render(FrameInfo& frameInfo);

	private:
		void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
		void createPipeline(VkRenderPass renderPass);
		

		CurenDevice& m_curenDevice;

		std::unique_ptr<CurenPipeline> m_curenPipeline;
		VkPipelineLayout m_pipelineLayout;
	};
}
