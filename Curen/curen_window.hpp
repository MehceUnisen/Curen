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

		
		CurenWindow(const CurenWindow&) = delete;
		CurenWindow& operator = (const CurenWindow&) = delete;
		
		VkExtent2D getExtent() { return { static_cast<uint32_t>(m_width), static_cast<uint32_t>(m_height) }; }
		GLFWwindow* getWindow() const { return m_window; }


		bool shouldClose() { return glfwWindowShouldClose(m_window); }
		bool wasWindowResized() { return m_frameBufferResized; }
		void resetWindowResizedFlag() { m_frameBufferResized = false; }
		void createWindowSurface(VkInstance instance, VkSurfaceKHR* surface);

	private: 
		static void frameBufferResizeCallback(GLFWwindow* window, int width, int height);
		void initWindow();
		
		int m_width;
		int m_height;
		
		bool m_frameBufferResized = false;

		std::string m_windowName;
		GLFWwindow* m_window;
	};
} // namespace curen