#pragma once
#include <source_location>

namespace LullabyHelpers {
	VkCommandPoolCreateInfo commandPoolCreateInfo(const uint32_t& queueFamily, const VkCommandPoolCreateFlags& vkFlags = {});
	VkCommandBufferAllocateInfo commandBufferAllocateInfo(const VkCommandPool& commandPool, const uint32_t& count = 1, const VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY);
	VkAttachmentDescription createAttachmentDescription(const VkFormat& format, const VkSampleCountFlagBits& samples, const VkAttachmentLoadOp& loadOp, const VkAttachmentStoreOp& storeOp, const VkAttachmentLoadOp& stencilLoadOp, const VkAttachmentStoreOp& stencilStoreOp, const VkImageLayout& initialLayout, const VkImageLayout& finalLayout);
	VkRenderPassCreateInfo createRenderPassInfo(uint32_t attachmentCount, const VkAttachmentDescription* pAttachments, uint32_t subpassCount, const VkSubpassDescription* pSubpasses, const VkStructureType& sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO, uint32_t dependencyCount = 0, const VkSubpassDependency* pDependencies = nullptr); //const VkRenderPassCreateFlags& flags,

	void checkVulkanError(VkResult result, const std::string& when, const std::source_location& location = std::source_location::current());
}