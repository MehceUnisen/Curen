#include "curen_window.hpp"

using namespace Curen;

CurenWindow::CurenWindow(int width, int height, std::string name) : m_width{ width }, m_height{ height }, m_windowName{ name } 
{
	initWindow();
}

void CurenWindow::initWindow() {
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	m_window = glfwCreateWindow(m_width, m_height, m_windowName.c_str(), nullptr, nullptr);
}

CurenWindow::~CurenWindow() {
	glfwDestroyWindow(m_window);
	glfwTerminate();
}