#pragma once

#include "DeletionQueue.h"
#include "VkBootstrap.h"
#include "vk_mem_alloc.h"
#include "DataContainers/Mesh.h"

struct MeshPushConstants {
	glm::vec4 data;
	glm::mat4 render_matrix;
};

namespace Lullaby {
	class VKRenderer : public Singleton<VKRenderer> {
		//Vulkan objects
		VkInstance _instance = nullptr; //Vulkan instance. 
		VkDebugUtilsMessengerEXT _debugMesseger = nullptr;
		VkPhysicalDevice _choosenGPU = nullptr;
		VkDevice _device = nullptr; //Vulkan device for commands
		VkQueue _graphicsQueue = nullptr; //queue we will submit to
		uint32_t _graphicsQueueFamily = 0; //family of that queue
		VkQueue _computeQueue = nullptr; //queue we will submit to
		uint32_t _computeQueueFamily = 0; //family of that queue

		//Swapchain
		VkSurfaceKHR _surface = nullptr;
		VkSwapchainKHR _swapchain = nullptr;
		VkFormat _swapchainImageFormat;
		std::vector<VkImage> _swapchainImages;
		std::vector<VkImageView> _swapchainImageViews;
		uvec2 _renderResolution = {};

		//GPU Commands
		VkCommandPool _graphicsCommandPool = nullptr; //the command pool for our graphics commands
		//VkCommandPool _computeCommandPool; //the command pool for our compute commands
		VkCommandBuffer _mainCommandBuffer = nullptr; //the buffer we will record into

		//Render pass
		VkRenderPass _mainRenderPass = nullptr;
		std::vector<VkFramebuffer> _framebuffers;

		//Sync
		VkSemaphore _presentSemaphore = nullptr, _renderSemaphore = nullptr;
		VkFence _renderFence = nullptr;

		//Pipelines
		VkPipeline _meshPipeline = nullptr;
		VkPipelineLayout _meshPipelineLayout = nullptr;

		//Memory allocator
		VmaAllocator _memoryAllocator = nullptr;

		//Deletion queue
		DeletionQueue _mainDeletionQueue;

		//Depth buffer
		VkImageView _depthImageView = nullptr;
		Types::AllocatedImage _depthImage;
		VkFormat _depthFormat;

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

		void resizeCallback(GLFWwindow* window, int width, int height);
		void releaseResources();
		virtual ~VKRenderer();
	};
}
