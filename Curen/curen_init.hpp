#pragma once

#include "curen_window.hpp"
#include "curen_pipeline.hpp"
#include "curen_device.hpp"
#include "curen_swap_chain.hpp"
#include "curen_model.hpp"

#include <memory>
#include <vector>
#include <stdexcept>
#include <array>

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
		void loadModels();
		void recreateSwapChain();
		void recordCommandBuffer(int imageIndex);


		CurenWindow m_curenWindow{WIDTH, HEIGHT, "Curen"};
		CurenDevice m_curenDevice{ m_curenWindow };
		std::unique_ptr<CurenSwapChain> m_curenSwapChain;
		std::unique_ptr<CurenPipeline> m_curenPipeline;
		VkPipelineLayout m_pipelineLayout;
		std::vector<VkCommandBuffer> m_commandBuffers;
		std::unique_ptr<CurenModel> m_curenModel;
	};
}