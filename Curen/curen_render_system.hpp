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



	class CurenRenderSystem {
	public:

		void renderObjects(FrameInfo& frameInfo, std::vector<CurenObject>& curenObjects);

		CurenRenderSystem(CurenDevice& device, VkRenderPass renderPass);
		~CurenRenderSystem();

		CurenRenderSystem(const CurenRenderSystem&) = delete;
		CurenRenderSystem& operator = (const CurenRenderSystem&) = delete;

	private:
		void createPipelineLayout();
		void createPipeline(VkRenderPass renderPass);
		

		CurenDevice& m_curenDevice;

		std::unique_ptr<CurenPipeline> m_curenPipeline;
		VkPipelineLayout m_pipelineLayout;
	};
}