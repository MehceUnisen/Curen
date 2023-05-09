#include "curen_pipeline.hpp"

#include <fstream>
#include <stdexcept>
#include <iostream>

using namespace Curen;

CurenPipeline::CurenPipeline(const std::string& vertFilePath, const std::string& fragFilePath) {
	createGraphicsPipeline(vertFilePath, fragFilePath);
}

std::vector<char> CurenPipeline::readFile(const std::string& filepath) {
	std::ifstream file(filepath, std::ios::ate | std::ios::binary);

	if (!file.is_open())
	{
		throw std::runtime_error("Failed to open the file : " + filepath);
	}

	std::size_t fileSize = static_cast<std::size_t>(file.tellg());
	std::vector<char> buffer(fileSize);

	file.seekg(0);
	file.read(buffer.data(), fileSize);
	file.close();

	return buffer;
}

void CurenPipeline::createGraphicsPipeline(const std::string& vertFilePath, const std::string& fragFilePath) {
	std::vector<char> vertCode = readFile(vertFilePath);
	std::vector<char> fragCode = readFile(fragFilePath);

	std::cout << "vert file size = " << vertCode.size() << '\n';
	std::cout << "frag file size = " << fragCode.size() << '\n';
}