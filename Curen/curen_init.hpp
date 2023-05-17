#pragma once

#include "curen_window.hpp"
#include "curen_pipeline.hpp"
#include "curen_device.hpp"
#include "curen_swap_chain.hpp"

#include <memory>
#include <vector>

namespace Curen {
	class CurenInit {
	public:
		static constexpr int WIDTH = 800;
		static constexpr int HEIGHT = 600;

		void run();

		CurenInit();
		~CurenInit();

		CurenInit(const CurenInit&) = delete;
		CurenInit& operator = (const CurenInit&) = delete;

	private:
		void createPipelineLayout();
		void createPipeline();
		void createCommandBuffers();
		void drawFrame();

		CurenWindow m_curenWindow{WIDTH, HEIGHT, "Curen"};
		CurenDevice m_curenDevice{ m_curenWindow };
		CurenSwapChain m_curenSwapChain{ m_curenDevice, m_curenWindow.getExtent() };
		std::unique_ptr<CurenPipeline> m_curenPipeline;
		VkPipelineLayout m_pipelineLayout;
		std::vector<VkCommandBuffer> m_commandBuffers;
	};
}