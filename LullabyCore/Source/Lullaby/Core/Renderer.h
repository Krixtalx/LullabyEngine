#pragma once
#include "GLFW/glfw3.h"
#include "Lullaby/Utilities/Singleton.h"
#include "vk_mem_alloc.hpp"
#include "DeletionQueue.h"
#include "DataContainers/Mesh.h"

struct MeshPushConstants {
	glm::vec4 data;
	glm::mat4 renderMatrix;
};

namespace Lullaby {
	class Renderer final : public Singleton<Renderer> {
		//Vulkan objects
		vk::Instance _instance = nullptr; //Vulkan instance. 
		VkDebugUtilsMessengerEXT _debugMesseger = nullptr;
		vk::PhysicalDevice _choosenGPU = nullptr;
		vk::Device _device = nullptr; //Vulkan device for commands
		vk::Queue _graphicsQueue = nullptr; //queue we will submit to
		uint32_t _graphicsQueueFamily = 0; //family of that queue
		vk::Queue _computeQueue = nullptr; //queue we will submit to
		uint32_t _computeQueueFamily = 0; //family of that queue

		//Swapchain
		vk::SurfaceKHR _surface = nullptr;
		vk::SwapchainKHR _swapchain = nullptr;
		vk::Format _swapchainImageFormat;
		std::vector<vk::Image> _swapchainImages;
		std::vector<vk::ImageView> _swapchainImageViews;
		uvec2 _renderResolution = {};

		//GPU Commands
		vk::CommandPool _graphicsCommandPool = nullptr; //the command pool for our graphics commands
		//vk::CommandPool _computeCommandPool; //the command pool for our compute commands
		vk::CommandBuffer _mainCommandBuffer = nullptr; //the buffer we will record into

		//Render pass
		vk::RenderPass _mainRenderPass = nullptr;
		std::vector<vk::Framebuffer> _framebuffers;

		//Sync
		vk::Semaphore _presentSemaphore = nullptr, _renderSemaphore = nullptr;
		vk::Fence _renderFence = nullptr;

		//Pipelines
		vk::Pipeline _meshPipeline = nullptr;
		vk::PipelineLayout _meshPipelineLayout = nullptr;

		//Memory allocator
		vma::Allocator _memoryAllocator = nullptr;

		//Deletion queue
		DeletionQueue _mainDeletionQueue;

		//Depth buffer
		vk::ImageView _depthImageView = nullptr;
		Types::AllocatedImage _depthImage;
		vk::Format _depthFormat;

		//Sample model
		Mesh _sampleMesh;
		Mesh _dragon;

		bool _isInitialized = false;
		uint64_t _frameNumber = 0;
	public:
		void initRenderer(GLFWwindow* window);
		void initSwapchain(ivec2 windowSize, bool addDeletors = true);
		void initCommands();
		void initFramebuffers(bool addDeletors = true);
		void initDefaultRenderpass();
		void initSyncStructures();
		void initPipelines();

		void render();

		void sampleTriangle();
		void sampleModel();
		void uploadGeometry(Mesh& mesh);

		static void resizeCallback(GLFWwindow* window, int width, int height);
		void releaseResources();
		~Renderer();
	};
}
