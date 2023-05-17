#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>
#include <stdexcept>

namespace Curen {
	class CurenWindow {
	public:
		CurenWindow(int width, int height, std::string name);
		~CurenWindow();

		VkExtent2D getExtent() { return { static_cast<uint32_t>(m_width), static_cast<uint32_t>(m_height) }; }

		CurenWindow(const CurenWindow&) = delete;
		CurenWindow& operator = (const CurenWindow&) = delete;

		bool shouldClose() { return glfwWindowShouldClose(m_window); }
		void createWindowSurface(VkInstance instance, VkSurfaceKHR* surface);

	private:
		void initWindow();
		
		const int m_width;
		const int m_height;
		
		std::string m_windowName;
		GLFWwindow* m_window;
	};
} // namespace curen