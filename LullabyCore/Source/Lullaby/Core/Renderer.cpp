// ReSharper disable All
#include "LullabyPch.h"
#define VMA_IMPLEMENTATION
#include "Renderer.h"
#include "VkBootstrap.h"
#include "VKInitializers.h"

#include "DataContainers/Mesh.h"
#include "Lullaby/ECS/Components/Transform.h"
#include "Shaders\PipelineBuilder.h"


void Lullaby::Renderer::initRenderer(GLFWwindow* window) {
	if (!_isInitialized) {
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

		//store the debug messenger
		_debugMesseger = vkbInstance.debug_messenger;

		//use vkbootstrap to select a GPU.
		//We want a GPU that can write to the GLFW surface and supports Vulkan 1.1
		vkb::PhysicalDeviceSelector selector(vkbInstance);
		vkb::PhysicalDevice physicalDevice;

		if (window) {
			// get the surface of the window we opened with GLFW
			VkSurfaceKHR surface;
			glfwCreateWindowSurface(_instance, window, nullptr, &surface);
			_surface = surface;
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

		vma::AllocatorCreateInfo allocatorInfo = {
			.physicalDevice = _choosenGPU,
			.device = _device,
			.preferredLargeHeapBlockSize = 0,
			.instance = _instance,
			.vulkanApiVersion = VK_API_VERSION_1_1
		};
		vma::createAllocator(&allocatorInfo, &_memoryAllocator);
		_mainDeletionQueue.addDeletor([=]() {vmaDestroyAllocator(_memoryAllocator); });

		_isInitialized = true;
	} else {
		fmt::print(fg(fmt::color::yellow), "Renderer already initialized!\n");
	}
}

void Lullaby::Renderer::initSwapchain(const ivec2 windowSize, bool addDeletors) {
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
	auto tempSwapchainImgs = vkbSwapchain.get_images().value();
	_swapchainImages.clear();
	_swapchainImages.reserve(tempSwapchainImgs.size());
	for (auto& img : tempSwapchainImgs) {
		_swapchainImages.push_back(img);
	}

	auto tempSwapchainImgViews = vkbSwapchain.get_image_views().value();
	_swapchainImageViews.clear();
	_swapchainImageViews.reserve(tempSwapchainImgViews.size());
	for (auto& imgView : tempSwapchainImgViews) {
		_swapchainImageViews.push_back(imgView);
	}

	_swapchainImageFormat = vk::Format(vkbSwapchain.image_format);
	_renderResolution = { vkbSwapchain.extent.width, vkbSwapchain.extent.height };
	if (addDeletors)
		_mainDeletionQueue.addDeletor([&]() {
		vkDestroySwapchainKHR(_device, _swapchain, nullptr);
	});

	//depth image size will match the window
	vk::Extent3D depthImageExtent = {
		.width = _renderResolution.x,
		.height = _renderResolution.y,
		.depth = 1
	};

	//hardcoding the depth format to 32 bit float
	_depthFormat = vk::Format::eD32Sfloat;
	//the depth image will be an image with the format we selected and Depth Attachment usage flag
	const vk::ImageCreateInfo imgInfo{
		.sType = vk::StructureType::eImageCreateInfo,
		.pNext = nullptr,
		.imageType = vk::ImageType::e2D,
		.format = _depthFormat,
		.extent = depthImageExtent,
		.mipLevels = 1,
		.arrayLayers = 1,
		.samples = vk::SampleCountFlagBits::e1,
		.tiling = vk::ImageTiling::eOptimal,
		.usage = vk::ImageUsageFlagBits::eDepthStencilAttachment,
	};

	//for the depth image, we want to allocate it from GPU local memory
	vma::AllocationCreateInfo imgAllocInfo = {
		.usage = vma::MemoryUsage::eAuto,
		.requiredFlags = vk::MemoryPropertyFlagBits::eDeviceLocal
	};

	//allocate and create the image
	_memoryAllocator.createImage(&imgInfo, &imgAllocInfo, &_depthImage.image, &_depthImage.allocation, nullptr);

	//build an image-view for the depth image to use for rendering
	const vk::ImageViewCreateInfo imgViewInfo{
		.sType = vk::StructureType::eImageViewCreateInfo,
		.pNext = nullptr,
		.image = _depthImage.image,
		.viewType = vk::ImageViewType::e2D,
		.format = _depthFormat,
		.subresourceRange = {
			.aspectMask = vk::ImageAspectFlagBits::eDepth,
			.baseMipLevel = 0,
			.levelCount = 1,
			.baseArrayLayer = 0,
			.layerCount = 1
		}
	};

	Helpers::checkVulkanError(_device.createImageView(&imgViewInfo, nullptr, &_depthImageView), "creating an image view");

	//add to deletion queue
	if (addDeletors)
		_mainDeletionQueue.addDeletor([&]() {
		vkDestroyImageView(_device, _depthImageView, nullptr);
		vmaDestroyImage(_memoryAllocator, _depthImage.image, _depthImage.allocation);
	});
}

void Lullaby::Renderer::initCommands() {
	//create a command pool for commands submitted to the graphics queue.
	//the command pool will be one that can submit graphics commands
	//we also want the pool to allow for resetting of individual command buffers
	vk::CommandPoolCreateInfo commandPoolInfo = {
		.sType = vk::StructureType::eCommandPoolCreateInfo,
		.pNext = nullptr,
		.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
		.queueFamilyIndex = _graphicsQueueFamily
	};

	//Create graphics command pool
	Helpers::checkVulkanError(_device.createCommandPool(&commandPoolInfo, nullptr, &_graphicsCommandPool), "creating graphics command pool");

	commandPoolInfo.queueFamilyIndex = _computeQueueFamily;

	//Create compute command pool
	Helpers::checkVulkanError(_device.createCommandPool(&commandPoolInfo, nullptr, &_computeCommandPool), "creating compute command pool");

	//allocate the default command buffer that we will use for rendering
	//commands will be made from our _graphicsCommandPool
	//we will allocate 1 command buffer
	// command level is Primary
	vk::CommandBufferAllocateInfo commandAllocInfo = {
		.sType = vk::StructureType::eCommandBufferAllocateInfo,
		.pNext = nullptr,
		.commandPool = _graphicsCommandPool,
		.level = vk::CommandBufferLevel::ePrimary,
		.commandBufferCount = 1
	};

	Helpers::checkVulkanError(_device.allocateCommandBuffers(&commandAllocInfo, &_graphicsCommandBuffer), "creating graphics command buffer");
	_mainDeletionQueue.addDeletor([=]() {
		_device.destroyCommandPool(_graphicsCommandPool);
	});

	commandAllocInfo.commandPool = _computeCommandPool;
	Helpers::checkVulkanError(_device.allocateCommandBuffers(&commandAllocInfo, &_computeCommandBuffer), "creating compute command buffer");
	_mainDeletionQueue.addDeletor([=]() {
		_device.destroyCommandPool(_computeCommandPool);
	});
}

void Lullaby::Renderer::initFramebuffers(bool addDeletors) {
	//create the framebuffers for the swapchain images. This will connect the render-pass to the images for rendering
	vk::FramebufferCreateInfo fbInfo = {
		.sType = vk::StructureType::eFramebufferCreateInfo,
		.pNext = nullptr,
		.renderPass = _mainRenderPass,
		.attachmentCount = 2,
		.width = _renderResolution.x,
		.height = _renderResolution.y,
		.layers = 1
	};

	//grab how many images we have in the swapchain
	const u32 swapchainImageCount = _swapchainImages.size();
	_framebuffers = std::vector<vk::Framebuffer>(swapchainImageCount);

	//create framebuffers for each of the swapchain image views
	for (int i = 0; i < swapchainImageCount; i++) {
		vk::ImageView attachments[2] = { _swapchainImageViews[i], _depthImageView };
		fbInfo.pAttachments = attachments;
		Helpers::checkVulkanError(_device.createFramebuffer(&fbInfo, nullptr, &_framebuffers[i]), "creating a framebuffer");
		if (addDeletors)
			_mainDeletionQueue.addDeletor([&]() {
			_device.destroyFramebuffer(_framebuffers[i]);
			_device.destroyImageView(_swapchainImageViews[i]);
		});
	}

}

void Lullaby::Renderer::initDefaultRenderpass() {
	// the renderpass will use this color attachment.		
	const vk::AttachmentDescription colorAttachment = {
		.format = _swapchainImageFormat, //the attachment will have the format needed by the swapchain
		.samples = vk::SampleCountFlagBits::e1,//1 sample, we won't be doing MSAA for now
		.loadOp = vk::AttachmentLoadOp::eClear, // we Clear when this attachment is loaded
		.storeOp = vk::AttachmentStoreOp::eStore, // we keep the attachment stored when the renderpass ends
		.stencilLoadOp = vk::AttachmentLoadOp::eDontCare, //we don't care about stencil	
		.stencilStoreOp = vk::AttachmentStoreOp::eDontCare,
		.initialLayout = vk::ImageLayout::eUndefined, //we don't know nor care about the starting layout of the attachment
		.finalLayout = vk::ImageLayout::ePresentSrcKHR //after the renderpass ends, the image has to be on a layout ready for display
	};

	const vk::AttachmentReference colorAttachmentRef = {
		//attachment number will index into the pAttachments array in the parent renderpass itself
		.attachment = 0,
		.layout = vk::ImageLayout::eColorAttachmentOptimal
	};

	// Depth render pass description.
	const vk::AttachmentDescription depthAttachment{
		.format = _depthFormat,
		.samples = vk::SampleCountFlagBits::e1,
		.loadOp = vk::AttachmentLoadOp::eClear,
		.storeOp = vk::AttachmentStoreOp::eStore,
		.stencilLoadOp = vk::AttachmentLoadOp::eClear,
		.stencilStoreOp = vk::AttachmentStoreOp::eStore,
		.initialLayout = vk::ImageLayout::eUndefined,
		.finalLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal
	};

	const vk::AttachmentReference depthAttachmentRef = {
		//attachment number will index into the pAttachments array in the parent renderpass itself
		.attachment = 1,
		.layout = vk::ImageLayout::eDepthStencilAttachmentOptimal
	};

	vk::SubpassDescription subpass = {
		.pipelineBindPoint = vk::PipelineBindPoint::eGraphics,
		.colorAttachmentCount = 1,
		.pColorAttachments = &colorAttachmentRef,
		.pDepthStencilAttachment = &depthAttachmentRef
	};

	//array of 2 attachments, one for the color, and other for depth
	vk::AttachmentDescription attachments[2] = { colorAttachment, depthAttachment };

	vk::SubpassDependency colorDependency = {
		.srcSubpass = VK_SUBPASS_EXTERNAL,
		.dstSubpass = 0,
		.srcStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput,
		.dstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput,
		.srcAccessMask = vk::AccessFlagBits::eNone,
		.dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite
	};

	vk::SubpassDependency depthDependency = {
		.srcSubpass = VK_SUBPASS_EXTERNAL,
		.dstSubpass = 0,
		.srcStageMask = vk::PipelineStageFlagBits::eEarlyFragmentTests | vk::PipelineStageFlagBits::eLateFragmentTests,
		.dstStageMask = vk::PipelineStageFlagBits::eEarlyFragmentTests | vk::PipelineStageFlagBits::eLateFragmentTests,
		.srcAccessMask = vk::AccessFlagBits::eNone,
		.dstAccessMask = vk::AccessFlagBits::eDepthStencilAttachmentWrite
	};

	vk::SubpassDependency dependencies[2] = { colorDependency, depthDependency };

	const vk::RenderPassCreateInfo renderPassInfo{
		.sType = vk::StructureType::eRenderPassCreateInfo,
		.attachmentCount = 2,
		.pAttachments = attachments,
		.subpassCount = 1,
		.pSubpasses = &subpass,
		.dependencyCount = 2,
		.pDependencies = dependencies
	};

	Helpers::checkVulkanError(_device.createRenderPass(&renderPassInfo, nullptr, &_mainRenderPass), "creating the main render pass");

	_mainDeletionQueue.addDeletor([=]() {
		_device.destroyRenderPass(_mainRenderPass);
	});
}

void Lullaby::Renderer::initSyncStructures() {
	//create synchronization structures

	vk::FenceCreateInfo fenceCreateInfo = {
		.sType = vk::StructureType::eFenceCreateInfo,
		.pNext = nullptr,
		//we want to create the fence with the Create Signaled flag, so we can wait on it before using it on a GPU command (for the first frame)
		.flags = vk::FenceCreateFlagBits::eSignaled
	};

	Helpers::checkVulkanError(_device.createFence(&fenceCreateInfo, nullptr, &_renderFence), "creating the main render fence");

	//for the semaphores we don't need any flags
	const vk::SemaphoreCreateInfo semaphoreCreateInfo = {
		.sType = vk::StructureType::eSemaphoreCreateInfo,
		.pNext = nullptr
	};

	Helpers::checkVulkanError(_device.createSemaphore(&semaphoreCreateInfo, nullptr, &_presentSemaphore), "creating presentation semaphore");
	Helpers::checkVulkanError(_device.createSemaphore(&semaphoreCreateInfo, nullptr, &_renderSemaphore), "creating rendering semaphore");
	_mainDeletionQueue.addDeletor([=]() {
		_device.destroySemaphore(_presentSemaphore);
		_device.destroySemaphore(_renderSemaphore);
	});
}

void Lullaby::Renderer::initPipelines() {
	const VkShaderModule triangleVertexShader = PipelineBuilder::loadShaderModule(
		_device, "../LullabyCore/Source/Lullaby/Shaders/TriangleVertex.spv");

	if (triangleVertexShader)
		fmt::print(fg(fmt::color::green), "Triangle vertex shader successfully loaded!\n");
	else
		fmt::print(stderr, fg(fmt::color::red), "Error when building the triangle vertex shader module\n");


	const VkShaderModule triangleFragShader = PipelineBuilder::loadShaderModule(
		_device, "../LullabyCore/Source/Lullaby/Shaders/TriangleFragment.spv");

	if (triangleFragShader)
		fmt::print(fg(fmt::color::green), "Triangle fragment shader successfully loaded!\n");
	else
		fmt::print(stderr, fg(fmt::color::red), "Error when building the triangle fragment shader module\n");

	//we start from just the default empty pipeline layout info
	auto layoutInfo = PipelineBuilder::defaultLayoutInfo();

	//setup push constants
	vk::PushConstantRange pushConstant{
		//this push constant range is accessible only in the vertex shader
		.stageFlags = vk::ShaderStageFlagBits::eVertex,
		//this push constant range starts at the beginning
		.offset = 0,
		//this push constant range takes up the size of a MeshPushConstants struct
		.size = sizeof(MeshPushConstants)
	};

	layoutInfo.pPushConstantRanges = &pushConstant;
	layoutInfo.pushConstantRangeCount = 1;

	Helpers::checkVulkanError(_device.createPipelineLayout(&layoutInfo, nullptr, &_meshPipelineLayout), "creating pipeline layout");

	auto vertexInputInfo = PipelineBuilder::defaultVertexInputInfo();
	auto vertexDescription = Vertex::getVertexDescription();
	vertexInputInfo.pVertexBindingDescriptions = vertexDescription.bindings.data();
	vertexInputInfo.vertexBindingDescriptionCount = vertexDescription.bindings.size();
	vertexInputInfo.pVertexAttributeDescriptions = vertexDescription.attributes.data();
	vertexInputInfo.vertexAttributeDescriptionCount = vertexDescription.attributes.size();

	PipelineInfo info{
		._shaderStages = {
			PipelineBuilder::defaultShaderStageInfo(vk::ShaderStageFlagBits::eVertex, triangleVertexShader),
			PipelineBuilder::defaultShaderStageInfo(vk::ShaderStageFlagBits::eFragment, triangleFragShader)
		},
		._vertexInputInfo = vertexInputInfo,
		._inputAssembly = PipelineBuilder::defaultInputAssemblyInfo(vk::PrimitiveTopology::eTriangleList),
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
		._rasterizer = PipelineBuilder::defaultRasterizationInfo(vk::PolygonMode::eFill),
		._colorBlendAttachment = PipelineBuilder::defaultColorBlendState(),
		._depthStencil = Helpers::depthStencilCreateInfo(true, true, vk::CompareOp::eLessOrEqual),
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

void Lullaby::Renderer::render() {
	//wait until the GPU has finished rendering the last frame. Timeout of 1 second
	Helpers::checkVulkanError(_device.waitForFences(1, &_renderFence, true, 1000000000), "waiting for render fence");
	Helpers::checkVulkanError(_device.resetFences(1, &_renderFence), "reseting render fence");

	//request image from the swapchain, one second timeout
	u32 swapchainImageIndex;
	Helpers::checkVulkanError(_device.acquireNextImageKHR(_swapchain, 1000000000, _presentSemaphore, nullptr, &swapchainImageIndex), "acquiring next image from swapchain");

	_graphicsCommandBuffer.reset();
	//Helpers::checkVulkanError(, "reseting command buffer");

	//Begin the command buffer recording. We will use this command buffer exactly once, so we want to let Vulkan know that
	const vk::CommandBufferBeginInfo cmdBeginInfo = {
		.sType = vk::StructureType::eCommandBufferBeginInfo,
		.pNext = nullptr,
		.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit,
		.pInheritanceInfo = nullptr
	};

	Helpers::checkVulkanError(_graphicsCommandBuffer.begin(&cmdBeginInfo), "beginning command buffer recording");

	std::array<vk::ClearValue, 2> clearValues;
	clearValues[0].color = { 0.05f, 0.05f, 0.05f, 1.0f };
	clearValues[1].depthStencil.depth = 1.f;

	//start the main renderpass.
	//We will use the clear color from above, and the framebuffer of the index the swapchain gave us
	const vk::RenderPassBeginInfo rpInfo = {
		.sType = vk::StructureType::eRenderPassBeginInfo,
		.pNext = nullptr,
		.renderPass = _mainRenderPass,
		.framebuffer = _framebuffers[swapchainImageIndex],
		.renderArea = {
			.offset = {0,0},
			.extent = {_renderResolution.x, _renderResolution.y}
		},
		.clearValueCount = 2,
		.pClearValues = clearValues.data(),
	};
	_graphicsCommandBuffer.beginRenderPass(&rpInfo, vk::SubpassContents::eInline);
	_graphicsCommandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, _meshPipeline);

	//make a model view matrix for rendering the object
	//camera position
	glm::vec3 camPos = { 0.f,-5.f,-20.f };

	glm::mat4 view = glm::translate(glm::mat4(1.f), camPos);
	//camera projection
	glm::mat4 projection = glm::perspective(glm::radians(70.f), 1700.f / 900.f, 0.1f, 200.0f);
	projection[1][1] *= -1;

	//calculate final mesh matrix
	glm::mat4 camMatrix = projection * view;

	MeshPushConstants constants;
	//constants.renderMatrix = camMatrix;


	//bind the mesh vertex buffer with offset 0
	vk::DeviceSize offset = 0;
	/*_mainCommandBuffer.bindVertexBuffers(0, 1, &_dragon._vertexBuffer.buffer, &offset);
	_mainCommandBuffer.draw(_dragon._vertices.size(), 1, 0, 0);*/

	//Collects entities meshes from world and render them
	WorldManager::ptr->getGameWorld().each([&](flecs::entity e, Transform& transform, MeshData& meshData) {
		constants.renderMatrix = camMatrix * transform.getMatrix();
		//upload the matrix to the GPU via push constants
		_graphicsCommandBuffer.pushConstants(_meshPipelineLayout, vk::ShaderStageFlagBits::eVertex, 0, sizeof(MeshPushConstants), &constants);
		//bind the mesh vertex buffer with offset 0
		_graphicsCommandBuffer.bindVertexBuffers(0, 1, &meshData.vertexBuffer.buffer, &offset);
		_graphicsCommandBuffer.draw(meshData.vertex.size(), 1, 0, 0);
	});


	//finalize the render pass
	_graphicsCommandBuffer.endRenderPass();
	_graphicsCommandBuffer.end();
	//Helpers::checkVulkanError(_mainCommandBuffer.end(), "ending command buffer");

	//prepare the submission to the queue.
	//we want to wait on the _presentSemaphore, as that semaphore is signaled when the swapchain is ready
	//we will signal the _renderSemaphore, to signal that rendering has finished
	vk::PipelineStageFlags waitStage = vk::PipelineStageFlagBits::eColorAttachmentOutput;
	const vk::SubmitInfo submit = {
		.sType = vk::StructureType::eSubmitInfo,
		.pNext = nullptr,
		.waitSemaphoreCount = 1,
		.pWaitSemaphores = &_presentSemaphore,
		.pWaitDstStageMask = &waitStage,
		.commandBufferCount = 1,
		.pCommandBuffers = &_graphicsCommandBuffer,
		.signalSemaphoreCount = 1,
		.pSignalSemaphores = &_renderSemaphore,
	};

	Helpers::checkVulkanError(_graphicsQueue.submit(1, &submit, _renderFence), "submitting  work to graphics queue");

	// this will put the image we just rendered into the visible window.
	// we want to wait on the _renderSemaphore for that,
	// as it's necessary that drawing commands have finished before the image is displayed to the user
	const vk::PresentInfoKHR presentInfo = {
		.sType = vk::StructureType::ePresentInfoKHR,
		.pNext = nullptr,
		.waitSemaphoreCount = 1,
		.pWaitSemaphores = &_renderSemaphore,
		.swapchainCount = 1,
		.pSwapchains = &_swapchain,
		.pImageIndices = &swapchainImageIndex,
	};
	Helpers::checkVulkanError(_graphicsQueue.presentKHR(&presentInfo), "presenting image");

}

void Lullaby::Renderer::uploadGeometry(MeshData& mesh) {
	//allocate vertex buffer
	vk::BufferCreateInfo bufferInfo = {
		.sType = vk::StructureType::eBufferCreateInfo,
		.size = mesh.vertex.size() * sizeof(vec3),
		.usage = vk::BufferUsageFlagBits::eVertexBuffer,
		.sharingMode = vk::SharingMode::eExclusive
	};

	//let the VMA library know that this data should be writeable by CPU, but also readable by GPU
	vma::AllocationCreateInfo vmaallocInfo = {
		.usage = vma::MemoryUsage::eCpuToGpu,
	};

	//allocate the buffer
	Helpers::checkVulkanError(_memoryAllocator.createBuffer(&bufferInfo, &vmaallocInfo,
		&mesh.vertexBuffer.buffer,
		&mesh.vertexBuffer.allocation,
		nullptr), "creating a memory buffer in GPU");

	//add the destruction of triangle mesh buffer to the deletion queue
	_mainDeletionQueue.addDeletor([=]() {
		_memoryAllocator.destroyBuffer(mesh.vertexBuffer.buffer, mesh.vertexBuffer.allocation);
		});

	void* data;
	_memoryAllocator.mapMemory(mesh.vertexBuffer.allocation, &data);
	memcpy(data, mesh.vertex.data(), mesh.vertex.size() * sizeof(vec3));
	_memoryAllocator.unmapMemory(mesh.vertexBuffer.allocation);
}

void Lullaby::Renderer::resizeCallback(GLFWwindow* window, int width, int height) {
	auto rendererPtr = getInstance();
	if (rendererPtr->_swapchain) {
		rendererPtr->_device.waitForFences(1, &rendererPtr->_renderFence, true, 10000000);
		for (int i = 0; i < rendererPtr->_swapchainImageViews.size(); i++) {
			vkDestroyFramebuffer(rendererPtr->_device, rendererPtr->_framebuffers[i], nullptr);
			vkDestroyImageView(rendererPtr->_device, rendererPtr->_swapchainImageViews[i], nullptr);
		}
		vkDestroyImageView(rendererPtr->_device, rendererPtr->_depthImageView, nullptr);
		vmaDestroyImage(rendererPtr->_memoryAllocator, rendererPtr->_depthImage.image, rendererPtr->_depthImage.allocation);
		vkDestroySwapchainKHR(rendererPtr->_device, rendererPtr->_swapchain, nullptr);
		rendererPtr->_renderResolution.x = width;
		rendererPtr->_renderResolution.y = height;
		rendererPtr->initSwapchain({ width, height }, false);
		rendererPtr->initFramebuffers(false);
	}
}

void Lullaby::Renderer::releaseResources() {
	if (_isInitialized) {
		//make sure the GPU has stopped doing its things
		_device.waitForFences(1, &_renderFence, true, 1000000000);

		_mainDeletionQueue.flush();

		vkDestroyDevice(_device, nullptr);
		//VkPhysicalDevice is not a resource per-se, as it's a handle to system GPU. So, you can't destroy it.
		vkDestroySurfaceKHR(_instance, _surface, nullptr);
		vkb::destroy_debug_utils_messenger(_instance, _debugMesseger);
		vkDestroyInstance(_instance, nullptr);
	} else {
		fmt::print(fg(fmt::color::yellow), "The renderer is not initialized, so you can't release resources\n");
	}
}

Lullaby::Renderer::~Renderer() {
	releaseResources();
}
