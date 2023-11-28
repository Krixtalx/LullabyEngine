#include "LullabyPch.h"
#include "VKInitializers.h"

VkCommandPoolCreateInfo Lullaby::Helpers::commandPoolCreateInfo(const uint32_t& queueFamily, const VkCommandPoolCreateFlags& vkFlags) {
	const VkCommandPoolCreateInfo info = {
		.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
		.pNext = nullptr,
		.flags = vkFlags,
		.queueFamilyIndex = queueFamily
	};
	return info;
}

VkCommandBufferAllocateInfo Lullaby::Helpers::commandBufferAllocateInfo(const VkCommandPool& commandPool, const uint32_t& count, const VkCommandBufferLevel level) {
	const VkCommandBufferAllocateInfo info = {
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
		.pNext = nullptr,
		.commandPool = commandPool,
		.level = level,
		.commandBufferCount = count
	};
	return info;
}

VkImageCreateInfo Lullaby::Helpers::imageCreateInfo(const VkFormat format, const VkImageUsageFlags usageFlags, const VkExtent3D extent) {
	const VkImageCreateInfo info{
		.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
		.pNext = nullptr,
		.imageType = VK_IMAGE_TYPE_2D,
		.format = format,
		.extent = extent,
		.mipLevels = 1,
		.arrayLayers = 1,
		.samples = VK_SAMPLE_COUNT_1_BIT,
		.tiling = VK_IMAGE_TILING_OPTIMAL,
		.usage = usageFlags,
	};
	return info;
}

VkImageViewCreateInfo Lullaby::Helpers::imageViewCreateInfo(const VkFormat format, const VkImage image,
	const VkImageAspectFlags aspectFlags) {
	const VkImageViewCreateInfo info{
		.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
		.pNext = nullptr,
		.image = image,
		.viewType = VK_IMAGE_VIEW_TYPE_2D,
		.format = format,
		.subresourceRange = {
			.aspectMask = aspectFlags,
			.baseMipLevel = 0,
			.levelCount = 1,
			.baseArrayLayer = 0,
			.layerCount = 1
		}
	};
	return info;
}

vk::PipelineDepthStencilStateCreateInfo Lullaby::Helpers::depthStencilCreateInfo(const bool doDepthTest, const bool depthWrite,
	const vk::CompareOp compareOp) {
	vk::PipelineDepthStencilStateCreateInfo info = {
		.sType = vk::StructureType::ePipelineDepthStencilStateCreateInfo,
		.pNext = nullptr,
		.depthTestEnable = doDepthTest ? VK_TRUE : VK_FALSE,
		.depthWriteEnable = depthWrite ? VK_TRUE : VK_FALSE,
		.depthCompareOp = doDepthTest ? compareOp : vk::CompareOp::eAlways,
		.depthBoundsTestEnable = VK_FALSE,
		.stencilTestEnable = VK_FALSE,
		.minDepthBounds = 0.0f,
		.maxDepthBounds = 1.0f
	};
	return info;
}

void Lullaby::Helpers::checkVulkanError(const vk::Result result, const std::string& when, const std::source_location& location) {
	if (result != vk::Result::eSuccess) {
		fmt::print(stderr, fg(fmt::color::red), "[{}]->Detected vulkan error while {}\n", location.function_name(), when);
	}
}

