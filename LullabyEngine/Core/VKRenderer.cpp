#include "stdafx.h"
#include "VKRenderer.h"
#include "VKInitializers.h"

void Lullaby::VKRenderer::initRenderer(GLFWwindow* window) {
	if (!_isInitialized) {
		/*if (volkInitialize() != VK_SUCCESS) {
			throw std::runtime_error("Problem while initializing Volk");
		}*/
		vkb::InstanceBuilder builder;

		//make the Vulkan instance, with basic debug features
		auto instanceRet = builder.set_app_name("Lullaby Engine")
			.request_validation_layers(true)
			.require_api_version(1, 1, 0)
			.use_default_debug_messenger()
			.build();

		vkb::Instance vkbInstance = instanceRet.value();

		//store the instance
		_instance = vkbInstance.instance;
		/*volkLoadInstance(_instance);*/
		//store the debug messenger
		_debugMesseger = vkbInstance.debug_messenger;

		//use vkbootstrap to select a GPU.
		//We want a GPU that can write to the SDL surface and supports Vulkan 1.1
		vkb::PhysicalDeviceSelector selector(vkbInstance);
		vkb::PhysicalDevice physicalDevice;

		if (window) {
			// get the surface of the window we opened with GLFW
			glfwCreateWindowSurface(_instance, window, nullptr, &_surface);

			physicalDevice = selector
				.set_minimum_version(1, 1)
				.set_surface(_surface)
				.select()
				.value();
		} else {
			physicalDevice = selector
				.set_minimum_version(1, 1)
				.select()
				.value();
		}

		//create the final Vulkan device
		vkb::DeviceBuilder deviceBuilder{ physicalDevice };

		vkb::Device vkbDevice = deviceBuilder.build().value();

		// Get the VkDevice handle used in the rest of a Vulkan application
		_device = vkbDevice.device;
		/*volkLoadDevice(_device);*/
		_chosenGPU = physicalDevice.physical_device;

		// use vkbootstrap to get a Graphics queue
		_graphicsQueue = vkbDevice.get_queue(vkb::QueueType::graphics).value();
		_graphicsQueueFamily = vkbDevice.get_queue_index(vkb::QueueType::graphics).value();
		_computeQueue = vkbDevice.get_queue(vkb::QueueType::compute).value();
		_computeQueueFamily = vkbDevice.get_queue_index(vkb::QueueType::compute).value();

		_isInitialized = true;
	} else {
		std::cout << "Renderer already initialized!" << std::endl;
	}
}

void Lullaby::VKRenderer::initSwapchain(const ivec2 windowSize) {
	_renderResolution = windowSize;
	vkb::SwapchainBuilder swapchainBuilder{ _chosenGPU,_device,_surface };

	vkb::Swapchain vkbSwapchain = swapchainBuilder
		.use_default_format_selection()
		//use vsync present mode
		.set_desired_present_mode(VK_PRESENT_MODE_FIFO_KHR)
		.set_desired_extent(windowSize.x, windowSize.y)
		.build()
		.value();

	//store swapchain and its related images
	_swapchain = vkbSwapchain.swapchain;
	_swapchainImages = vkbSwapchain.get_images().value();
	_swapchainImageViews = vkbSwapchain.get_image_views().value();

	_swapchainImageFormat = vkbSwapchain.image_format;
}

void Lullaby::VKRenderer::initCommands() {
	//create a command pool for commands submitted to the graphics queue.
	//the command pool will be one that can submit graphics commands
	//we also want the pool to allow for resetting of individual command buffers
	const auto commandPoolInfo = LullabyHelpers::commandPoolCreateInfo(_graphicsQueueFamily, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

	//Create graphics command pool
	LullabyHelpers::checkVulkanError(vkCreateCommandPool(_device, &commandPoolInfo, nullptr, &_graphicsCommandPool), "creating command pool");

	//allocate the default command buffer that we will use for rendering
	//commands will be made from our _graphicsCommandPool
	//we will allocate 1 command buffer
	// command level is Primary
	const auto commandAllocInfo = LullabyHelpers::commandBufferAllocateInfo(_graphicsCommandPool);

	LullabyHelpers::checkVulkanError(vkAllocateCommandBuffers(_device, &commandAllocInfo, &_mainCommandBuffer), "creating command buffer");
}

void Lullaby::VKRenderer::initFramebuffers() {
	//create the framebuffers for the swapchain images. This will connect the render-pass to the images for rendering
	VkFramebufferCreateInfo fb_info = {};
	fb_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	fb_info.pNext = nullptr;

	fb_info.renderPass = _mainRenderPass;
	fb_info.attachmentCount = 1;
	fb_info.width = _renderResolution.x;
	fb_info.height = _renderResolution.y;
	fb_info.layers = 1;

	//grab how many images we have in the swapchain
	const uint32_t swapchain_imagecount = _swapchainImages.size();
	_framebuffers = std::vector<VkFramebuffer>(swapchain_imagecount);

	//create framebuffers for each of the swapchain image views
	for (int i = 0; i < swapchain_imagecount; i++) {
		fb_info.pAttachments = &_swapchainImageViews[i];
		LullabyHelpers::checkVulkanError(vkCreateFramebuffer(_device, &fb_info, nullptr, &_framebuffers[i]), "creating a framebuffer");
	}
}

void Lullaby::VKRenderer::initDefaultRenderpass() {
	// the renderpass will use this color attachment.
	const auto colorAttachment = LullabyHelpers::createAttachmentDescription(
		_swapchainImageFormat, //the attachment will have the format needed by the swapchain
		VK_SAMPLE_COUNT_1_BIT, //1 sample, we won't be doing MSAA for now
		VK_ATTACHMENT_LOAD_OP_CLEAR, // we Clear when this attachment is loaded
		VK_ATTACHMENT_STORE_OP_STORE, // we keep the attachment stored when the renderpass ends
		VK_ATTACHMENT_LOAD_OP_DONT_CARE, //we don't care about stencil		
		VK_ATTACHMENT_STORE_OP_DONT_CARE,
		VK_IMAGE_LAYOUT_UNDEFINED, //we don't know nor care about the starting layout of the attachment
		VK_IMAGE_LAYOUT_PRESENT_SRC_KHR); //after the renderpass ends, the image has to be on a layout ready for display

	VkAttachmentReference color_attachment_ref = {};
	//attachment number will index into the pAttachments array in the parent renderpass itself
	color_attachment_ref.attachment = 0;
	color_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	//we are going to create 1 subpass, which is the minimum you can do
	VkSubpassDescription subpass = {};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &color_attachment_ref;

	const auto renderPassInfo = LullabyHelpers::createRenderPassInfo(1, &colorAttachment, 1, &subpass);
	LullabyHelpers::checkVulkanError(vkCreateRenderPass(_device, &renderPassInfo, nullptr, &_mainRenderPass), "creating the main render pass");
}

void Lullaby::VKRenderer::initSyncStructures() {
	//create synchronization structures

	VkFenceCreateInfo fenceCreateInfo = {};
	fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceCreateInfo.pNext = nullptr;

	//we want to create the fence with the Create Signaled flag, so we can wait on it before using it on a GPU command (for the first frame)
	fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	LullabyHelpers::checkVulkanError(vkCreateFence(_device, &fenceCreateInfo, nullptr, &_renderFence), "creating the main render fence");

	//for the semaphores we don't need any flags
	VkSemaphoreCreateInfo semaphoreCreateInfo = {};
	semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	semaphoreCreateInfo.pNext = nullptr;
	semaphoreCreateInfo.flags = 0;
	LullabyHelpers::checkVulkanError(vkCreateSemaphore(_device, &semaphoreCreateInfo, nullptr, &_presentSemaphore), "creating presentation semaphore");
	LullabyHelpers::checkVulkanError(vkCreateSemaphore(_device, &semaphoreCreateInfo, nullptr, &_renderSemaphore), "creating rendering semaphore");
}

void Lullaby::VKRenderer::render() {
	LullabyHelpers::checkVulkanError(vkWaitForFences(_device, 1, &_renderFence, true, 1000000000), "waiting for render fence");
	LullabyHelpers::checkVulkanError(vkResetFences(_device, 1, &_renderFence), "reseting render fence");
}

void Lullaby::VKRenderer::releaseResources() const {
	if (_isInitialized) {
		//You should always destroy the objects in the opposite way they are created
		vkDestroySwapchainKHR(_device, _swapchain, nullptr);

		for (int i = 0; i < _swapchainImages.size(); i++) {
			vkDestroyFramebuffer(_device, _framebuffers[i], nullptr);
		}

		vkDestroyRenderPass(_device, _mainRenderPass, nullptr);

		vkDestroyCommandPool(_device, _graphicsCommandPool, nullptr);

		vkDestroySwapchainKHR(_device, _swapchain, nullptr);

		//destroy swapchain resources
		for (const auto _swapchainImageView : _swapchainImageViews) {
			vkDestroyImageView(_device, _swapchainImageView, nullptr);
		}

		vkDestroyDevice(_device, nullptr);
		//VkPhysicalDevice is not a resource per-se, as it's a handle to system GPU. So, you can't destroy it.
		vkDestroySurfaceKHR(_instance, _surface, nullptr);
		vkb::destroy_debug_utils_messenger(_instance, _debugMesseger);
		vkDestroyInstance(_instance, nullptr);
	} else {
		std::cout << "The renderer is not initialized, so you can't release resources" << std::endl;
	}
}

Lullaby::VKRenderer::~VKRenderer() {
	releaseResources();
}
