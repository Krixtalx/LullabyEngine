#pragma once
#include <source_location>

namespace Lullaby::Helpers {
	VkCommandPoolCreateInfo commandPoolCreateInfo(const u32& queueFamily, const VkCommandPoolCreateFlags& vkFlags = {});
	VkCommandBufferAllocateInfo commandBufferAllocateInfo(const VkCommandPool& commandPool, const u32& count = 1, const VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY);
	VkImageCreateInfo imageCreateInfo(VkFormat format, VkImageUsageFlags usageFlags, VkExtent3D extent);
	VkImageViewCreateInfo imageViewCreateInfo(VkFormat format, VkImage image, VkImageAspectFlags aspectFlags);
	vk::PipelineDepthStencilStateCreateInfo depthStencilCreateInfo(bool doDepthTest, bool depthWrite, vk::CompareOp compareOp);

	void checkVulkanError(const vk::Result result, const std::string& when, const std::source_location& location = std::source_location::current());
}