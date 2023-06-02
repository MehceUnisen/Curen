#pragma once

#include "curen_device.hpp"
#include "curen_utils.hpp"
#include "curen_buffer.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <vector>
#include <cassert>
#include <memory>
#include <unordered_map>

namespace Curen {
	class CurenModel {
	public:

		struct Vertex {
			glm::vec3 position{};
			glm::vec3 color{};
			glm::vec3 normal{};
			glm::vec2 uv{};

			static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
			static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();

			bool operator==(const Vertex& other) const {
				return position == other.position && color == other.color && normal == other.normal &&
					uv == other.uv;
			}
		};

		struct Builder {
			std::vector<Vertex> vertices{};
			std::vector<uint32_t> indices{};

			void loadModel(const std::string& filePath);
		};

		CurenModel(CurenDevice& curenDevice, const CurenModel::Builder& builder);
		~CurenModel();

		CurenModel(const CurenModel&) = delete;
		CurenModel& operator = (const CurenModel&) = delete;

		static std::unique_ptr<CurenModel> createModelFromFile(CurenDevice& curenDevice, const std::string& filePath);

		void bind(VkCommandBuffer commandBuffer);
		void draw(VkCommandBuffer commandBuffer);

	private:

		void createVertexBuffer(const std::vector<Vertex>& vertices);
		void createIndexBuffer(const std::vector<uint32_t>& indices);

		CurenDevice& m_curenDevice;

		std::unique_ptr<CurenBuffer> m_vertexBuffer;
		uint32_t m_vertexCount;

		bool m_hasIndexBuffer = false;
		std::unique_ptr<CurenBuffer> m_indexBuffer;
		uint32_t m_indexCount;
	};
}