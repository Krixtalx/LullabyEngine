#pragma once
#include <vk_mem_alloc.h>

namespace Lullaby::Types {
	struct AllocatedBuffer {
		VkBuffer _buffer = nullptr;
		VmaAllocation _allocation = nullptr;
	};

	struct AllocatedImage
	{
		VkImage _image = nullptr;
		VmaAllocation _allocation = nullptr;
	};
}
