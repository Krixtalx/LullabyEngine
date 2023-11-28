#pragma once
#include "Types.h"

namespace Lullaby {
	struct VertexInputDescription {

		std::vector<vk::VertexInputBindingDescription> _bindings;
		std::vector<vk::VertexInputAttributeDescription> _attributes;

		vk::PipelineVertexInputStateCreateFlags _flags;
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

		bool loadFromObj(const std::string& filename);
	};
}
