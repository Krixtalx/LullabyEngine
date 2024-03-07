#pragma once
#include "Lullaby/ECS/Components/Vulkan/GPUData.h"

namespace Lullaby {
	struct VertexInputDescription {

		std::vector<vk::VertexInputBindingDescription> bindings;
		std::vector<vk::VertexInputAttributeDescription> attributes;

		vk::PipelineVertexInputStateCreateFlags flags;
	};
	struct Vertex {
		glm::vec3 _position;
		glm::vec3 _normal;
		glm::vec3 _color;

		static VertexInputDescription getVertexDescription();
	};
}
