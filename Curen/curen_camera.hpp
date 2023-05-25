#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <cassert>
#include <limits>

namespace Curen {
	class CurenCamera {
	public:
		void setOrthographicProjection(float left, float right, float top, float bottom, float near, float far);
		void setPerspectiveProjection(float fovy, float aspcect, float near, float far);
		const glm::mat4& getProjection() const { return m_projectionMatrix; }
	private:
		glm::mat4 m_projectionMatrix{ 1.f };
	};
}