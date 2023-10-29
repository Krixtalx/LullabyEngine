#include "stdafx.h"
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

VkAttachmentDescription Lullaby::Helpers::createAttachmentDescription(const VkFormat& format, const VkSampleCountFlagBits& samples, const VkAttachmentLoadOp& loadOp, const VkAttachmentStoreOp& storeOp, const VkAttachmentLoadOp& stencilLoadOp, const VkAttachmentStoreOp& stencilStoreOp, const VkImageLayout& initialLayout, const VkImageLayout& finalLayout) {
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

VkRenderPassCreateInfo Lullaby::Helpers::createRenderPassInfo(uint32_t attachmentCount, const VkAttachmentDescription* pAttachments, uint32_t subpassCount, const VkSubpassDescription* pSubpasses, const VkStructureType& sType, uint32_t dependencyCount, const VkSubpassDependency* pDependencies) {
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

VkImageCreateInfo Lullaby::Helpers::imageCreateInfo(VkFormat format, VkImageUsageFlags usageFlags, VkExtent3D extent) {
	VkImageCreateInfo info{
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

VkImageViewCreateInfo Lullaby::Helpers::imageViewCreateInfo(VkFormat format, VkImage image,
	VkImageAspectFlags aspectFlags) {
	VkImageViewCreateInfo info{
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

void Lullaby::Helpers::checkVulkanError(const VkResult result, const std::string& when, const std::source_location& location) {
	if (result != VK_SUCCESS) {
		std::cerr << "[" << location.function_name() << "]->Detected vulkan error " << result << " while " << when << std::endl;
	}
}

