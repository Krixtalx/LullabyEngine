#include "stdafx.h"
#include "VKInitializers.h"

VkCommandPoolCreateInfo LullabyHelpers::commandPoolCreateInfo(const uint32_t& queueFamily, const VkCommandPoolCreateFlags& vkFlags) {
	const VkCommandPoolCreateInfo info = {
		.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
		.pNext = nullptr,
		.flags = vkFlags,
		.queueFamilyIndex = queueFamily
	};
	return info;
}

VkCommandBufferAllocateInfo LullabyHelpers::commandBufferAllocateInfo(const VkCommandPool& commandPool, const uint32_t& count, const VkCommandBufferLevel level) {
	const VkCommandBufferAllocateInfo info = {
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
		.pNext = nullptr,
		.commandPool = commandPool,
		.level = level,
		.commandBufferCount = count
	};
	return info;
}

VkAttachmentDescription LullabyHelpers::createAttachmentDescription(const VkFormat& format, const VkSampleCountFlagBits& samples, const VkAttachmentLoadOp& loadOp, const VkAttachmentStoreOp& storeOp, const VkAttachmentLoadOp& stencilLoadOp, const VkAttachmentStoreOp& stencilStoreOp, const VkImageLayout& initialLayout, const VkImageLayout& finalLayout) {
	const VkAttachmentDescription attDesc = {
		.format = format,
		.samples = samples,
		.loadOp = loadOp,
		.storeOp = storeOp,
		.stencilLoadOp = stencilLoadOp,
		.stencilStoreOp = stencilStoreOp,
		.initialLayout = initialLayout,
		.finalLayout = finalLayout
	};
	return attDesc;
}

VkRenderPassCreateInfo LullabyHelpers::createRenderPassInfo(uint32_t attachmentCount, const VkAttachmentDescription* pAttachments, uint32_t subpassCount, const VkSubpassDescription* pSubpasses, const VkStructureType& sType, uint32_t dependencyCount, const VkSubpassDependency* pDependencies) {
	const VkRenderPassCreateInfo renderPassInfo = {
		.sType = sType,
		.attachmentCount = attachmentCount,
		.pAttachments = pAttachments,
		.subpassCount = subpassCount,
		.pSubpasses = pSubpasses,
		.dependencyCount = dependencyCount,
		.pDependencies = pDependencies
	};
	return renderPassInfo;
}

void LullabyHelpers::checkVulkanError(const VkResult result, const std::string& when, const std::source_location& location) {
	if (result != VK_SUCCESS) {
		std::cerr << "[" << location.function_name() << "]->Detected vulkan error " << result << " while " << when << std::endl;
	}
}

