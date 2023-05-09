#include "curen_init.hpp"

using namespace Curen;

void CurenInit::run() {
	while (!m_curenWindow.shouldClose()) {
		glfwPollEvents();
	}
}