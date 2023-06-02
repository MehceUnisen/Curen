#pragma once

#include "curen_window.hpp"
#include "curen_device.hpp"
#include "curen_swap_chain.hpp"
#include "curen_model.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <memory>
#include <vector>
#include <stdexcept>
#include <array>
#include <iostream>
#include <cassert>

namespace Curen {

	class CurenRenderer {
	public:
		
		
		CurenRenderer(CurenWindow& curenWindow, CurenDevice& curenDevice);
		~CurenRenderer();

		CurenRenderer(const CurenRenderer&) = delete;
		CurenRenderer& operator = (const CurenRenderer&) = delete;

		VkRenderPass getSwapChainRenderPass() const { return m_curenSwapChain->getRenderPass(); }
		float getAspectRatio() const { return m_curenSwapChain->extentAspectRatio(); }

		bool isFrameInProgress() const { return m_isFrameStarted; }

		VkCommandBuffer getCurrentCommandBuffer() const { 
			assert(m_isFrameStarted && "Can't get command buffer when frame is in progress");
			return m_commandBuffers.at(m_currentFrameIndex); 
		}

		int getFrameIndex() const { 
			assert(m_isFrameStarted && "Can't get frame index when frame is in progress");
			return m_currentFrameIndex;
		}

		VkCommandBuffer beginFrame();
		void endFrame();

		void beginSwapChainRenderPass(VkCommandBuffer commandBuffer);
		void endSwapChainRenderPass(VkCommandBuffer commandBuffer);

	private:
		void createCommandBuffers();
		void freeCommandBuffers();
		void recreateSwapChain();

		CurenWindow& m_curenWindow;
		CurenDevice& m_curenDevice;
		std::unique_ptr<CurenSwapChain> m_curenSwapChain;
		std::vector<VkCommandBuffer> m_commandBuffers;

		uint32_t m_currentImageIndex;
		int m_currentFrameIndex;
		bool m_isFrameStarted;
	};
}