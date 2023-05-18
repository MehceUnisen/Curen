#include "curen_model.hpp"

Curen::CurenModel::CurenModel(CurenDevice& curenDevice, const std::vector<Vertex>& vertices) :
	m_curenDevice {curenDevice}
{
	createVertexBuffer(vertices);
}

Curen::CurenModel::~CurenModel()
{
	vkDestroyBuffer(m_curenDevice.device(), m_vertexBuffer, nullptr);
	vkFreeMemory(m_curenDevice.device(), m_vertexBufferMemory, nullptr);
}

void Curen::CurenModel::bind(VkCommandBuffer commandBuffer)
{
	VkBuffer buffers[] = { m_vertexBuffer };
	VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);
}

void Curen::CurenModel::draw(VkCommandBuffer commandBuffer)
{
	vkCmdDraw(commandBuffer, m_vertexCount, 1, 0, 0);
}

void Curen::CurenModel::createVertexBuffer(const std::vector<Vertex>& vertices)
{
	m_vertexCount = static_cast<uint32_t>(vertices.size());
	assert(m_vertexCount >= 3 && "Vertex count must be at least three.");
	VkDeviceSize bufferSize = sizeof(vertices.at(0)) * m_vertexCount;
	m_curenDevice.createBuffer(bufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		m_vertexBuffer, m_vertexBufferMemory);
	void* data;
	vkMapMemory(m_curenDevice.device(), m_vertexBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, vertices.data(), (size_t)bufferSize);
	vkUnmapMemory(m_curenDevice.device(), m_vertexBufferMemory);
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

	std::vector<VkVertexInputAttributeDescription> attributeDescriptions(2);

	attributeDescriptions[0].binding = 0;
	attributeDescriptions[0].location = 0;
	attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
	attributeDescriptions[0].offset = offsetof(Vertex, position);

	attributeDescriptions[1].binding = 0;
	attributeDescriptions[1].location = 1;
	attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
	attributeDescriptions[1].offset = offsetof(Vertex, color);


	return attributeDescriptions;
}
