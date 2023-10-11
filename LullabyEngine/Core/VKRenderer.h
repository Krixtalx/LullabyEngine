#pragma once

#include "VkBootstrap.h"

namespace Lullaby {
	class VKRenderer : public Singleton<VKRenderer> {
		//Vulkan objects
		VkInstance _instance = nullptr; //Vulkan instance. 
		VkDebugUtilsMessengerEXT _debugMesseger = nullptr;
		VkPhysicalDevice _chosenGPU = nullptr;
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
		//VkCommandPool _computeCommandPool; //the command pool for our graphics commands
		VkCommandBuffer _mainCommandBuffer = nullptr; //the buffer we will record into

		//Render pass
		VkRenderPass _mainRenderPass = nullptr;
		std::vector<VkFramebuffer> _framebuffers;

		//Sync
		VkSemaphore _presentSemaphore = nullptr, _renderSemaphore = nullptr;
		VkFence _renderFence = nullptr;

		bool _isInitialized = false;
		uint64_t _frameNumber = 0;
	public:
		void initRenderer(GLFWwindow* window);
		void initSwapchain(ivec2 windowSize);
		void initCommands();
		void initFramebuffers();
		void initDefaultRenderpass();
		void initSyncStructures();
		void initPipelines();

		void render();

		void releaseResources() const;
		virtual ~VKRenderer();
	};
}