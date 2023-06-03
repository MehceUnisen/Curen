#pragma once

#include "curen_camera.hpp"

#include <vulkan/vulkan.h>

namespace Curen {
	struct FrameInfo {
		int frameIndex;
		float frameTime;
		VkCommandBuffer commandBuffer;
		CurenCamera& camera;
		VkDescriptorSet globalDescriptorSet;
	};
}