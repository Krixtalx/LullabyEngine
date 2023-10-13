#include "stdafx.h"
#include "VKRenderer.h"

#include "PipelineBuilder.h"
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
		//We want a GPU that can write to the GLFW surface and supports Vulkan 1.1
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
	_renderResolution = { vkbSwapchain.extent.width, vkbSwapchain.extent.height };
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
	VkFramebufferCreateInfo fb_info = {
		.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
		.pNext = nullptr
	};

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

	VkFenceCreateInfo fenceCreateInfo = {
		.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
		.pNext = nullptr,
		//we want to create the fence with the Create Signaled flag, so we can wait on it before using it on a GPU command (for the first frame)
		.flags = VK_FENCE_CREATE_SIGNALED_BIT
	};

	LullabyHelpers::checkVulkanError(vkCreateFence(_device, &fenceCreateInfo, nullptr, &_renderFence), "creating the main render fence");

	//for the semaphores we don't need any flags
	const VkSemaphoreCreateInfo semaphoreCreateInfo = {
		.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0
	};
	LullabyHelpers::checkVulkanError(vkCreateSemaphore(_device, &semaphoreCreateInfo, nullptr, &_presentSemaphore), "creating presentation semaphore");
	LullabyHelpers::checkVulkanError(vkCreateSemaphore(_device, &semaphoreCreateInfo, nullptr, &_renderSemaphore), "creating rendering semaphore");
}

void Lullaby::VKRenderer::initPipelines() {
	const VkShaderModule triangleVertexShader = PipelineBuilder::loadShaderModule(
		_device, "Shaders/TriangleVertex.spv");

	if (triangleVertexShader)
		std::cout << "Triangle vertex shader successfully loaded" << std::endl;
	else
		std::cerr << "Error when building the triangle vertex shader module" << std::endl;


	const VkShaderModule triangleFragShader = PipelineBuilder::loadShaderModule(
		_device, "Shaders/TriangleFragment.spv");

	if (triangleFragShader)
		std::cout << "Triangle fragment shader successfully loaded" << std::endl;
	else
		std::cerr << "Error when building the triangle fragment shader module" << std::endl;

	const auto layoutInfo = PipelineBuilder::defaultLayoutInfo();
	LullabyHelpers::checkVulkanError(vkCreatePipelineLayout(_device, &layoutInfo, nullptr, &_trianglePipelineLayout), "creating pipeline layout");

	PipelineInfo info{
		._shaderStages = {
			PipelineBuilder::defaultShaderStageInfo(VK_SHADER_STAGE_VERTEX_BIT, triangleVertexShader),
			PipelineBuilder::defaultShaderStageInfo(VK_SHADER_STAGE_FRAGMENT_BIT, triangleFragShader)
		},
		._vertexInputInfo = PipelineBuilder::defaultVertexInputInfo(),
		._inputAssembly = PipelineBuilder::defaultInputAssemblyInfo(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST),
		._viewport = {.x = .0f, .y = .0f, .width = (float)_renderResolution.x, .height = (float)_renderResolution.y, .minDepth = .0f, .maxDepth = 1.0f},
		._scissor = {.offset = {0,0}, .extent = {_renderResolution.x, _renderResolution.y}},
		._rasterizer = PipelineBuilder::defaultRasterizationInfo(VK_POLYGON_MODE_FILL),
		._colorBlendAttachment = PipelineBuilder::defaultColorBlendState(),
		._multisampling = PipelineBuilder::defaultMultisampleInfo(),
		._pipelineLayout = _trianglePipelineLayout
	};
	_trianglePipeline = PipelineBuilder::buildPipeline(info, _device, _mainRenderPass);
}

void Lullaby::VKRenderer::render() {
	//wait until the GPU has finished rendering the last frame. Timeout of 1 second
	LullabyHelpers::checkVulkanError(vkWaitForFences(_device, 1, &_renderFence, true, 1000000000), "waiting for render fence");
	LullabyHelpers::checkVulkanError(vkResetFences(_device, 1, &_renderFence), "reseting render fence");

	//request image from the swapchain, one second timeout
	uint32_t swapchainImageIndex;
	LullabyHelpers::checkVulkanError(vkAcquireNextImageKHR(_device, _swapchain, 1000000000, _presentSemaphore, nullptr, &swapchainImageIndex), "acquiring next image from swapchain");

	LullabyHelpers::checkVulkanError(vkResetCommandBuffer(_mainCommandBuffer, 0), "reseting command buffer");

	//Begin the command buffer recording. We will use this command buffer exactly once, so we want to let Vulkan know that
	const VkCommandBufferBeginInfo cmdBeginInfo = {
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
		.pNext = nullptr,
		.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
		.pInheritanceInfo = nullptr
	};

	LullabyHelpers::checkVulkanError(vkBeginCommandBuffer(_mainCommandBuffer, &cmdBeginInfo), "beginning command buffer recording");

	//make a clear-color from frame number. This will flash with a 120*pi frame period.
	VkClearValue clearValue;
	const float flash = abs(sin(_frameNumber++ / 120.f));
	clearValue.color = { { flash, 0.0f, flash, 1.0f } };

	//start the main renderpass.
	//We will use the clear color from above, and the framebuffer of the index the swapchain gave us
	const VkRenderPassBeginInfo rpInfo = {
		.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
		.pNext = nullptr,
		.renderPass = _mainRenderPass,
		.framebuffer = _framebuffers[swapchainImageIndex],
		.renderArea = {.offset = {0,0}, .extent = {_renderResolution.x, _renderResolution.y}},
		.clearValueCount = 1,
		.pClearValues = &clearValue
	};

	vkCmdBeginRenderPass(_mainCommandBuffer, &rpInfo, VK_SUBPASS_CONTENTS_INLINE);

	vkCmdBindPipeline(_mainCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, _trianglePipeline);
	vkCmdDraw(_mainCommandBuffer, 3, 1, 0, 0);

	//finalize the render pass
	vkCmdEndRenderPass(_mainCommandBuffer);
	LullabyHelpers::checkVulkanError(vkEndCommandBuffer(_mainCommandBuffer), "ending command buffer");

	//prepare the submission to the queue.
	//we want to wait on the _presentSemaphore, as that semaphore is signaled when the swapchain is ready
	//we will signal the _renderSemaphore, to signal that rendering has finished
	VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	const VkSubmitInfo submit = {
		.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
		.pNext = nullptr,
		.waitSemaphoreCount = 1,
		.pWaitSemaphores = &_presentSemaphore,
		.pWaitDstStageMask = &waitStage,
		.commandBufferCount = 1,
		.pCommandBuffers = &_mainCommandBuffer,
		.signalSemaphoreCount = 1,
		.pSignalSemaphores = &_renderSemaphore,
	};
	LullabyHelpers::checkVulkanError(vkQueueSubmit(_graphicsQueue, 1, &submit, _renderFence), "submitting  work to graphics queue");

	// this will put the image we just rendered into the visible window.
	// we want to wait on the _renderSemaphore for that,
	// as it's necessary that drawing commands have finished before the image is displayed to the user
	const VkPresentInfoKHR presentInfo = {
		.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
		.pNext = nullptr,
		.waitSemaphoreCount = 1,
		.pWaitSemaphores = &_renderSemaphore,
		.swapchainCount = 1,
		.pSwapchains = &_swapchain,
		.pImageIndices = &swapchainImageIndex,
	};
	LullabyHelpers::checkVulkanError(vkQueuePresentKHR(_graphicsQueue, &presentInfo), "presenting image");

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
