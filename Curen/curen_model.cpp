#include "curen_model.hpp"


#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

namespace std {
	template <>
	struct hash<Curen::CurenModel::Vertex> {
		size_t operator()(Curen::CurenModel::Vertex const& vertex) const {
			size_t seed = 0;
			Utils::hashCombine(seed, vertex.position, vertex.color, vertex.normal, vertex.uv);
			return seed;
		}
	};
}

Curen::CurenModel::CurenModel(CurenDevice& curenDevice, const CurenModel::Builder& builder) :
	m_curenDevice {curenDevice}
{
	createVertexBuffer(builder.vertices);
	createIndexBuffer(builder.indices);
}

Curen::CurenModel::~CurenModel()
{
	vkDestroyBuffer(m_curenDevice.device(), m_vertexBuffer, nullptr);
	vkFreeMemory(m_curenDevice.device(), m_vertexBufferMemory, nullptr);

	if (m_hasIndexBuffer)
	{
		vkDestroyBuffer(m_curenDevice.device(), m_indexBuffer, nullptr);
		vkFreeMemory(m_curenDevice.device(), m_indexBufferMemory, nullptr);
	}
}

void Curen::CurenModel::Builder::loadModel(const std::string& filePath) 
{
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string warn, err;

	if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filePath.c_str())) {
		throw std::runtime_error(warn + err);
	}

	vertices.clear();
	indices.clear();

	std::unordered_map<Vertex, uint32_t> uniqueVertices{};
	for (const auto& shape : shapes) {
		for (const auto& index : shape.mesh.indices) {
			Vertex vertex{};

			if (index.vertex_index >= 0) {
				vertex.position = {
					attrib.vertices[3 * index.vertex_index + 0],
					attrib.vertices[3 * index.vertex_index + 1],
					attrib.vertices[3 * index.vertex_index + 2],
				};

				vertex.color = {
					attrib.colors[3 * index.vertex_index + 0],
					attrib.colors[3 * index.vertex_index + 1],
					attrib.colors[3 * index.vertex_index + 2],
				};

			}

			if (index.normal_index >= 0) {
				vertex.normal = {
					attrib.normals[3 * index.normal_index + 0],
					attrib.normals[3 * index.normal_index + 1],
					attrib.normals[3 * index.normal_index + 2],
				};
			}

			if (index.texcoord_index >= 0) {
				vertex.uv = {
					attrib.texcoords[2 * index.texcoord_index + 0],
					attrib.texcoords[2 * index.texcoord_index + 1],
				};
			}

			if (uniqueVertices.count(vertex) == 0) {
				uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
				vertices.push_back(vertex);
			}
			indices.push_back(uniqueVertices[vertex]);
		}
	}
}

std::unique_ptr<Curen::CurenModel> Curen::CurenModel::createModelFromFile(CurenDevice& curenDevice, const std::string& filePath)
{
	Builder builder{};
	builder.loadModel(filePath);

	return std::make_unique<Curen::CurenModel>(curenDevice, builder);
}

void Curen::CurenModel::bind(VkCommandBuffer commandBuffer)
{
	VkBuffer buffers[] = { m_vertexBuffer };
	VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);
	
	if (m_hasIndexBuffer)
	{
		vkCmdBindIndexBuffer(commandBuffer, m_indexBuffer, 0, VK_INDEX_TYPE_UINT32); 
	}
}

void Curen::CurenModel::draw(VkCommandBuffer commandBuffer)
{
	if (m_hasIndexBuffer)
	{
		vkCmdDrawIndexed(commandBuffer, m_indexCount, 1, 0, 0, 0);

	}
	else {
		vkCmdDraw(commandBuffer, m_vertexCount, 1, 0, 0);
	}
}

void Curen::CurenModel::createVertexBuffer(const std::vector<Vertex>& vertices)
{
	m_vertexCount = static_cast<uint32_t>(vertices.size());
	assert(m_vertexCount >= 3 && "Vertex count must be at least three.");
	VkDeviceSize bufferSize = sizeof(vertices.at(0)) * m_vertexCount;

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;


	m_curenDevice.createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		stagingBuffer, stagingBufferMemory);
	void* data;
	vkMapMemory(m_curenDevice.device(), stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, vertices.data(), (size_t)bufferSize);
	vkUnmapMemory(m_curenDevice.device(), stagingBufferMemory);

	m_curenDevice.createBuffer(bufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		m_vertexBuffer, m_vertexBufferMemory);

	m_curenDevice.copyBuffer(stagingBuffer, m_vertexBuffer, bufferSize);
	
	vkDestroyBuffer(m_curenDevice.device(), stagingBuffer, nullptr);
	vkFreeMemory(m_curenDevice.device(), stagingBufferMemory, nullptr);

}


void Curen::CurenModel::createIndexBuffer(const std::vector<uint32_t>& indices)
{
	m_indexCount = static_cast<uint32_t>(indices.size());
	m_hasIndexBuffer = m_indexCount > 0;

	if (!m_hasIndexBuffer)
	{
		return;
	}

	VkDeviceSize bufferSize = sizeof(indices.at(0)) * m_indexCount;


	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;


	m_curenDevice.createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		stagingBuffer, stagingBufferMemory);
	void* data;
	vkMapMemory(m_curenDevice.device(), stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, indices.data(), (size_t)bufferSize);
	vkUnmapMemory(m_curenDevice.device(), stagingBufferMemory);

	m_curenDevice.createBuffer(bufferSize, VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		m_indexBuffer, m_indexBufferMemory);

	m_curenDevice.copyBuffer(stagingBuffer, m_indexBuffer, bufferSize);

	vkDestroyBuffer(m_curenDevice.device(), stagingBuffer, nullptr);
	vkFreeMemory(m_curenDevice.device(), stagingBufferMemory, nullptr);

}

std::vector<VkVertexInputBindingDescription> Curen::CurenModel::Vertex::getBindingDescriptions()
{
	std::vector<VkVertexInputBindingDescription> bindingDescriptions(1);
	bindingDescriptions[0].binding = 0;
	bindingDescriptions[0].stride = sizeof(Vertex);
	bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
	return bindingDescriptions;
}

std::vector<VkVertexInputAttributeDescription> Curen::CurenModel::Vertex::getAttributeDescriptions()
{

	std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};
	attributeDescriptions.push_back({ 0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, position) });
	attributeDescriptions.push_back({ 1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, color) });
	attributeDescriptions.push_back({ 2, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, normal) });
	attributeDescriptions.push_back({ 3, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, uv) });

	return attributeDescriptions;
}
