#pragma once

#include "VkBootstrap.h"

#define VK_CHECK(x)                                                 \
	do                                                              \
	{                                                               \
		VkResult err = x;                                           \
		if (err)                                                    \
		{                                                           \
			std::cout <<"Detected Vulkan error: " << err << std::endl; \
			abort();                                                \
		}                                                           \
	} while (0)

namespace Lullaby {
	class VKRenderer : public Singleton<VKRenderer> {
		//Vulkan objects
		VkInstance _instance; //Vulkan instance. 
		VkDebugUtilsMessengerEXT _debugMesseger;
		VkPhysicalDevice _chosenGPU;
		VkDevice _device; //Vulkan device for commands
		VkQueue _graphicsQueue; //queue we will submit to
		uint32_t _graphicsQueueFamily; //family of that queue

		//Swapchain
		VkSurfaceKHR _surface;
		VkSwapchainKHR _swapchain;
		VkFormat _swapchainImageFormat;
		std::vector<VkImage> _swapchainImages;
		std::vector<VkImageView> _swapchainImageViews;

		//GPU Commands
		VkCommandPool _commandPool; //the command pool for our commands
		VkCommandBuffer _mainCommandBuffer; //the buffer we will record into


		bool _isInitialized = false;

	public:
		void initRenderer(GLFWwindow* window);
		void initSwapchain(const ivec2 windowSize);
		void releaseResources();
		virtual ~VKRenderer();
	};
}