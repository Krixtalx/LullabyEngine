#pragma once
#include "Types.h"

namespace Lullaby {
	struct VertexInputDescription {

		std::vector<VkVertexInputBindingDescription> _bindings;
		std::vector<VkVertexInputAttributeDescription> _attributes;

		VkPipelineVertexInputStateCreateFlags _flags = 0;
	};
	struct Vertex {
		glm::vec3 _position;
		glm::vec3 _normal;
		glm::vec3 _color;

		static VertexInputDescription getVertexDescription();
	};

	class Mesh {

	public:
		std::vector<Vertex> _vertices;
		Types::AllocatedBuffer _vertexBuffer;
	};
}
