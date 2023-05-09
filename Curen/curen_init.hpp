#pragma once

#include "curen_window.hpp"
#include "curen_pipeline.hpp"
#include "curen_device.hpp"

namespace Curen {
	class CurenInit {
	public:
		static constexpr int WIDTH = 800;
		static constexpr int HEIGHT = 600;

		void run();

	private:
		CurenWindow m_curenWindow{WIDTH, HEIGHT, "Curen"};
		CurenDevice m_curenDevice{ m_curenWindow };
		CurenPipeline m_curenPipline{m_curenDevice, "first_shader.vert.spv", "first_shader.frag.spv", CurenPipeline::defPipelineConfigInfo(WIDTH, HEIGHT)};
	};
}