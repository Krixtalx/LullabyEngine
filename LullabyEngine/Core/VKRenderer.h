#pragma once

#include "VkBootstrap.h"

namespace Lullaby {
	class VKRenderer : public Singleton<VKRenderer> {
		//Vulkan objects
		VkInstance _instance; //Vulkan instance. 
		VkDebugUtilsMessengerEXT _debugMesseger;
		VkPhysicalDevice _chosenGPU;
		VkDevice _device; //Vulkan device for commands
		VkQueue _graphicsQueue; //queue we will submit to
		uint32_t _graphicsQueueFamily; //family of that queue
		VkQueue _computeQueue; //queue we will submit to
		uint32_t _computeQueueFamily; //family of that queue

		//Swapchain
		VkSurfaceKHR _surface;
		VkSwapchainKHR _swapchain;
		VkFormat _swapchainImageFormat;
		std::vector<VkImage> _swapchainImages;
		std::vector<VkImageView> _swapchainImageViews;
		ivec2 _renderResolution;

		//GPU Commands
		VkCommandPool _graphicsCommandPool; //the command pool for our graphics commands
		//VkCommandPool _computeCommandPool; //the command pool for our graphics commands
		VkCommandBuffer _mainCommandBuffer; //the buffer we will record into

		//Render pass
		VkRenderPass _mainRenderPass;
		std::vector<VkFramebuffer> _framebuffers;

		bool _isInitialized = false;

	public:
		void initRenderer(GLFWwindow* window);
		void initSwapchain(const ivec2 windowSize);
		void initCommands();
		void initFramebuffers();
		void initDefaultRenderpass();
		void releaseResources();
		virtual ~VKRenderer();
	};
}