#pragma once
#include "Vulkan/GPUData.h"
#include "Lullaby/Typedefs.h"

namespace Lullaby {
	struct MeshData {
		std::vector<vec3> vertex;
		std::vector<u32> index;
		std::vector<vec3> normals;
		std::vector<vec2> uv;

		AllocatedGpuBuffer vertexBuffer;
		AllocatedGpuBuffer indexBuffer;
		AllocatedGpuBuffer normalsBuffer;
		AllocatedGpuBuffer uvBuffer;
	};
}
