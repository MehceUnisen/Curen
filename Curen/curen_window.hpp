#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>

namespace Curen {
	class CurenWindow {
	public:
		CurenWindow(int width, int height, std::string name);
		~CurenWindow();

		CurenWindow(const CurenWindow&) = delete;
		CurenWindow& operator = (const CurenWindow&) = delete;

		bool shouldClose() { return glfwWindowShouldClose(m_window); }

	private:
		void initWindow();
		
		const int m_width;
		const int m_height;
		
		std::string m_windowName;
		GLFWwindow* m_window;
	};
} // namespace curen