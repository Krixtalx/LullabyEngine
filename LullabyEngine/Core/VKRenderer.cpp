// ReSharper disable All
#include "stdafx.h"
#include "VKRenderer.h"
#include "Shaders\PipelineBuilder.h"
#include "VKInitializers.h"
#define VMA_IMPLEMENTATION
#include "vk_mem_alloc.h"

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
		_choosenGPU = physicalDevice.physical_device;

		// use vkbootstrap to get a Graphics queue
		_graphicsQueue = vkbDevice.get_queue(vkb::QueueType::graphics).value();
		_graphicsQueueFamily = vkbDevice.get_queue_index(vkb::QueueType::graphics).value();
		_computeQueue = vkbDevice.get_queue(vkb::QueueType::compute).value();
		_computeQueueFamily = vkbDevice.get_queue_index(vkb::QueueType::compute).value();

		VmaAllocatorCreateInfo allocatorInfo = {
			.physicalDevice = _choosenGPU,
			.device = _device,
			.preferredLargeHeapBlockSize = 0,
			.instance = _instance,
			.vulkanApiVersion = VK_API_VERSION_1_1
		};
		vmaCreateAllocator(&allocatorInfo, &_memoryAllocator);
		_mainDeletionQueue.addDeletor([=]() {vmaDestroyAllocator(_memoryAllocator); });

		_isInitialized = true;
	} else {
		std::cout << "Renderer already initialized!" << std::endl;
	}
}

void Lullaby::VKRenderer::initSwapchain(const ivec2 windowSize) {
	vkb::SwapchainBuilder swapchainBuilder{ _choosenGPU,_device,_surface };

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
	_mainDeletionQueue.addDeletor([=]() {
		vkDestroySwapchainKHR(_device, _swapchain, nullptr);
		});

	//depth image size will match the window
	VkExtent3D depthImageExtent = {
		.width = _renderResolution.x,
		.height = _renderResolution.y,
		.depth = 1
	};

	//hardcoding the depth format to 32 bit float
	_depthFormat = VK_FORMAT_D32_SFLOAT;

	//the depth image will be an image with the format we selected and Depth Attachment usage flag
	VkImageCreateInfo imgInfo = Lullaby::Helpers::imageCreateInfo(_depthFormat, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, depthImageExtent);

	//for the depth image, we want to allocate it from GPU local memory
	VmaAllocationCreateInfo imgAllocInfo = {
		.usage = VMA_MEMORY_USAGE_GPU_ONLY,
		.requiredFlags = VkMemoryPropertyFlags(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)
	};

	//allocate and create the image
	vmaCreateImage(_memoryAllocator, &imgInfo, &imgAllocInfo, &_depthImage._image, &_depthImage._allocation, nullptr);

	//build an image-view for the depth image to use for rendering
	VkImageViewCreateInfo imgViewInfo = Helpers::imageViewCreateInfo(_depthFormat, _depthImage._image, VK_IMAGE_ASPECT_DEPTH_BIT);

	Helpers::checkVulkanError(vkCreateImageView(_device, &imgViewInfo, nullptr, &_depthImageView), "creating an image view");

	//add to deletion queue
	_mainDeletionQueue.addDeletor([=]() {
		vkDestroyImageView(_device, _depthImageView, nullptr);
		vmaDestroyImage(_memoryAllocator, _depthImage._image, _depthImage._allocation);
		});
}

void Lullaby::VKRenderer::initCommands() {
	//create a command pool for commands submitted to the graphics queue.
	//the command pool will be one that can submit graphics commands
	//we also want the pool to allow for resetting of individual command buffers
	const auto commandPoolInfo = Lullaby::Helpers::commandPoolCreateInfo(_graphicsQueueFamily, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

	//Create graphics command pool
	Lullaby::Helpers::checkVulkanError(vkCreateCommandPool(_device, &commandPoolInfo, nullptr, &_graphicsCommandPool), "creating command pool");

	//allocate the default command buffer that we will use for rendering
	//commands will be made from our _graphicsCommandPool
	//we will allocate 1 command buffer
	// command level is Primary
	const auto commandAllocInfo = Lullaby::Helpers::commandBufferAllocateInfo(_graphicsCommandPool);

	Lullaby::Helpers::checkVulkanError(vkAllocateCommandBuffers(_device, &commandAllocInfo, &_mainCommandBuffer), "creating command buffer");
	_mainDeletionQueue.addDeletor([=]() {
		vkDestroyCommandPool(_device, _graphicsCommandPool, nullptr);
		});
}

void Lullaby::VKRenderer::initFramebuffers() {
	//create the framebuffers for the swapchain images. This will connect the render-pass to the images for rendering
	VkFramebufferCreateInfo fbInfo = {
		.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
		.pNext = nullptr,
		.renderPass = _mainRenderPass,
		.attachmentCount = 2,
		.width = _renderResolution.x,
		.height = _renderResolution.y,
		.layers = 1
	};

	//grab how many images we have in the swapchain
	const uint32_t swapchainImageCount = _swapchainImages.size();
	_framebuffers = std::vector<VkFramebuffer>(swapchainImageCount);

	//create framebuffers for each of the swapchain image views
	for (int i = 0; i < swapchainImageCount; i++) {
		VkImageView attachments[2] = { _swapchainImageViews[i], _depthImageView };
		fbInfo.pAttachments = attachments;
		Lullaby::Helpers::checkVulkanError(vkCreateFramebuffer(_device, &fbInfo, nullptr, &_framebuffers[i]), "creating a framebuffer");
		_mainDeletionQueue.addDeletor([=]() {
			vkDestroyFramebuffer(_device, _framebuffers[i], nullptr);
			vkDestroyImageView(_device, _swapchainImageViews[i], nullptr);
			});
	}

}

void Lullaby::VKRenderer::initDefaultRenderpass() {
	// the renderpass will use this color attachment.		
	const VkAttachmentDescription colorAttachment = {
		.format = _swapchainImageFormat, //the attachment will have the format needed by the swapchain
		.samples = VK_SAMPLE_COUNT_1_BIT,//1 sample, we won't be doing MSAA for now
		.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR, // we Clear when this attachment is loaded
		.storeOp = VK_ATTACHMENT_STORE_OP_STORE, // we keep the attachment stored when the renderpass ends
		.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE, //we don't care about stencil	
		.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
		.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED, //we don't know nor care about the starting layout of the attachment
		.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR //after the renderpass ends, the image has to be on a layout ready for display
	};

	const VkAttachmentReference colorAttachmentRef = {
		//attachment number will index into the pAttachments array in the parent renderpass itself
		.attachment = 0,
		.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
	};

	// Depth render pass description.
	const VkAttachmentDescription depthAttachment{
		.flags = 0,
		.format = _depthFormat,
		.samples = VK_SAMPLE_COUNT_1_BIT,
		.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
		.storeOp = VK_ATTACHMENT_STORE_OP_STORE,
		.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
		.stencilStoreOp = VK_ATTACHMENT_STORE_OP_STORE,
		.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
		.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
	};

	const VkAttachmentReference depthAttachmentRef = {
		//attachment number will index into the pAttachments array in the parent renderpass itself
		.attachment = 1,
		.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
	};


	VkSubpassDescription subpass = {
		.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
		.colorAttachmentCount = 1,
		.pColorAttachments = &colorAttachmentRef,
		.pDepthStencilAttachment = &depthAttachmentRef
	};
	//array of 2 attachments, one for the color, and other for depth
	VkAttachmentDescription attachments[2] = { colorAttachment, depthAttachment };

	VkSubpassDependency colorDependency = {
		.srcSubpass = VK_SUBPASS_EXTERNAL,
		.dstSubpass = 0,
		.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		.srcAccessMask = 0,
		.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT
	};

	VkSubpassDependency depthDependency = {
		.srcSubpass = VK_SUBPASS_EXTERNAL,
		.dstSubpass = 0,
		.srcStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
		.dstStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
		.srcAccessMask = 0,
		.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT
	};

	VkSubpassDependency dependencies[2] = { colorDependency, depthDependency };

	const VkRenderPassCreateInfo renderPassInfo{
		.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
		.attachmentCount = 2,
		.pAttachments = attachments,
		.subpassCount = 1,
		.pSubpasses = &subpass,
		.dependencyCount = 2,
		.pDependencies = dependencies
	};

	Helpers::checkVulkanError(vkCreateRenderPass(_device, &renderPassInfo, nullptr, &_mainRenderPass), "creating the main render pass");

	_mainDeletionQueue.addDeletor([=]() {
		vkDestroyRenderPass(_device, _mainRenderPass, nullptr);
		});
}

void Lullaby::VKRenderer::initSyncStructures() {
	//create synchronization structures

	VkFenceCreateInfo fenceCreateInfo = {
		.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
		.pNext = nullptr,
		//we want to create the fence with the Create Signaled flag, so we can wait on it before using it on a GPU command (for the first frame)
		.flags = VK_FENCE_CREATE_SIGNALED_BIT
	};

	Lullaby::Helpers::checkVulkanError(vkCreateFence(_device, &fenceCreateInfo, nullptr, &_renderFence), "creating the main render fence");

	//for the semaphores we don't need any flags
	const VkSemaphoreCreateInfo semaphoreCreateInfo = {
		.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0
	};
	Lullaby::Helpers::checkVulkanError(vkCreateSemaphore(_device, &semaphoreCreateInfo, nullptr, &_presentSemaphore), "creating presentation semaphore");
	Lullaby::Helpers::checkVulkanError(vkCreateSemaphore(_device, &semaphoreCreateInfo, nullptr, &_renderSemaphore), "creating rendering semaphore");
	_mainDeletionQueue.addDeletor([=]() {
		vkDestroySemaphore(_device, _presentSemaphore, nullptr);
		vkDestroySemaphore(_device, _renderSemaphore, nullptr);
		});
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

	//we start from just the default empty pipeline layout info
	auto layoutInfo = PipelineBuilder::defaultLayoutInfo();

	//setup push constants
	VkPushConstantRange pushConstant{};
	//this push constant range starts at the beginning
	pushConstant.offset = 0;
	//this push constant range takes up the size of a MeshPushConstants struct
	pushConstant.size = sizeof(MeshPushConstants);
	//this push constant range is accessible only in the vertex shader
	pushConstant.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

	layoutInfo.pPushConstantRanges = &pushConstant;
	layoutInfo.pushConstantRangeCount = 1;

	Lullaby::Helpers::checkVulkanError(vkCreatePipelineLayout(_device, &layoutInfo, nullptr, &_meshPipelineLayout), "creating pipeline layout");

	auto vertexInputInfo = PipelineBuilder::defaultVertexInputInfo();
	auto vertexDescription = Vertex::getVertexDescription();
	vertexInputInfo.pVertexBindingDescriptions = vertexDescription._bindings.data();
	vertexInputInfo.vertexBindingDescriptionCount = vertexDescription._bindings.size();
	vertexInputInfo.pVertexAttributeDescriptions = vertexDescription._attributes.data();
	vertexInputInfo.vertexAttributeDescriptionCount = vertexDescription._attributes.size();

	PipelineInfo info{
		._shaderStages = {
			PipelineBuilder::defaultShaderStageInfo(VK_SHADER_STAGE_VERTEX_BIT, triangleVertexShader),
			PipelineBuilder::defaultShaderStageInfo(VK_SHADER_STAGE_FRAGMENT_BIT, triangleFragShader)
		},
		._vertexInputInfo = vertexInputInfo,
		._inputAssembly = PipelineBuilder::defaultInputAssemblyInfo(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST),
		._viewport = {
			.x = .0f,
			.y = .0f,
			.width = (float)_renderResolution.x,
			.height = (float)_renderResolution.y,
			.minDepth = .0f,
			.maxDepth = 1.0f},
		._scissor = {
			.offset = {0,0},
			.extent = {_renderResolution.x, _renderResolution.y}
		},
		._rasterizer = PipelineBuilder::defaultRasterizationInfo(VK_POLYGON_MODE_FILL),
		._colorBlendAttachment = PipelineBuilder::defaultColorBlendState(),
		._depthStencil = Helpers::depthStencilCreateInfo(true, true, VK_COMPARE_OP_LESS_OR_EQUAL),
		._multisampling = PipelineBuilder::defaultMultisampleInfo(),
		._pipelineLayout = _meshPipelineLayout
	};
	_meshPipeline = PipelineBuilder::buildPipeline(info, _device, _mainRenderPass);

	vkDestroyShaderModule(_device, triangleVertexShader, nullptr);
	vkDestroyShaderModule(_device, triangleFragShader, nullptr);
	_mainDeletionQueue.addDeletor([=]() {
		//destroy the 2 pipelines we have created
		vkDestroyPipeline(_device, _meshPipeline, nullptr);

		//destroy the pipeline layout that they use
		vkDestroyPipelineLayout(_device, _meshPipelineLayout, nullptr);
		});
}

void Lullaby::VKRenderer::render() {
	//wait until the GPU has finished rendering the last frame. Timeout of 1 second
	Lullaby::Helpers::checkVulkanError(vkWaitForFences(_device, 1, &_renderFence, true, 1000000000), "waiting for render fence");
	Lullaby::Helpers::checkVulkanError(vkResetFences(_device, 1, &_renderFence), "reseting render fence");

	//request image from the swapchain, one second timeout
	uint32_t swapchainImageIndex;
	Lullaby::Helpers::checkVulkanError(vkAcquireNextImageKHR(_device, _swapchain, 1000000000, _presentSemaphore, nullptr, &swapchainImageIndex), "acquiring next image from swapchain");

	Lullaby::Helpers::checkVulkanError(vkResetCommandBuffer(_mainCommandBuffer, 0), "reseting command buffer");

	//Begin the command buffer recording. We will use this command buffer exactly once, so we want to let Vulkan know that
	const VkCommandBufferBeginInfo cmdBeginInfo = {
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
		.pNext = nullptr,
		.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
		.pInheritanceInfo = nullptr
	};

	Lullaby::Helpers::checkVulkanError(vkBeginCommandBuffer(_mainCommandBuffer, &cmdBeginInfo), "beginning command buffer recording");

	VkClearValue clearColor;
	clearColor.color = { { 0.05f, 0.05f, 0.05f, 1.0f } };

	VkClearValue clearDepth;
	clearDepth.depthStencil.depth = 1.f;

	VkClearValue clears[2] = { clearColor, clearDepth };

	//start the main renderpass.
	//We will use the clear color from above, and the framebuffer of the index the swapchain gave us
	const VkRenderPassBeginInfo rpInfo = {
		.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
		.pNext = nullptr,
		.renderPass = _mainRenderPass,
		.framebuffer = _framebuffers[swapchainImageIndex],
		.renderArea = {
			.offset = {0,0},
			.extent = {_renderResolution.x, _renderResolution.y}
		},
		.clearValueCount = 2,
		.pClearValues = clears,
	};

	vkCmdBeginRenderPass(_mainCommandBuffer, &rpInfo, VK_SUBPASS_CONTENTS_INLINE);

	vkCmdBindPipeline(_mainCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, _meshPipeline);

	//make a model view matrix for rendering the object
	//camera position
	glm::vec3 camPos = { 0.f,-5.f,-20.f };

	glm::mat4 view = glm::translate(glm::mat4(1.f), camPos);
	//camera projection
	glm::mat4 projection = glm::perspective(glm::radians(70.f), 1700.f / 900.f, 0.1f, 200.0f);
	projection[1][1] *= -1;
	//model rotation
	glm::mat4 model = glm::rotate(glm::mat4{ 1.0f }, glm::radians(++_frameNumber * 0.4f), glm::vec3(0, 1, 0));

	//calculate final mesh matrix
	glm::mat4 mesh_matrix = projection * view * model;

	MeshPushConstants constants;
	constants.render_matrix = mesh_matrix;

	//upload the matrix to the GPU via push constants
	vkCmdPushConstants(_mainCommandBuffer, _meshPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(MeshPushConstants), &constants);

	//bind the mesh vertex buffer with offset 0
	VkDeviceSize offset = 0;
	/*vkCmdBindVertexBuffers(_mainCommandBuffer, 0, 1, &_sampleMesh._vertexBuffer._buffer, &offset);
	vkCmdDraw(_mainCommandBuffer, _sampleMesh._vertices.size(), 1, 0, 0);*/

	vkCmdBindVertexBuffers(_mainCommandBuffer, 0, 1, &_dragon._vertexBuffer._buffer, &offset);
	vkCmdDraw(_mainCommandBuffer, _dragon._vertices.size(), 1, 0, 0);

	//finalize the render pass
	vkCmdEndRenderPass(_mainCommandBuffer);
	Lullaby::Helpers::checkVulkanError(vkEndCommandBuffer(_mainCommandBuffer), "ending command buffer");

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
	Lullaby::Helpers::checkVulkanError(vkQueueSubmit(_graphicsQueue, 1, &submit, _renderFence), "submitting  work to graphics queue");

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
	Lullaby::Helpers::checkVulkanError(vkQueuePresentKHR(_graphicsQueue, &presentInfo), "presenting image");

}

void Lullaby::VKRenderer::sampleTriangle() {//make the array 3 vertices long
	_sampleMesh._vertices.resize(3);

	//vertex positions
	_sampleMesh._vertices[0]._position = { 1.f, 1.f, 0.0f };
	_sampleMesh._vertices[1]._position = { -1.f, 1.f, 0.0f };
	_sampleMesh._vertices[2]._position = { 0.f,-1.f, 0.0f };

	//vertex colors, all green
	_sampleMesh._vertices[0]._color = { 0.f, 1.f, 0.0f }; //pure green
	_sampleMesh._vertices[1]._color = { 0.f, 1.f, 0.0f }; //pure green
	_sampleMesh._vertices[2]._color = { 0.f, 1.f, 0.0f }; //pure green

	uploadGeometry(_sampleMesh);
}

void Lullaby::VKRenderer::sampleModel() {
	_dragon.loadFromObj("../Assets/Models/Dragon.obj");
	uploadGeometry(_dragon);
}

void Lullaby::VKRenderer::uploadGeometry(Mesh& mesh) {
	//allocate vertex buffer
	VkBufferCreateInfo bufferInfo = {
		.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
		.size = mesh._vertices.size() * sizeof(Vertex),
		.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
		.sharingMode = VK_SHARING_MODE_EXCLUSIVE
	};

	//let the VMA library know that this data should be writeable by CPU, but also readable by GPU
	VmaAllocationCreateInfo vmaallocInfo = {
		.usage = VMA_MEMORY_USAGE_CPU_TO_GPU,
	};

	//allocate the buffer
	Lullaby::Helpers::checkVulkanError(vmaCreateBuffer(_memoryAllocator, &bufferInfo, &vmaallocInfo,
		&mesh._vertexBuffer._buffer,
		&mesh._vertexBuffer._allocation,
		nullptr), "creating a memory buffer in GPU");

	//add the destruction of triangle mesh buffer to the deletion queue
	_mainDeletionQueue.addDeletor([=]() {
		vmaDestroyBuffer(_memoryAllocator, mesh._vertexBuffer._buffer, mesh._vertexBuffer._allocation);
		});

	void* data;
	vmaMapMemory(_memoryAllocator, mesh._vertexBuffer._allocation, &data);

	memcpy(data, mesh._vertices.data(), mesh._vertices.size() * sizeof(Vertex));

	vmaUnmapMemory(_memoryAllocator, mesh._vertexBuffer._allocation);
}

void Lullaby::VKRenderer::releaseResources() {
	if (_isInitialized) {
		//make sure the GPU has stopped doing its things
		vkWaitForFences(_device, 1, &_renderFence, true, 1000000000);

		_mainDeletionQueue.flush();

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
