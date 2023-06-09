#pragma once

#include "curen_window.hpp"
#include "curen_device.hpp"
#include "curen_renderer.hpp"
#include "curen_model.hpp"
#include "curen_object.hpp"
#include "curen_render_system.hpp"
#include "curen_camera.hpp"
#include "keyboard_manager.hpp"
#include "curen_descriptor.hpp"
#include "curen_point_light_system.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <memory>
#include <vector>
#include <stdexcept>
#include <array>
#include <iostream>
#include <chrono>

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
		void loadObjects();

		CurenWindow m_curenWindow{WIDTH, HEIGHT, "Curen"};
		CurenDevice m_curenDevice{ m_curenWindow };
		CurenRenderer m_curenRenderer{ m_curenWindow, m_curenDevice };
		std::unique_ptr <CurenDescriptorPool> m_globalDescriptorPool{};
		CurenObject::Map m_curenObjects;
	};
}