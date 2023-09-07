#include "stdafx.h"
#include "VKInitializers.h"

VkCommandPoolCreateInfo LullabyHelpers::commandPoolCreateInfo(const uint32_t& queueFamily, const VkCommandPoolCreateFlags& vkFlags) {
	VkCommandPoolCreateInfo info = {};
	info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	info.pNext = nullptr;

	info.queueFamilyIndex = queueFamily;
	info.flags = vkFlags;
	return info;
}

VkCommandBufferAllocateInfo LullabyHelpers::commandBufferAllocateInfo(const VkCommandPool& commandPool, const uint32_t& count, const VkCommandBufferLevel level) {
	VkCommandBufferAllocateInfo info = {};
	info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	info.pNext = nullptr;

	info.commandPool = commandPool;
	info.commandBufferCount = count;
	info.level = level;
	return info;
}

VkAttachmentDescription LullabyHelpers::createAttachmentDescription(const VkFormat& format, const VkSampleCountFlagBits& samples, const VkAttachmentLoadOp& loadOp, const VkAttachmentStoreOp& storeOp, const VkAttachmentLoadOp& stencilLoadOp, const VkAttachmentStoreOp& stencilStoreOp, const VkImageLayout& initialLayout, const VkImageLayout& finalLayout) {
	VkAttachmentDescription attDesc = {};
	attDesc.format = format;
	attDesc.samples = samples;
	attDesc.loadOp = loadOp;
	attDesc.storeOp = storeOp;
	attDesc.stencilLoadOp = stencilLoadOp;
	attDesc.stencilStoreOp = stencilStoreOp;
	attDesc.initialLayout = initialLayout;
	attDesc.finalLayout = finalLayout;
	return attDesc;
}

VkRenderPassCreateInfo LullabyHelpers::createRenderPassInfo(uint32_t attachmentCount, const VkAttachmentDescription* pAttachments, uint32_t subpassCount, const VkSubpassDescription* pSubpasses, const VkStructureType& sType, uint32_t dependencyCount, const VkSubpassDependency* pDependencies) {
	VkRenderPassCreateInfo renderPassInfo = {};
	renderPassInfo.sType = sType;
	renderPassInfo.attachmentCount = attachmentCount;
	renderPassInfo.pAttachments = pAttachments;
	renderPassInfo.subpassCount = subpassCount;
	renderPassInfo.pSubpasses = pSubpasses;
	renderPassInfo.dependencyCount = dependencyCount;
	renderPassInfo.pDependencies = pDependencies;
	return renderPassInfo;
}

void LullabyHelpers::checkVulkanError(VkResult result, const std::string& when, const std::source_location& location) {
	if (result != VK_SUCCESS) {
		std::cerr << "[" << location.function_name() << "]->Detected vulkan error " << result << " while " << when << std::endl;
	}
}

