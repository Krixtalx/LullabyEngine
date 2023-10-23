#include "stdafx.h"
#include "Mesh.h"

Lullaby::VertexInputDescription Lullaby::Vertex::getVertexDescription() {
	VertexInputDescription description;

	//we will have just 1 vertex buffer binding, with a per-vertex rate
	constexpr VkVertexInputBindingDescription mainBinding = {
		.binding = 0,
		.stride = sizeof(Vertex),
		.inputRate = VK_VERTEX_INPUT_RATE_VERTEX
	};

	description._bindings.push_back(mainBinding);

	const VkVertexInputAttributeDescription positionAttribute = {
		.location = 0,
		.binding = 0,
		.format = VK_FORMAT_R32G32B32_SFLOAT,
		.offset = offsetof(Vertex, _position)
	};
	description._attributes.push_back(positionAttribute);

	const VkVertexInputAttributeDescription normalAttibute = {
		.location = 1,
		.binding = 0,
		.format = VK_FORMAT_R32G32B32_SFLOAT,
		.offset = offsetof(Vertex, _normal)
	};
	description._attributes.push_back(normalAttibute);

	const VkVertexInputAttributeDescription colorAttribute = {
		.location = 2,
		.binding = 0,
		.format = VK_FORMAT_R32G32B32_SFLOAT,
		.offset = offsetof(Vertex, _color)
	};
	description._attributes.push_back(colorAttribute);

	return description;
}
