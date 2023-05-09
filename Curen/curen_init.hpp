#pragma once

#include "curen_window.hpp"
#include "curen_pipeline.hpp"

namespace Curen {
	class CurenInit {
	public:
		static constexpr int WIDTH = 800;
		static constexpr int HEIGHT = 600;

		void run();

	private:
		CurenWindow m_curenWindow{WIDTH, HEIGHT, "Curen"};
		CurenPipeline m_curenPipline{ "first_shader.vert.spv", "first_shader.frag.spv" };
	};
}