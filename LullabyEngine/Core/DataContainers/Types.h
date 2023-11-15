#pragma once
#include <vk_mem_alloc.h>

namespace Lullaby::Types {
	struct AllocatedBuffer {
		vk::Buffer _buffer = nullptr;
		vma::Allocation _allocation = nullptr;
	};

	struct AllocatedImage{
		vk::Image _image = nullptr;
		vma::Allocation _allocation = nullptr;
	};
}
