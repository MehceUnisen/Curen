#pragma once

#include "curen_device.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <vector>
#include <cassert>

namespace Curen {
	class CurenModel {
	public:

		struct Vertex {
			glm::vec2 position;
			glm::vec3 color;
			static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
			static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();
		};

		CurenModel(CurenDevice& curenDevice, const std::vector<Vertex>& vertices);
		~CurenModel();

		CurenModel(const CurenModel&) = delete;
		CurenModel& operator = (const CurenModel&) = delete;

		void bind(VkCommandBuffer commandBuffer);
		void draw(VkCommandBuffer commandBuffer);

	private:

		void createVertexBuffer(const std::vector<Vertex>& vertices);

		CurenDevice& m_curenDevice;
		VkBuffer m_vertexBuffer;
		VkDeviceMemory m_vertexBufferMemory;
		uint32_t m_vertexCount;
	};
}