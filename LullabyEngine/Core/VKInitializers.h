#pragma once
#include <source_location>

namespace Lullaby::Helpers {
	VkCommandPoolCreateInfo commandPoolCreateInfo(const uint32_t& queueFamily, const VkCommandPoolCreateFlags& vkFlags = {});
	VkCommandBufferAllocateInfo commandBufferAllocateInfo(const VkCommandPool& commandPool, const uint32_t& count = 1, const VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY);
	VkImageCreateInfo imageCreateInfo(VkFormat format, VkImageUsageFlags usageFlags, VkExtent3D extent);
	VkImageViewCreateInfo imageViewCreateInfo(VkFormat format, VkImage image, VkImageAspectFlags aspectFlags);
	VkPipelineDepthStencilStateCreateInfo depthStencilCreateInfo(bool doDepthTest, bool depthWrite, VkCompareOp compareOp);

	void checkVulkanError(const vk::Result result, const std::string& when, const std::source_location& location = std::source_location::current());
}