#pragma once
#include "vk_mem_alloc.hpp"

namespace Lullaby {
	struct AllocatedGpuBuffer {
		vk::Buffer buffer = nullptr;
		vma::Allocation allocation = nullptr;
	};

	struct AllocatedGpuImage {
		vk::Image image = nullptr;
		vma::Allocation allocation = nullptr;
	};
}
